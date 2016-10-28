#include <iostream>

// tools
#include "tools/AutoPtr.h"
#include "tools/BinIO.h"
#include "tools/MsgLogger.h"
#include "tools/Filename.h"

// index
#include "storage/IndexFile.h"
#include "storage/RTree.h"

// File Layout:
// long cookie;
// NameHash::anchor_size anchor;
// Rest handled by FileAllocator, NameHash and RTree

#define DEBUG_SOFTLINKS

// #define DEBUG_INDEXFILE

const uint32_t IndexFile::ht_size = 1009;
// const uint32_t IndexFile::ht_size = 10007;

IndexFile::IndexFile(int RTreeM) : RTreeM(RTreeM), names(fa, 4)
{
#ifdef DEBUG_INDEXFILE
    printf("IndexFile()\n");
#endif
}

IndexFile::~IndexFile()
{
    close();
#ifdef DEBUG_INDEXFILE
    printf("~IndexFile()\n");
#endif
}

void IndexFile::open(const stdString &filename, ReadWrite readwrite)
{
#ifdef DEBUG_INDEXFILE
    printf("IndexFile::open(%s)\n", filename.c_str());
#endif
    close();
    
    stdString name = filename;
    stdString linked_filename;

    std::string orig_name = filename;
    // Follow links.
    // The 'final' index file should contain path names
    // to data files which are valid relative to the index.
    // But as long as we're only looking at a link to that
    // final index, the data file paths can be wrong.
    while (Filename::getLinkedFilename(name, linked_filename))
    {
#ifdef DEBUG_SOFTLINKS
        LOG_MSG("soft link '%s' -> '%s'\n", name.c_str(), linked_filename.c_str());
#endif
        if (Filename::containsFullPath(linked_filename)){
	  name = linked_filename;
	} else {
            Filename::getDirname(name, name);
            if (name.length() > 0)
                name += '/';
            name += linked_filename;
        }
#ifdef DEBUG_SOFTLINKS
        LOG_MSG("   ==> '%s'\n", name.c_str());
#endif
    }
    
    // In case the name was a link,
    // set the name of this index to the actual target

    // setFilename(name);
    setFilename(orig_name);
    
    bool new_file = false;
    if (readwrite == ReadOnly)
        f.open(name.c_str(), "rb");
    else
    {   // Try existing file
        f.open(name.c_str(), "r+b");
        if (!f)
        {   // Create new file
            f.open(name.c_str(), "w+b");
            new_file = true;
        }   
    }
    if (!f)
        throw GenericException(__FILE__, __LINE__,
                               "Cannot %s file '%s'",
                               (new_file ? "create" : "open"),
                               name.c_str());
    // Tune these two? All 0 seems best?!
    FileAllocator::minimum_size = 0;
    FileAllocator::file_size_increment = 0;
    fa.attach(f, 4+NameHash::anchor_size, readwrite == ReadAndWrite);
    if (new_file)
    {
        if (fseek(f, 0, SEEK_SET))
            throw GenericException(__FILE__, __LINE__,
                                   "IndexFile::open(%s) seek error",
                                   filename.c_str());
        if (!writeLong(f, cookie))
            throw GenericException(__FILE__, __LINE__,
                                   "IndexFile::open(%s) cannot write cookie.",
                                   filename.c_str());
        names.init(ht_size);
        return; // OK, created new file.
    }
    // Check existing file
    uint32_t file_cookie;
    if (fseek(f, 0, SEEK_SET))
        throw GenericException(__FILE__, __LINE__,
                               "IndexFile::open(%s) seek error",     
                               filename.c_str());
    if (!readLong(f, &file_cookie))
        throw GenericException(__FILE__, __LINE__,
                               "IndexFile::open(%s) cannot read cookie.",
                               filename.c_str());
    if (file_cookie != cookie)
        throw GenericException(__FILE__, __LINE__,
                               "IndexFile::open(%s) Invalid cookie, "
                               "0x%08lX instead of 0x%08lX.",
                               filename.c_str(),
                               (unsigned long)file_cookie,
                               (unsigned long)cookie);
    names.reattach();
}

