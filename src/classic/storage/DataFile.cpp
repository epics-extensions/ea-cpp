// $Id: DataFile.cpp,v 1.16 2007/07/03 19:48:01 kasemir Exp $
//////////////////////////////////////////////////////////////////////

// The map-template generates names of enormous length
// which will generate warnings because the debugger cannot display them:
#ifdef WIN32
#pragma warning (disable: 4786)
#endif

#include <iostream>
#include <cstring>

// tools
#include "tools/AutoPtr.h"
#include "tools/BinIO.h"
#include "tools/Conversions.h"
#include "tools/MsgLogger.h"
#include "tools/Filename.h"
#include "tools/epicsTimeHelper.h"
#include "tools/string2cp.h"

// storage
#include "storage/CtrlInfo.h"
#include "storage/DataFile.h"

// TODO: Convert to BinIO?

// #define LOG_DATAFILE

// List of all DataFiles currently open
// We assume that there aren't that many open,
// so a simple list is sufficient.
static stdList<DataFile *> open_data_files;

DataFile::DataFile(const stdString &dirname,
                   const stdString &basename,
                   const stdString &filename, bool for_write)
  : ref_count(1),
    for_write(for_write),
    is_tagged_file(false),
    filename(filename),
    dirname(dirname),
    basename(basename)

{
#ifdef LOG_DATAFILE
    LOG_MSG("DataFile %s (%c) created\n",
            filename.c_str(), (for_write?'W':'R'));
#endif
}

DataFile::~DataFile ()
{
#ifdef LOG_DATAFILE
    LOG_MSG("DataFile %s (%c) deleted\n",
            filename.c_str(), (for_write?'W':'R'));
#endif
}

DataFile *DataFile::reference(const stdString &req_dirname,
                              const stdString &req_basename, bool for_write)
{
    DataFile *datafile = 0;
    stdString dirname, basename, filename;

    Filename::build(req_dirname, req_basename, filename);
    Filename::getDirname(filename, dirname);
    Filename::getBasename(filename, basename);
#ifdef LOG_DATAFILE
    LOG_MSG("reference('%s', '%s', %s)\n",
            req_dirname.c_str(),
            req_basename.c_str(),
            (for_write ?  "read/write" : "read-only"));
    LOG_MSG("normalized: '%s' + '%s' = '%s')\n",
            dirname.c_str(), basename.c_str(), filename.c_str());
#endif
    stdList<DataFile *>::iterator i = open_data_files.begin();
    for (/**/;  i != open_data_files.end ();  ++i)
    {
        datafile = *i;
        if (datafile->getFilename() == filename  &&
            datafile->for_write == for_write)
        {
#ifdef LOG_DATAFILE
            LOG_MSG("DataFile %s (%c) is cached (%d)\n",
                    filename.c_str(),
                    (for_write?'W':'R'), datafile->ref_count);
#endif
            datafile->reference();
            // When it was put in the cache, it might
            // have been a new file.
            // But now it's one that already existed,
            // so reset is_new_file:
            datafile->is_new_file = false;
            return datafile;
        }
    }
    try
    {
        datafile = new DataFile(dirname, basename, filename, for_write);
        datafile->reopen();
        open_data_files.push_back(datafile);
        return datafile;
    }
    catch (...)
    {
        if (datafile)
            delete datafile;
        throw GenericException(__FILE__, __LINE__, "Cannot reference '%s'",
                               filename.c_str());
    }
}

// Add reference to current DataFile
DataFile *DataFile::reference()
{
    ++ref_count;
#ifdef LOG_DATAFILE
    LOG_MSG("DataFile %s referenced %d times\n",
            filename.c_str(), ref_count);
#endif
    return this;
}

// Call instead of delete:
void DataFile::release()
{
    if (ref_count <= 0)
        throw GenericException(__FILE__, __LINE__,
                               "DataFile(%s): over-released",
                               filename.c_str());
    --ref_count;
    // You might expect
    //   delete this;
    // in here, but we keep the files open
    // and cache them until close_all() is called.
#ifdef LOG_DATAFILE
    LOG_MSG("DataFile %s released, %d references\n",
            filename.c_str(), ref_count);
#endif
}

