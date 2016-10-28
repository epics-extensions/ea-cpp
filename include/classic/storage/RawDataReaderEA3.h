// -*- c++ -*-

#ifndef _EA4_RAW_DATA_READER_EA3_H__
#define _EA4_RAW_DATA_READER_EA3_H__

#include <tr1/memory>

// Tools
#include "tools/ToolsConfig.h"
#include "tools/AutoPtr.h"

// Storage
#include "storage/DataReader.h"
#include "storage/RTree.h"

typedef std::tr1::shared_ptr<Index> IndexPtr;

namespace ea4 { namespace storage {

/// \addtogroup Storage
/// @{

/** An implementation of the DataReader for raw data.
 *  <p>
 *  It reads the original samples from a 'full' index.
 *  No averaging, no support for 'shallow' indices.
 */
class RawDataReaderEA3 : public DataReader
{

public:

  /** Constructor */
  RawDataReaderEA3(IndexPtr index);

public:

  // DataReader API
    
  /** Destructor */
  virtual ~RawDataReaderEA3();

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
  inline const CtrlInfo &getInfo() const;
    
  /** next() updates this if dbr_type/count changed. */ 
  inline bool changedType();
    
  /** next() updates this if ctrl_info changed. */
  inline bool changedInfo();

private:

  // Name of channel from last find() call
  std::string channel_name;

private:

    // Index
    IndexPtr index;

    // Channel found in index
    AutoPtr<Index::Result>    index_result;  

    // Current data block info for that channel  
    AutoPtr<RTree::Datablock> datablock;

private:
       
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

// DataReader API

inline const std::string& RawDataReaderEA3::getName() const {
    return channel_name;
}   

inline const RawValue::Data* RawDataReaderEA3::get() const {   
  return data; 
}

inline DbrType RawDataReaderEA3::getType() const {   
  return dbr_type; 
}
    
inline DbrCount RawDataReaderEA3::getCount() const {   
  return dbr_count; 
}
    
inline const CtrlInfo& RawDataReaderEA3::getInfo() const {  
  return ctrl_info; 
}

inline bool RawDataReaderEA3::changedType(){
    bool changed = type_changed;
    type_changed = false;
    return changed;
}

inline bool RawDataReaderEA3::changedInfo() {
    bool changed = ctrl_info_changed;
    ctrl_info_changed = false;
    return changed;
} 

}}


/// @}

#endif
