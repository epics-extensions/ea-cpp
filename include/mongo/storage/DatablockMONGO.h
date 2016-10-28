#ifndef _EA4_DATABLOCK_MONGO_H__
#define _EA4_DATABLOCK_MONGO_H__

#include <set>
#include "mongo/client/dbclient.h"

#include "tools/AutoPtr.h"
#include "storage/RTree.h"

namespace ea4 { namespace storage {

class DatablockMONGO : public RTree::Datablock {

 public:

  DatablockMONGO();

  void setArchiveName(const std::string& archiveName);

  inline const std::string& getArchiveName() const;

  bool checkPvName(const std::string& pvName);

 public:

  // Read file-interval entries without arrays of datablocks
  bool search(const std::string& pvName, const epicsTime *start);

 public:

  // RTree::Datablock API

  virtual bool isValid() const;
           
  virtual const Interval& getInterval() const;
        
  virtual bool getNextChainedBlock();
	    
  virtual bool getNextDatablock();

  virtual bool getPrevDatablock();

 protected:

  class IntervalLess {
    bool operator() (Interval x, Interval y);

  };
       
 protected:

  std::string archiveName;

 protected:

  std::string pvName;

  // file entries

  struct FileDoc {
    std::string filename; 
    Interval interval;
  };

  std::map<epicsTime, FileDoc>::iterator fid; // file entry iterator
  std::map<epicsTime, FileDoc> fileDocs;

  // datablock entries

  struct BlockDoc {
    FileOffset offset; 
    Interval interval;
  };
 
  std::map<epicsTime, BlockDoc>::iterator bid; // block iterator
  std::map<epicsTime, BlockDoc> blockDocs;

 protected:

  // inherited
  // FileOffset data_offset;        
  // std::string  data_filename;

 private:

  bool readFileDocs();
  void addFileDoc(const mongo::BSONObj& obj);
  bool selectFid(const epicsTime *start);
  bool readBlockDocs(const std::string& fileName);
  void addBlockDoc(const mongo::BSONObj& obj);
  bool selectBid(const epicsTime *start);
  void clear();

  void beginFid();
  void nextFid();
  void prevFid();
  void endFid();

  void beginBid();
  void nextBid();
  void prevBid();
  void endBid();

 private:

  static Interval emptyInterval;

 private:

  // void epicsTime2pieces(const epicsTime& t,int& secs, int& nano);
  // void pieces2epicsTime(int secs, int nano, epicsTime& t);

};

inline const std::string& DatablockMONGO::getArchiveName() const {
  return archiveName;
}


  }}

#endif