FileOffset DataFile::getSize() const
{
    if (fseek(file, 0, SEEK_END) != 0)
        throw GenericException(__FILE__, __LINE__,
                               "DataFile(%s): Cannot seek to end",
                               filename.c_str());
    long end = ftell(file);
    if (end < 0)
        throw GenericException(__FILE__, __LINE__,
                               "DataFile(%s): ftell failed",
                               filename.c_str());
    return (FileOffset) end;
}

void DataFile::reopen()
{
    is_new_file = is_tagged_file = false;
    // Try existing
    if (for_write)
        file.open(filename.c_str(), "r+b");
    else
        file.open(filename.c_str(), "rb");
    if (file)
    {   // Opened existing file. Check type
        uint32_t file_cookie;
        if (fseek(file, 0, SEEK_SET) != 0  ||
            readLong(file, &file_cookie) == false)
            throw GenericException(__FILE__, __LINE__,
                                   "DataFile(%s): Read error",
                                   filename.c_str());
        is_tagged_file = file_cookie == cookie;
#ifdef LOG_DATAFILE
        LOG_MSG("DataFile %s opened for %s\n",
                filename.c_str(), (for_write?"writing":"read-only access"));
#endif
        return;
    }
    // No file, yet.
    if (!for_write)
        throw GenericException(__FILE__, __LINE__,
                               "DataFile(%s): No existing file found.",
                               filename.c_str());
    // Create a new one.
    if (!file.open(filename.c_str(), "w+b"))
        throw GenericException(__FILE__, __LINE__,
                               "DataFile(%s): Cannot create new file.",
                               filename.c_str());
    is_new_file = true;
    if (fseek(file, 0, SEEK_SET) != 0  ||
        writeLong(file, cookie) == false)
        throw GenericException(__FILE__, __LINE__,
                               "DataFile(%s): Cannot write to file.",
                               filename.c_str());
    is_tagged_file = true;
#ifdef LOG_DATAFILE
    LOG_MSG("DataFile %s created for writing\n", filename.c_str());
#endif
}

void DataFile::show_all_sizes() {

  stdList<DataFile *>::iterator i = open_data_files.begin();
  int counter = 0;
  while (i != open_data_files.end()) {
    DataFile *df = *i;
      std::cout << counter++ << ", " 
		<< df->filename << ", " 
		<< df->getSize() << std::endl;
      ++i;

  }

}

size_t DataFile::clear_cache()
{
    size_t left = 0;
    stdList<DataFile *>::iterator i = open_data_files.begin();
    while (i != open_data_files.end())
    {
        DataFile *file = *i;
        if (file->ref_count > 0)
        {
            ++left;
            ++i;
        }
        else
        {
#           ifdef LOG_DATAFILE
            LOG_MSG("DataFile::clear_cache: closing %s\n",
                    file->filename.c_str());
#           endif
            i = open_data_files.erase(i);
            delete file;
        }
    }
    return left;
}

void DataFile::close_all()
{
    size_t left = clear_cache();
    if (left > 0)
       throw GenericException(__FILE__, __LINE__,
                              "%zu data files are still ref'ed in close_all",
                              left);
}

DataHeader *DataFile::getHeader(FileOffset offset)
{
    AutoPtr<DataHeader> header;
    try
    {   // Header ref's the datafile.
        header = new DataHeader(this);
    }
    catch (...)
    {
        throw GenericException(__FILE__, __LINE__, "Cannot allocate DataHeader");
    }
    try
    {
        header->read(offset);
        return header.release();
    }
    catch (GenericException &e)
    {
        throw GenericException(__FILE__, __LINE__,
                               "Datafile::getHeader:\n%s", e.what());
    }
}

size_t DataFile::getHeaderSize(const stdString &name,
                               DbrType dbr_type, DbrCount dbr_count,
                               size_t num_samples)
{
    size_t raw_value_size = RawValue::getSize(dbr_type, dbr_count);
    size_t buf_free = num_samples * raw_value_size;
    // 'INFO' + name + '\0' + header info + data buffer
    return 4 + name.length() + 1 +
        sizeof(DataHeader::DataHeaderData) + buf_free;
}

