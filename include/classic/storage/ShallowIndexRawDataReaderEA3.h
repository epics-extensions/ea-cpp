// -*- c++ -*-

#ifndef _EA4_SHALLOW_INDEX_RAW_DATA_READER_EA3_H__
#define _EA4_SHALLOW_INDEX_RAW_DATA_READER_EA3_H__

#include <tr1/memory>

// Tools
#include "tools/ToolsConfig.h"
#include "tools/AutoPtr.h"

// Storage
#include "storage/DataReader.h"

typedef std::tr1::shared_ptr<Index> IndexPtr;

namespace ea4 { namespace storage {

/// \addtogroup Storage
/// @{

/** RawDataReader that understands 'shallow' indices.
 *  <p>
 *  As long as a 'shallow' index is hit, this reader keeps drilling down
 *  to the first 'full' index, and then uses a RawDataReader to get the
 *  samples.
 */
class ShallowIndexRawDataReaderEA3 : public DataReader
{
public:

  /** constructor */
  ShallowIndexRawDataReaderEA3(IndexPtr index);

public:

  // DataReader API

   /** Destructor */
  virtual ~ShallowIndexRawDataReaderEA3();

  /** Locate data */
  virtual const RawValue::Data *find(const std::string& channel_name,
				     const epicsTime* start);

  /** Name of the channel, i.e. the one passed to find() */
  virtual const stdString &getName() const;

  /** Obtain the next value. */  
  virtual const RawValue::Data *next();

  /** Current value. */
  virtual const RawValue::Data *get() const;


    virtual DbrType getType() const;
    virtual DbrCount getCount() const;
    virtual const CtrlInfo &getInfo() const;
    virtual bool changedType();
    virtual bool changedInfo();

private:

    /** Top-most index; where the search starts */
    IndexPtr top_index;
    
    /** Channel found in top_index */
    AutoPtr<Index::Result> top_result;
      
    /** Current data block from top_result */
    AutoPtr<RTree::Datablock> top_datablock;
    
    /** The sub index.
     *  Used iteratively while following the chain of sub indices.
     *  In the end, this is the bottom-most index, the one
     *  that points to the actual data.
     */
    IndexPtr sub_index;

    /** Channel found in sub_index. */
    AutoPtr<Index::Result> sub_result;
    
    /** Data block from DataFile received via sub_result. */
    AutoPtr<RTree::Datablock> sub_datablock;
    
    /** The reader for the actual 'full' index that we might have
     *  found under the 'shallow' index.
     */
    AutoPtr<DataReader> reader;

    /** Find channel_name and start data,
     *  if necessary recursively going down,
     *  beginning with index/result/datablock.
     */
    const RawValue::Data* find(IndexPtr index,
                               AutoPtr<Index::Result> &index_result,
                               AutoPtr<RTree::Datablock> &datablock,
                               const stdString &channel_name,
                               const epicsTime *start);
};

}}

/// @}

#endif
