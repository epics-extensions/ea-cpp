// RawDataReader.cpp

#include <iostream>

// tools
#include "tools/MsgLogger.h"
#include "tools/Conversions.h"
#include "tools/Filename.h"

// storage
#include "storage/RawDataReader.h"
#include "storage/RawDataReaderRegistry.h"

// #include "ea3/RawDataReaderEA3.h"
// #include "hdf5/RawDataReaderHDF5.h"

using namespace ea4::storage;

// #define DEBUG_DATAREADER

void RawDataReader::PrvData::init(){
  dbr_type  = 0;
  dbr_count = 0;
  type_changed = false;
  ctrl_info_changed = false;
  period = 0.0;
  data = 0;
}

RawDataReader::RawDataReader(Index &index)
        : index(index){

  LOG_ASSERT(index.getFilename().length() > 0);

  prvData.init();
 
  dataReader = 0;
}

RawDataReader::~RawDataReader() {
  if(dataReader) delete dataReader;
}

const RawValue::Data* RawDataReader::find(const stdString &channel_name,
                                          const epicsTime *start){

  prvData.init();

  if(!findDataBlock(channel_name, start)) return 0;

  if(dataReader) delete dataReader;

  const std::string& fname = prvData.datablock->getDataFilename();

  /*
  if(fname.substr(fname.find_last_of(".") + 1) == "h5") {
    dataReader = new ea4::RawDataReaderHDF5(index, &prvData);
  } else {
    dataReader = new RawDataReaderEA3(index, &prvData);
  }
  */

  RawDataReaderRegistry* rdrr = RawDataReaderRegistry::getInstance();
  RawDataReaderFactory* rdrf = rdrr->getFactory(fname);

  dataReader = rdrf->createReader(index, &prvData);
  return dataReader->find(channel_name, start);

}

// Read next sample, the one to which val_idx points.
const RawValue::Data *RawDataReader::next(){
  if(!dataReader) return 0;
  return dataReader->next();
}

const std::string& RawDataReader::getName() const {
    return prvData.channel_name;
}                                  

const RawValue::Data* RawDataReader::get() const {   
  return prvData.data; 
}

DbrType RawDataReader::getType() const {   
  return prvData.dbr_type; 
}
    
DbrCount RawDataReader::getCount() const {   
  return prvData.dbr_count; 
}
    
const CtrlInfo& RawDataReader::getInfo() const {   
  return prvData.ctrl_info; 
}

bool RawDataReader::changedType() {
    bool changed = prvData.type_changed;
    prvData.type_changed = false;
    return changed;
}

bool RawDataReader::changedInfo() {
    bool changed = prvData.ctrl_info_changed;
    prvData.ctrl_info_changed = false;
    return changed;
} 

bool RawDataReader::findDataBlock(const stdString &channel_name,
				  const epicsTime *start) {
#   ifdef DEBUG_DATAREADER
    printf("- RawDataReader()::findDataBlock(%s)\n",
            channel_name.c_str());
#   endif

    prvData.channel_name = channel_name;

    // Lookup channel in index
    prvData.index_result = index.findChannel(channel_name);

    if (! prvData.index_result) return false; // Channel not found

    try {
        // Get 1st data block
      if (start){
	prvData.datablock = prvData.index_result->getRTree()->search(*start);
      } else {
	prvData.datablock = prvData.index_result->getRTree()->getFirstDatablock();
      }

      if (! prvData.datablock) return false; // No values for this time in index
            
      LOG_ASSERT(prvData.datablock->isValid());

#     ifdef DEBUG_DATAREADER
        {
	  std::string range = prvData.datablock->getInterval().toString();
            printf("- First Block: %s @ 0x%lX: %s\n",
                   prvData.datablock->getDataFilename().c_str(),
                   (unsigned long) prvData.datablock->getDataOffset(),
                   range.c_str());
        }
#     endif

	if (prvData.datablock->getDataOffset() == 0)
            throw GenericException(__FILE__, __LINE__,
				   "Cannot handle shallow indices");

    } catch (GenericException &e) {  // Add channel name to the message
        throw GenericException(__FILE__, __LINE__,
                               "Index '%s', Channel '%s':\n%s",
                               index.getFilename().c_str(),
                               channel_name.c_str(), e.what());
    }

    return true;

}

    