DataHeader *DataFile::addHeader(const stdString &name,
                                DbrType dbr_type, DbrCount dbr_count,
                                double period, size_t num_samples)
{
    AutoPtr<DataHeader> header;
    try
    {
        header = new DataHeader(this);
    }
    catch (...)
    {
        throw GenericException(__FILE__, __LINE__,
                               "DataFile(%s): Cannot alloc new header",
                               filename.c_str());
    }
    FileOffset new_offset = getSize();
    if (fseek(file, new_offset, SEEK_SET) != 0  ||
        fwrite("DATA", 4, 1, file) != 1         ||
        fwrite(name.c_str(), name.length() + 1, 1, file) != 1)
    {
        throw GenericException(__FILE__, __LINE__,
                               "DataFile::addHeader(%s, %s): Cannot write at 0x%lX",
                               filename.c_str(), name.c_str(),
                               (unsigned long) new_offset);
    }
    header->offset = new_offset + 4 + name.length() + 1;
    size_t raw_value_size = RawValue::getSize(dbr_type, dbr_count);
    header->data.dbr_type = dbr_type;
    header->data.dbr_count = dbr_count;
    header->data.period = period;
    header->data.buf_free = num_samples * raw_value_size;
    header->data.buf_size =
        header->data.buf_free + sizeof(DataHeader::DataHeaderData);
    header->write();
    // allocate data buffer by writing some marker at the end:
    uint32_t marker = 0xfacefade;
    FileOffset pos = header->offset
        + header->data.buf_size - sizeof marker;    
    if (fseek(file, pos, SEEK_SET) != 0 ||
        (FileOffset) ftell(file) != pos ||
        !writeLong(file, marker))    
        throw GenericException(__FILE__, __LINE__,
                               "DataFile::addHeader(%s, %s): "
                               "Cannot mark end of new buffer",
                               filename.c_str(), name.c_str());
    return header.release();
}

void DataFile::addCtrlInfo(const CtrlInfo &info, FileOffset &offset)
{
    if (fseek(file, 0, SEEK_END) != 0)
        throw GenericException(__FILE__, __LINE__,
                               "DataFile(%s): Seek error",
                               filename.c_str());
    offset = ftell(file);
    if (fwrite("INFO", 4, 1, file) != 1)
        throw GenericException(__FILE__, __LINE__,
                               "DataFile::addCtrlInfo(%s): Cannot tag at 0x%X\n",
                               filename.c_str(), offset);
    offset += 4;

    write(info, offset);     // info.write(this, offset);

}

DataHeader::DataHeader(DataFile *datafile)
{
    datafile->reference();
    this->datafile = datafile;
    clear();
}

DataHeader::~DataHeader()
{
    datafile->release();
}
    
void DataHeader::clear()
{
    memset(&data, 0, sizeof(struct DataHeaderData));
    offset = INVALID_OFFSET;
}

bool DataHeader::isValid()
{
    return offset != INVALID_OFFSET;
}

size_t DataHeader::available()
{
    LOG_ASSERT(isValid());
    if (data.buf_free <= 0)
        return 0;
    size_t val_size = RawValue::getSize(data.dbr_type, data.dbr_count);
    LOG_ASSERT(val_size > 0);
    return data.buf_free / val_size;
}

size_t DataHeader::capacity()
{
    size_t val_size = RawValue::getSize(data.dbr_type, data.dbr_count);
    LOG_ASSERT(val_size > 0);
    return (data.buf_size - sizeof(DataHeader::DataHeaderData))
            / val_size;
}

