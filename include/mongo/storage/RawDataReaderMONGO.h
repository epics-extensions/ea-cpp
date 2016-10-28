#ifndef _EA4_RAW_DATA_READER_MONGO_H__
#define _EA4_RAW_DATA_READER_MONGO_H__

// tools
#include "tools/ToolsConfig.h"
#include "tools/AutoPtr.h"

// storage
#include "storage/DataReader.h"
#include "storage/CtrlInfo.h"

#include "storage/DatablockMONGO.h"

namespace ea4 { namespace storage {

class RawDataReaderMONGO : public DataReader
{

public:

  /** Constructor */
  RawDataReaderMONGO();

  RawDataReaderMONGO(const std::string& archive_name);

 public:

  inline void setArchiveName(const std::string& archive_name);

 public:

  // DataReader API

  /** Destructor */
  virtual ~RawDataReaderMONGO();
    
  /** Locate data */
  virtual const RawValue::Data* find(const std::string& channel_name,
                                     const epicsTime *start);

  /** Obtain the next value. */                                
  virtual const RawValue::Data* next();

  // inline methods 

  /** Name of the channel, i.e. the one passed to find() */
  inline const std::string& getName() const;  

  /** Current value. */
  inline const RawValue::Data* get() const;
    
  /** The dbr_time_xxx type */
  inline DbrType getType() const;

  /** array size  */
  inline DbrCount getCount() const;

  /** The meta information for the channel */
  inline const CtrlInfo& getInfo() const;

  /** next() updates this if dbr_type/count changed. */ 
  inline bool changedType();

  /** next() updates this if ctrl_info changed. */
  inline bool changedInfo();

 private:

  std::string channel_name;

private:

    // Index
    // Index &index;
    // Channel found in index
    // AutoPtr<Index::Result> index_result;  
    // Current data block info for that channel  
    // AutoPtr<RTree::Datablock> datablock;

  DatablockMONGO datablock;
        
    // Meta-info for current sample    
    DbrType dbr_type;
    DbrCount dbr_count;
    CtrlInfo ctrl_info;
    bool type_changed;
    bool ctrl_info_changed;    
    double period;    

    // Current sample
    RawValueAutoPtr data;
    size_t raw_value_size;

    AutoPtr<class DataHeader> header;
    size_t val_idx; // current index in data buffer

 private:

    bool checkChannelName(const std::string& channel_name);

    void getHeader(const stdString &basename, FileOffset offset);
    const RawValue::Data *findSample(const epicsTime &start);
    const RawValue::Data *nextFromDatablock();

 public:

    /// Read a value from binary file.
    /// size: pre-calculated from type, count.
    /// @exception GenericException on error.
    void read(DbrType type, DbrCount count,
	      size_t size, RawValue::Data *value,
	      class DataFile *datafile, FileOffset offset);

};

// new commands

inline void RawDataReaderMONGO::setArchiveName(const std::string& archive_name){
  datablock.setArchiveName(archive_name);
}

// DataReader API

inline const std::string& RawDataReaderMONGO::getName() const {
    return channel_name;
}   

inline const RawValue::Data* RawDataReaderMONGO::get() const {   
  return data; 
}

inline DbrType RawDataReaderMONGO::getType() const {   
  return dbr_type; 
}
    
inline DbrCount RawDataReaderMONGO::getCount() const {   
  return dbr_count; 
}
    
inline const CtrlInfo& RawDataReaderMONGO::getInfo() const {  
  return ctrl_info; 
}

inline bool RawDataReaderMONGO::changedType(){
    bool changed = type_changed;
    type_changed = false;
    return changed;
}

inline bool RawDataReaderMONGO::changedInfo() {
    bool changed = ctrl_info_changed;
    ctrl_info_changed = false;
    return changed;
} 

}}

#endif