void IndexFile::close()
{
    if (f)
    {
        fa.detach();
        f.close();
#ifdef DEBUG_INDEXFILE
        printf("IndexFile::close(%s)\n", filename.c_str());
#endif
        clearFilename();
    }   
}

Index::Result *IndexFile::addChannel(const stdString &channel)
{
    Result *result = findChannel(channel);
    if (result)
        return result; // Done, found existing.
    FileOffset tree_anchor = fa.allocate(RTree::anchor_size);
    // Using AutoPtr in case e.g. tree->init throws exception.
    AutoPtr<RTree> tree;
    try
    {
        tree = new RTree(fa, tree_anchor);
    }
    catch (...)
    {
        tree.release();
        throw GenericException(__FILE__, __LINE__,
                               "Cannot allocate RTree for channel '%s'",
                               channel.c_str());
    }
    stdString tree_filename;
    tree->init(RTreeM);
    if (names.insert(channel, tree_filename, tree_anchor) == false)
        throw GenericException(__FILE__, __LINE__,
                               "Index consistency error: New channel '%s' "
                               "had no RTree but was already in name hash.",
                               channel.c_str());
    // Done, new name entry & RTree.
    return new Result(tree.release(), getDirectory());
}

Index::Result *IndexFile::findChannel(const stdString &channel) {

    AutoPtr<NameHash::Entry> entry(names.find(channel));

    if (!entry) return 0; // All OK, but channel not found.
    stdString  tree_filename = entry->getIdTxt();
    FileOffset tree_anchor = entry->getId();
    // Using AutoPtr in case e.g. tree->reattach throws exception.
    AutoPtr<RTree> tree;
    try
    {
        tree = new RTree(fa, tree_anchor);
    }
    catch (...)
    {
        tree.release();
        throw GenericException(__FILE__, __LINE__,
                               "Cannot allocate RTree for channel '%s'",
                               channel.c_str());
    }
    tree->reattach();
    return new Result(tree.release(), getDirectory());
}

class IndexFileNameIterator : public Index::NameIterator
{
public:
    IndexFileNameIterator(NameHash::Iterator *name_iter);
       
    // NameIterator interface
    virtual bool isValid() const;
    virtual const stdString &getName() const;
    virtual void  next();
private:
    AutoPtr<NameHash::Iterator> name_iter;
};

IndexFileNameIterator::IndexFileNameIterator(NameHash::Iterator *name_iter)
    : name_iter(name_iter)
{}

bool IndexFileNameIterator::isValid() const
{
    return name_iter->isValid();
}

const stdString &IndexFileNameIterator::getName() const
{
    return name_iter->getName();
}

void IndexFileNameIterator::next()
{
    name_iter->next();
}

Index::NameIterator *IndexFile::iterator()
{
    return new IndexFileNameIterator(names.iterator());
}

void IndexFile::showStats(FILE *f)
{
    names.showStats(f);
}

bool IndexFile::check(int level)
{
    printf("Checking FileAllocator...\n");
    try
    {
        if (fa.dump(level))
            printf("FileAllocator OK\n");
        else
        {
            printf("FileAllocator ERROR\n");
            return false;
        }
        AutoPtr<NameIterator> names;
        unsigned long channels = 0;
        unsigned long total_nodes=0, total_used_records=0, total_records=0;
        unsigned long nodes, records;
        
        for (names = iterator();  names->isValid();  names->next())
        {
            ++channels;
            AutoPtr<Result> result(findChannel(names->getName()));
            if (!result)
            {
                printf("%s not found\n", names->getName().c_str());
                return false;
            }
            printf(".");
            fflush(stdout);
            if (!result->getRTree()->selfTest(nodes, records))
            {
                printf("RTree for channel '%s' is broken\n",
                       names->getName().c_str());
                return false;
            }
            total_nodes += nodes;
            total_used_records += records;
            total_records += nodes * result->getRTree()->getM();
        }
        printf("\nAll RTree self-tests check out fine\n");
        printf("%ld channels\n", channels);
        printf("Total: %ld nodes, %ld records out of %ld are used (%.1lf %%)\n",
               total_nodes, total_used_records, total_records,
               total_used_records*100.0/total_records);
    }
    catch (GenericException &e)
    {
        printf("Exception:\n%s\n", e.what());
    }
    return true;
}