void DataHeader::read(FileOffset offset)
{
    this->offset = offset;
    if (fseek(datafile->file, offset, SEEK_SET) != 0   ||
        (FileOffset)ftell(datafile->file) != offset  ||
        fread(&data, sizeof(struct DataHeaderData), 1, datafile->file) != 1)
    {
        clear();
        throw GenericException(__FILE__, __LINE__,
                               "Data header read error '%s' @ 0x%08lX",
                               datafile->getFilename().c_str(),
                               (unsigned long) offset);
    }
    // convert the data header into host format:
    FileOffsetFromDisk(data.dir_offset);
    FileOffsetFromDisk(data.next_offset);
    FileOffsetFromDisk(data.prev_offset);
    FileOffsetFromDisk(data.curr_offset);
    ULONGFromDisk(data.num_samples);
    FileOffsetFromDisk(data.ctrl_info_offset);
    ULONGFromDisk(data.buf_size);
    ULONGFromDisk(data.buf_free);
    USHORTFromDisk(data.dbr_type);
    USHORTFromDisk(data.dbr_count);
    DoubleFromDisk(data.period);
    // Would like to give error messages for bad time stamps,
    // but just printing them messes the data server up,
    // and exceptions would result in no data.
    // The flat zero nanoseconds are often an indicator
    // for bad time stamps, though.
    safeEpicsTimeStampFromDisk(data.begin_time);
    safeEpicsTimeStampFromDisk(data.next_file_time);
    safeEpicsTimeStampFromDisk(data.end_time);
}

void DataHeader::write() const
{
    DataHeaderData copy = data;
    // convert the data header into host format:
    FileOffsetToDisk(copy.dir_offset);
    FileOffsetToDisk(copy.next_offset);
    FileOffsetToDisk(copy.prev_offset);
    FileOffsetToDisk(copy.curr_offset);
    ULONGToDisk(copy.num_samples);
    FileOffsetToDisk(copy.ctrl_info_offset);
    ULONGToDisk(copy.buf_size);
    ULONGToDisk(copy.buf_free);
    USHORTToDisk(copy.dbr_type);
    USHORTToDisk(copy.dbr_count);
    DoubleToDisk(copy.period);
    epicsTimeStampToDisk(copy.begin_time);
    epicsTimeStampToDisk(copy.next_file_time);
    epicsTimeStampToDisk(copy.end_time);
    if (!(fseek(datafile->file, offset, SEEK_SET) == 0 &&
          (FileOffset) ftell(datafile->file) == offset  &&
          fwrite(&copy, sizeof(struct DataHeaderData), 1, datafile->file) == 1))
        throw GenericException(__FILE__, __LINE__,
                               "Data header write error '%s' @ 0x%08lX",
                               datafile->getFilename().c_str(),
                               (unsigned long) offset);

}

bool DataHeader::read_next()
{
    if (offset == INVALID_OFFSET)
        return false;
    return get_prev_next(data.next_file, data.next_offset);
}

bool DataHeader::read_prev()
{
    if (offset == INVALID_OFFSET)
        return false;
    return get_prev_next(data.prev_file, data.prev_offset);
}

bool DataHeader::get_prev_next(const char *name, FileOffset new_offset)
{
    if (new_offset == INVALID_OFFSET ||
        !Filename::isValid(name))
    {
        clear();
        return false;
    }
    // Do we need to switch data files?
    if (strcmp(datafile->basename.c_str(), name))
    {
        DataFile *next = DataFile::reference(
            datafile->dirname,
            name, datafile->for_write);
        datafile->release();
        datafile = next;
    }
    read(new_offset);
    return true;
}

void DataHeader::set_prev(const stdString &basename, FileOffset offset)
{
    string2cp(data.prev_file, basename, FilenameLength);
    data.prev_offset = offset;
}
     
void DataHeader::set_next(const stdString &basename, FileOffset offset)
{
    string2cp(data.next_file, basename, FilenameLength);
    data.next_offset = offset;
}

