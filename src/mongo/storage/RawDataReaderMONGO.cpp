// RawDataReaderMONGO.cpp

#include <iostream>

// storage
#include "storage/DataFile.h"
#include "storage/RawDataReaderMONGO.h"

// tools
#include "tools/MsgLogger.h"
#include "tools/Conversions.h"
#include "tools/Filename.h"

// #define DEBUG_DATAREADER

using namespace std;
using namespace mongo;

namespace ea4 { namespace storage {

RawDataReaderMONGO::RawDataReaderMONGO()
        : dbr_type(0),
          dbr_count(0),
          type_changed(false),
          ctrl_info_changed(false),
          period(0.0),
          raw_value_size(0),
          val_idx(0) {
}

RawDataReaderMONGO::RawDataReaderMONGO(const string& name)
  : dbr_type(0),
    dbr_count(0),
    type_changed(false),
    ctrl_info_changed(false),
    period(0.0),
    raw_value_size(0),
    val_idx(0) {
  datablock.setArchiveName(name);
}

RawDataReaderMONGO::~RawDataReaderMONGO() {
    // Delete the header which might still keep a data file open.
    header = 0;
    DataFile::clear_cache();
}

const RawValue::Data* 
RawDataReaderMONGO::find(const string& channel_name,
			 const epicsTime* start) {

#   ifdef DEBUG_DATAREADER
    printf("RawDataReaderMONGO(%s)::find(%s)\n",
           datablock.getArchiveName().c_str(), channel_name.c_str());
#   endif
    
    if(!datablock.checkPvName(channel_name)) return 0;
    this->channel_name = channel_name;

    bool res = datablock.search(channel_name, start);
    if(!res) return 0; // No values for this time in index

#ifdef DEBUG_DATAREADER
      {
	std::string range = datablock.getInterval().toString();
        printf("RawDataReaderMONGO::find - First Block: %s @ 0x%lX: %s\n",
	       datablock.getDataFilename().c_str(),
	       (unsigned long) datablock.getDataOffset(),
	       range.c_str());
        }
#endif

    try {
     
      if (datablock.getDataOffset() == 0){
	throw GenericException(__FILE__, __LINE__, "Cannot handle shallow indices");
      }

      // Get the buffer for that data block
      getHeader(datablock.getDataFilename(), datablock.getDataOffset());

     const RawValue::Data* rawData;

        if (start){
            return findSample(*start);
	} else {
            rawData =  findSample(datablock.getInterval().getStart());
	    std::cout << "OK3" << std::endl;
	    return rawData;

	}

    } catch (GenericException &e) {  // Add channel name to the message
        throw GenericException(__FILE__, __LINE__,
                               "MongoDB, Archive: '%s', Channel: '%s':\n%s",
                               datablock.getArchiveName().c_str(),
                               channel_name.c_str(), 
			       e.what());
    }
}

// Read next sample, the one to which val_idx points.
const RawValue::Data* RawDataReaderMONGO::next() {

  if (!header){
    throw GenericException(__FILE__, __LINE__,
                               "Data Reader called after reaching end of data");
  }
    
  // Still in current header?
  if (val_idx < header->data.num_samples)
    return nextFromDatablock();

  // Exhaused the current header.
  if (datablock.isValid()) {  
 
    // Try to get next data block
    LOG_ASSERT(datablock.isValid());

    if (datablock.getNextDatablock()) {

#ifdef DEBUG_DATAREADER
      printf("- Next Block: %s @ 0x%lX: %s, samples: %d\n",
	     datablock.getDataFilename().c_str(),
	     (unsigned long) datablock.getDataOffset(),
	     datablock.getInterval().toString().c_str(),
	     header->data.num_samples);
                   
#endif

      getHeader(datablock.getDataFilename(), datablock.getDataOffset());

      if (!findSample(datablock.getInterval().getStart())) {
	return 0;
      }

      if (RawValue::getTime(data) >= datablock.getInterval().getStart()){
	return data;
      }

#ifdef DEBUG_DATAREADER
            printf("- findSample gave sample<start, skipping that one\n");
#endif

     return next();  
    } 
    // indicate that there's no datablock
    // datablock = 0;
  }
    
  // TODO: For better ListIndex functionality,
  //       ask index again with proper start time,
  //       which might switch to to another sub-archive
  // data= find(...., last known end time);
  // maybe skip one sample < proper time.

    // else: RTree indicates end of data.
    // In the special case of a master index between updates,
    // the last data file might in fact have more samples
    // than the RTree thinks there are, so try to read on.

#ifdef DEBUG_DATAREADER
    printf("- RawDataReader(%s) reached end for '%s' in '%s': ",
           datablock.getArchiveName().c_str(),
           header->datafile->getBasename().c_str(),
           header->datafile->getDirname().c_str());
    printf("Sample %zd of %lu\n",
           val_idx, (unsigned long)header->data.num_samples);
#endif

    // Refresh datafile and header.
    header->datafile->reopen();
    header->read(header->offset);

    // Need to look for next header (w/o asking RTree) ?
    if (val_idx >= header->data.num_samples) {

        if (!Filename::isValid(header->data.next_file))
            return 0; // Reached last sample in last buffer.

        // Found a new buffer, unknown to RTree
        getHeader(header->data.next_file, 
		  header->data.next_offset);

#       ifdef DEBUG_DATAREADER
        printf("- Using new data block %s @ 0x%X:\n",
                   header->datafile->getFilename().c_str(),
                   (unsigned int)header->offset);
#       endif

        return findSample(header->data.begin_time);
    }

    // After refresh, the last buffer had more samples.
#   ifdef DEBUG_DATAREADER 
    printf("Using sample %zd of %lu\n",
           val_idx, (unsigned long)header->data.num_samples);
#   endif

    return nextFromDatablock();
} 

//

// Sets header to new dirname/basename/offset,
// or throws GenericException with details.
void RawDataReaderMONGO::getHeader(const std::string& basename, FileOffset offset)
{

  if (!Filename::isValid(basename)){
    throw GenericException(__FILE__, __LINE__, "'%s': Invalid basename",
			   channel_name.c_str());
  }

  try {

    AutoPtr<DataHeader>  new_header;

    {   
      // Read new header
      DataFile *datafile;
            
      if (basename[0] == '/') {
	// Index gave us the data file with the full path
	datafile = DataFile::reference("", basename, false);
      } else {	      
	// Look relative to the index's directory
	// datafile = DataFile::reference(prvData->index_result->getDirectory(),
	//                                basename, false);
	throw GenericException(__FILE__, __LINE__,
	 "file does not include the full path, archive: '%s', file: '%s' @ 0x%08lX for channel '%s'.\n",
			       datablock.getArchiveName().c_str(),
			       basename.c_str(),
			       (unsigned long) offset, 
			       channel_name.c_str());
      }

            // If we keep opening data files, we'll hit the max-open-files limit.
            // Close files which are no longer referenced:
            DataFile::clear_cache();
    
            try  {
                new_header = datafile->getHeader(offset);
            } catch (GenericException &e) {   
	      // Something is wrong with datafile, can't read header.
	      // Release the file, then send the error upwards.
	      datafile->release();
	      throw e;
            }
            // DataFile now ref'ed by new_header.
            datafile->release();
        }

        // Need to read CtrlInfo because we don't have any or it changed?
        if (!header                                                              ||
            new_header->data.ctrl_info_offset   != header->data.ctrl_info_offset ||
            new_header->datafile->getFilename() != header->datafile->getFilename())
        {
            CtrlInfo new_ctrl_info;

            // new_ctrl_info.read(new_header->datafile,
            //                   new_header->data.ctrl_info_offset);
	    new_header->datafile->read(new_ctrl_info,
				       new_header->data.ctrl_info_offset);

            // When we switch files, we read a new CtrlInfo,
            // but it might contain the same values, so compare:
            if (header == 0  ||  new_ctrl_info != ctrl_info)
            {
                ctrl_info = new_ctrl_info;
                ctrl_info_changed = true;
            }
        }

        // Switch to new header. AutoPtr will release previous header.
        header = new_header;

        // If we never allocated a RawValue, or the type changed...
        if (!data ||
            header->data.dbr_type  != dbr_type  ||
            header->data.dbr_count != dbr_count) {

            dbr_type  = header->data.dbr_type;
            dbr_count = header->data.dbr_count;

            raw_value_size = RawValue::getSize(dbr_type, dbr_count);

            data = RawValue::allocate(dbr_type, dbr_count, 1);
            type_changed = true;
        }
    }
    catch (GenericException &e) {
        throw GenericException(__FILE__, __LINE__,
           "Error in data header '%s', '%s' @ 0x%08lX for channel '%s'.\n%s",
	   datablock.getArchiveName().c_str(),
           basename.c_str(),
	   (unsigned long) offset, 
	   channel_name.c_str(),
           e.what());
    }
}

// Based on a valid 'header' & allocated 'data',
// return sample before-or-at start,
// leaving val_idx set to the following sample
// (i.e. we return sample[val_idx-1],
//  its stamp is <= start,
//  and sample[val_idx] should be > start)
const RawValue::Data* RawDataReaderMONGO::findSample(const epicsTime &start)
{
    LOG_ASSERT(header);
    LOG_ASSERT(data); 

#ifdef DEBUG_DATAREADER
    printf("RawDataReaderMONGO::findSample(const epicsTime &start)\n");
    std::string stamp_txt;
    epicsTime2string(start, stamp_txt);
    printf("- Goal: %s\n", stamp_txt.c_str());
#endif

    // Speedier handling of start == header->data.begin_time
    if (start == header->data.begin_time) {
#ifdef DEBUG_DATAREADER
        printf("- Using the first sample in the buffer\n");
#endif
        val_idx = 0;
        return next();
    }

    // Binary search for sample before-or-at start in current header
    epicsTime stamp;
    size_t low = 0, high = header->data.num_samples - 1;

    FileOffset offset, offset0 =
        header->offset + sizeof(DataHeader::DataHeaderData);

    while (true){ 
  
      // Pick middle value, rounded up
        val_idx = (low+high+1)/2;
        offset = offset0 + val_idx * raw_value_size;

        read(dbr_type, dbr_count, 
	     raw_value_size, 
	     data,
	     header->datafile, offset);
        stamp = RawValue::getTime(data);

#ifdef DEBUG_DATAREADER
        epicsTime2string(stamp, stamp_txt);
        printf("- Index %zd: %s\n", val_idx, stamp_txt.c_str());
#endif

        if (high-low <= 1){   
	  // The intervall can't shrink further.
	  // idx == high because of up-rounding above.
	  // Which value's best?
	  LOG_ASSERT(val_idx == high);
            
	  if (stamp > start) {
	    val_idx = low;
	    return next();
	  }
            
	  // else: val_idx == high is good & already read into data
	  break;
        }

        if (stamp == start)
            break; // val_idx is perfect & already read into data
        else if (stamp > start)
            high = val_idx;
        else
            low = val_idx;
    }   

    ++val_idx;
    return data;
}

// Read next sample, the one to which val_idx points.
const RawValue::Data *RawDataReaderMONGO::nextFromDatablock()
{    

    // Read 'val_idx' sample in current block.
    FileOffset offset = header->offset
        + sizeof(DataHeader::DataHeaderData) + val_idx * raw_value_size;

    read(dbr_type, dbr_count, 
	 raw_value_size, 
	 data,
	 header->datafile, offset);

    // If we still have an RTree entry: Are we within bounds?
    // This is because the DataFile might contain the current sample
    // in the current buffer, but the RTree already has a different
    // DataFile in mind for this time stamp.
    if (datablock.isValid()  && RawValue::getTime(data) > 
	datablock.getInterval().getEnd()) {   
      // Recurse after marking end of samples in the current datafile
      val_idx = header->data.num_samples;
      return next();
    }
    ++val_idx;
    return data;
}


void  RawDataReaderMONGO::read(DbrType type, DbrCount count, size_t size, 
			       RawValue::Data *value,
			       DataFile *datafile, FileOffset offset) {

  int fseek_flag = fseek(datafile->file, offset, SEEK_SET);
  FileOffset ftell_offset = (FileOffset) ftell(datafile->file);
  int fread_status = fread(value, size, 1, datafile->file);

    /*
    if (fseek(datafile->file, offset, SEEK_SET) != 0 ||
        (FileOffset) ftell(datafile->file) != offset   ||
        fread(value, size, 1, datafile->file) != 1){
    */
    if (fseek_flag != 0 ||
        ftell_offset != offset   ||
        fread_status != 1){
 
        throw GenericException(__FILE__, __LINE__,
          "Data read error in '%s' @ 0x%08lX, fseek: %d, ftell: 0x%08lX, fread: %d",
          datafile->getFilename().c_str(),
	  (FileOffset)offset,
          fseek_flag,
          (FileOffset) ftell_offset,
	  fread_status);
    }
    
    SHORTFromDisk(value->status);
    SHORTFromDisk(value->severity);
    // Show error message which confuses XML-RPC,
    // or throw an exception?
    // Or just use the patched time stamp and run on so that
    // we get some data?
    safeEpicsTimeStampFromDisk(value->stamp);

    // nasty: cannot use inheritance in lightweight RawValue,
    // so we have to switch on the type here:
    switch (type)
    {
    case DBR_TIME_CHAR:
    case DBR_TIME_STRING:
        break;

        // The following might generate "unused variable 'data'"
        // warnings on systems where no byte swapping is required.
#define FROM_DISK(DBR, TYP, TIMETYP, MACRO)                                  \
    case DBR:                                                                \
        {                                                                    \
            TYP *data __attribute__ ((unused)) = & ((TIMETYP *)value)->value;\
            for (size_t i=0; i<count; ++i)  MACRO(data[i]);                  \
        }                                                                    \
        break;
        FROM_DISK(DBR_TIME_DOUBLE,dbr_double_t,dbr_time_double, DoubleFromDisk)
        FROM_DISK(DBR_TIME_FLOAT, dbr_float_t, dbr_time_float,  FloatFromDisk)
        FROM_DISK(DBR_TIME_SHORT, dbr_short_t, dbr_time_short,  SHORTFromDisk)
        FROM_DISK(DBR_TIME_ENUM,  dbr_enum_t,  dbr_time_enum,   USHORTFromDisk)
        FROM_DISK(DBR_TIME_LONG,  dbr_long_t,  dbr_time_long,   LONGFromDisk)
    default:
        throw GenericException(__FILE__, __LINE__,
                               "Data with unknown DBR_xx %d in '%s' @ 0x%08lX",
                               type, datafile->getFilename().c_str(),
                               (unsigned long)offset);
#undef FROM_DISK
    }
}

  }}