void DataHeader::show(FILE *f, bool full_detail)
{
    size_t val_size = RawValue::getSize(data.dbr_type, data.dbr_count);
    size_t maxcount = (val_size > 0) ?
        (data.buf_size - sizeof(DataHeader::DataHeaderData)) / val_size : 0;
    stdString t;
    if (full_detail)
    {
        fprintf(f, "Buffer  : '%s' @ 0x%lX\n",
                datafile->basename.c_str(), (unsigned long)offset);
        fprintf(f, "Prev    : '%s' @ 0x%lX\n",
                data.prev_file, (unsigned long)data.prev_offset);
        epicsTime2string(data.begin_time, t);
        fprintf(f, "Time    : %s\n", t.c_str());
        epicsTime2string(data.end_time, t);
        fprintf(f, "...     : %s\n", t.c_str());
        epicsTime2string(data.next_file_time, t);
        fprintf(f, "New File: %s\n", t.c_str());
        fprintf(f, "DbrType : %d, %d elements (%lu bytes/sample)\n",
                data.dbr_type, data.dbr_count,
                (unsigned long) val_size);
        fprintf(f, "Samples : %lu used out of %lu\n",
                (unsigned long)data.num_samples, (unsigned long)maxcount);
        fprintf(f, "Size    : %ld bytes, free: %ld bytes"
                " (header: %zu bytes)\n",
                (long)data.buf_size, (long)data.buf_free,
                sizeof(DataHeader::DataHeaderData));
        fprintf(f, "Curr.   : %ld\n", (unsigned long)data.curr_offset);
        fprintf(f, "Period  : %g\n", data.period);
        fprintf(f, "CtrlInfo@ 0x%lX\n", (unsigned long)data.ctrl_info_offset);
        fprintf(f, "Next    : '%s' @ 0x%lX\n",
                data.next_file, (unsigned long)data.next_offset);
    }
    else
    {
        fprintf(f, "'%s' @ 0x%lX ",
                datafile->basename.c_str(), (unsigned long)offset);
        fprintf(f, "%lu/%lu samples from ",
                (unsigned long)data.num_samples, (unsigned long)maxcount);
        epicsTime2string(data.begin_time, t);
        fprintf(f, " %s - ", t.c_str());
        epicsTime2string(data.end_time, t);
        fprintf(f, "%s\n", t.c_str());
        epicsTime2string(data.next_file_time, t);
    }
}

// Write CtrlInfo to file.
void DataFile::write(const CtrlInfo& ctrlInfo, FileOffset offset){ 

  DataFile *datafile = this;
  
  // Attention:
  // copy holds only the fixed CtrlInfo portion,  not enum strings etc.!

  // const CtrlInfoData *info = _infobuf.mem();
  const CtrlInfoData *info = ctrlInfo.getMemoryBuffer().mem();

    CtrlInfoData copy = *info;
    size_t converted;
    
    switch (copy.type) // convert to disk format
    {
    case CtrlInfo::Numeric:
            FloatToDisk(copy.value.analog.disp_high);
            FloatToDisk(copy.value.analog.disp_low);
            FloatToDisk(copy.value.analog.low_warn);
            FloatToDisk(copy.value.analog.low_alarm);
            FloatToDisk(copy.value.analog.high_warn);
            FloatToDisk(copy.value.analog.high_alarm);
            LONGToDisk (copy.value.analog.prec);
            converted = offsetof (CtrlInfoData, value) + sizeof (NumericInfo);
            break;
    case CtrlInfo::Enumerated:
            SHORTToDisk (copy.value.index.num_states);
            converted = offsetof (CtrlInfoData, value)
                + sizeof (EnumeratedInfo);
            break;
    case CtrlInfo::Invalid:
            throw GenericException(__FILE__, __LINE__,
                                   "Datafile %s: CtrlInfo for 0x%lX is "
                                   "undefined, size %d\n",
                                   datafile->getBasename().c_str(),
                                   (unsigned long)offset, info->size);
        default:
            throw GenericException(__FILE__, __LINE__,
                                   "Datafile %s: CtrlInfo for 0x%lX has "
                                   "invalid type %d, size %d\n",
                                   datafile->getBasename().c_str(),
                                   (unsigned long)offset, info->type,
                                   info->size);
    }
    SHORTToDisk(copy.size);
    SHORTToDisk(copy.type);
    if (fseek(datafile->file, offset, SEEK_SET) != 0 ||
        (FileOffset) ftell(datafile->file) != offset ||
        fwrite(&copy, converted, 1, datafile->file) != 1)
        throw GenericException(__FILE__, __LINE__,
                               "Datafile %s: Cannot write CtrlInfo @ 0x%lX\n",
                               datafile->getBasename().c_str(),
                               (unsigned long)offset);
    // only the common, minimal CtrlInfoData portion was converted,
    // the remaining strings are written from 'this'
    if (info->size > converted &&
        fwrite(((char *)info) + converted,
               info->size - converted, 1, datafile->file) != 1)
        throw GenericException(__FILE__, __LINE__,
                               "Datafile %s: Cannot write rest of "
                               "CtrlInfo @ 0x%lX\n",
                               datafile->getBasename().c_str(),
                               (unsigned long)offset);
}

// Read CtrlInfo for Binary format
//
// Especially for the original engine,
// the CtrlInfo on disk can be damaged.
// Special case of a CtrlInfo that's "too small":
// For enumerated types, an empty info is assumed.
// For other types, the current info is maintained
// so that the reader can decide to ignore the problem.
// In other cases, the type is set to Invalid
void DataFile::read(CtrlInfo& ctrlInfo, FileOffset offset)
{

  DataFile *datafile = this;
  MemoryBuffer<CtrlInfoData>& _infobuf = ctrlInfo.getMemoryBuffer();
  
    // read size field only
    uint16_t size;
    if (fseek(datafile->file, offset, SEEK_SET) != 0 ||
        (FileOffset) ftell(datafile->file) != offset ||
        fread(&size, sizeof size, 1, datafile->file) != 1)
    {
      _infobuf.mem()->type = CtrlInfo::Invalid;

        throw GenericException(__FILE__, __LINE__,
                               "Datafile %s: Cannot read size of CtrlInfo "
                               " @ 0x%lX\n",
                               datafile->getBasename().c_str(),
                               (unsigned long)offset);
    }
    SHORTFromDisk(size);
    if (size < offsetof(CtrlInfoData, value) + sizeof(EnumeratedInfo))
    {
      if (ctrlInfo.getType() == CtrlInfo::Enumerated)
        {
            LOG_MSG("CtrlInfo too small: %d, "
                    "forcing to empty enum for compatibility\n", size);
            ctrlInfo.setEnumerated (0, 0);
            return;
        }
        // keep current values for _infobuf!
        throw GenericException(__FILE__, __LINE__,
                               "Datafile %s: Incomplete CtrlInfo @ 0x%lX\n",
                               datafile->getBasename().c_str(),
                               (unsigned long)offset);
    }
    _infobuf.reserve(size+1); // +1 for possible unit string hack, see below

    CtrlInfoData *info = _infobuf.mem();

    info->size = size;
    if (info->size > _infobuf.capacity())
    {
      info->type = CtrlInfo::Invalid;
        throw GenericException(__FILE__, __LINE__,
                               "Datafile %s: CtrlInfo @ 0x%lX is too big\n",
                               datafile->getBasename().c_str(),
                               (unsigned long)offset);
    }
    // read remainder of CtrlInfo:
    if (fread(((char *)info) + sizeof size,
              info->size - sizeof size, 1, datafile->file) != 1)
    {
      info->type = CtrlInfo::Invalid;
        throw GenericException(__FILE__, __LINE__,
                               "Datafile %s: Cannot read remainder of "
                               "CtrlInfo @ 0x%lX\n",
                               datafile->getBasename().c_str(),
                               (unsigned long)offset);
    }
    // convert rest from disk format
    SHORTFromDisk(info->type);
    switch (info->type)
    {
    case CtrlInfo::Numeric:
            FloatFromDisk(info->value.analog.disp_high);
            FloatFromDisk(info->value.analog.disp_low);
            FloatFromDisk(info->value.analog.low_warn);
            FloatFromDisk(info->value.analog.low_alarm);
            FloatFromDisk(info->value.analog.high_warn);
            FloatFromDisk(info->value.analog.high_alarm);
            LONGFromDisk (info->value.analog.prec);
            {
                // Hack: some old archives are written with nonterminated
                // unit strings:
                int i, end;
                end = info->size - offsetof(CtrlInfoData, value.analog.units);
                for (i=0; i<end; ++i)
                    if (info->value.analog.units[i] == '\0')
                        return; // OK, string is terminated
                ++info->size; // include string terminator
                info->value.analog.units[end] = '\0';
            }
            break;
    case CtrlInfo::Enumerated:
            SHORTFromDisk(info->value.index.num_states);
            break;
        default:
	  info->type = CtrlInfo::Invalid;
            throw GenericException(__FILE__, __LINE__,
                                   "Datafile %s: "
                                   "CtrlInfo @ 0x%lX has invalid  type %d, "
                                   "size %d\n",
                                   datafile->getBasename().c_str(),
                                   (unsigned long)offset, info->type,
                                   info->size);
    }
}





 

