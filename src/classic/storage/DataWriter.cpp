// DataWriter.cpp

#include <iostream>

// tools
#include "tools/MsgLogger.h"
#include "tools/Conversions.h"
#include "tools/Filename.h"
#include "tools/epicsTimeHelper.h"

// storage
#include "storage/RTree.h"
#include "storage/DataFile.h"
#include "storage/DataWriter.h"

// #define DEBUG_DATA_WRITER

FileOffset DataWriter::file_size_limit = 100*1024*1024; // 100MB Default.

stdString DataWriter::data_file_name_base;

DataWriter::DataWriter(Index &index,
                       const stdString &channel_name,
                       const CtrlInfo &ctrl_info,
                       DbrType dbr_type,
                       DbrCount dbr_count,
                       double period,
                       size_t num_samples)
  : index(index),
    channel_name(channel_name),
    ctrl_info(ctrl_info),
    dbr_type(dbr_type),
    dbr_count(dbr_count),
    period(period),
    raw_value_size(RawValue::getSize(dbr_type, dbr_count)),
    next_buffer_size(0),
    available(0)
{
    DataFile *datafile = 0;

    try
    {
        // Size of next buffer should at least hold num_samples
        calc_next_buffer_size(num_samples);

        // Find or add appropriate data buffer
        index_result = index.addChannel(channel_name);
        LOG_ASSERT(index_result);
        
        
        AutoPtr<RTree::Datablock> block(
            index_result->getRTree()->getLastDatablock()); 
        if (block)       
        {   // - There is a data file and buffer
            datafile = DataFile::reference(index_result->getDirectory(),
                                           block->getDataFilename(), true);
            header = datafile->getHeader(block->getDataOffset());
            datafile->release(); // now ref'ed by header
            datafile = 0;
            // See if anything has changed
            CtrlInfo prev_ctrl_info;

	    header->datafile->read(prev_ctrl_info, 
				   header->data.ctrl_info_offset);
            // prev_ctrl_info.read(header->datafile,
            //                    header->data.ctrl_info_offset);

            if (prev_ctrl_info != ctrl_info)
                // Add new header and new ctrl_info
                addNewHeader(true);
            else if (header->data.dbr_type != dbr_type  ||
                     header->data.dbr_count != dbr_count)
                // Add new header, use existing ctrl_info
                addNewHeader(false);
            else
            {   // All fine, just check if we're already in bigger league
                size_t capacity = header->capacity();
                if (capacity > num_samples)
                    calc_next_buffer_size(capacity);
            }
        }
        else {
	    // New data file, add the initial header
            addNewHeader(true);
	}
        available = header->available();
    }
    catch (GenericException &e)
    {
        index_result = 0;
        if (datafile)
            datafile->release();
        throw GenericException(__FILE__, __LINE__,
                               "Channel '%s':\n%s",
                               channel_name.c_str(),
                               e.what());
    }
}
    
DataWriter::~DataWriter()
{

    try
    {   // Update index
        if (header)
        {   

            header->write();
            if (index_result)
            {
                if (!index_result->getRTree()->updateLastDatablock(
                    Interval(header->data.begin_time, header->data.end_time),
                    header->offset, header->datafile->getBasename()))
                {
                    // LOG_MSG("~DataWriter: updateLastDatablock '%s' %s @ 0x%lX was a NOP\n",
                    //         channel_name.c_str(),
                    //         header->datafile->getBasename().c_str(),
                    //         (unsigned long)header->offset);
                }
                index_result = 0;
            }

            header = 0;
        }
    }
    catch (GenericException &e)
    {
        LOG_MSG("Exception in %s (%zu):\n%s\n",
                __FILE__, (size_t) __LINE__, e.what());
    }
    catch (...)
    {
        LOG_MSG("Unknown Exception in %s (%zu)\n\n",
                __FILE__, (size_t) __LINE__);
    }
}

epicsTime DataWriter::getLastStamp()
{
    if (header)
        return epicsTime(header->data.end_time);
    return nullTime;
}

bool DataWriter::add(const RawValue::Data *data)
{
    LOG_ASSERT(header);
    epicsTime data_stamp = RawValue::getTime(data);
    if (data_stamp < header->data.end_time)
        return false;
    if (available <= 0) // though it might be full
    {
        addNewHeader(false);
        available = header->available();
    }
    // Add the value
    available -= 1;
    FileOffset offset = header->offset
        + sizeof(DataHeader::DataHeaderData)
        + header->data.curr_offset;

    write(dbr_type, dbr_count,
                    raw_value_size, data,
                    cvt_buffer,
                    header->datafile, offset);

    // Update the header
    header->data.curr_offset += raw_value_size;
    header->data.num_samples += 1;
    header->data.buf_free    -= raw_value_size;
    if (header->data.num_samples == 1) // first entry?
        header->data.begin_time = data_stamp;
    header->data.end_time = data_stamp;
    // Note: we didn't write the header nor update the index,
    // that'll happen when we close the DataWriter!
    return true;
}


void DataWriter::makeDataFileName(int serial, stdString &name)
{
    int  len;
    char buffer[30];    

    if (data_file_name_base.length() > 0)
    {
	name = data_file_name_base;
        if (serial > 0)
        {
            len = snprintf(buffer, sizeof(buffer), "-%d", serial);
            if (len >= (int)sizeof(buffer))
                len = sizeof(buffer)-1;
            name.append(buffer, len);
        }
        return;
    }
    // Else: Create name based on  "<today>[-serial]"
    int year, month, day, hour, min, sec;
    unsigned long nano;
    epicsTime now = epicsTime::getCurrent();    
    epicsTime2vals(now, year, month, day, hour, min, sec, nano);
    if (serial > 0)
        len = snprintf(buffer, sizeof(buffer),
                       "%04d%02d%02d-%d", year, month, day, serial);
    else
        len = snprintf(buffer,sizeof(buffer),
                       "%04d%02d%02d", year, month, day);
    if (len >= (int)sizeof(buffer))
        len = sizeof(buffer)-1;
    name.assign(buffer, len);
}

// Create new DataFile that's below file_size_limit in size.
DataFile *DataWriter::createNewDataFile(size_t headroom)
{

    LOG_ASSERT(index_result);
    DataFile *datafile = 0;
    int serial=0;
    stdString data_file_name;
    try
    {   // Keep opening existing data files until we find
        // one below the file limit.
        // We might have to create a new one.
        while (true)
        {
            makeDataFileName(serial, data_file_name);
            datafile = DataFile::reference(index_result->getDirectory(),
                                           data_file_name, true);
            FileOffset file_size = datafile->getSize();

            if (file_size+headroom < file_size_limit)
                return datafile;
            if (datafile->is_new_file)
            {
                LOG_MSG ("Warning: %s: "
                         "Cannot create a new data file within file size limit\n"
                         "type %d, count %d, %zu samples, file limit: %d bytes.\n",
                         channel_name.c_str(),
                         dbr_type, dbr_count, next_buffer_size, file_size_limit);
                return datafile; // Use anyway
            }
            // Try the next one.
            ++serial;
            datafile->release();
            datafile = 0;
        }
    }
    catch (GenericException &e)
    {
        if (datafile)
        {
            datafile->release();
            datafile = 0;
        }
        throw GenericException(__FILE__, __LINE__,
                               "Reference new datafile '%s':\n%s",
                               data_file_name.c_str(), e.what());
    }
    throw GenericException(__FILE__, __LINE__,
                           "createNewDataFile(%zu) failed", headroom);
    return 0;
}

void DataWriter::calc_next_buffer_size(size_t start)
{
    if (start < 64)
        next_buffer_size = 64;
    else if (start >= 4096)
        next_buffer_size = 4096;
    else
    {   // We want the next power of 2:
        int log2 = 0;
        size_t req = start;
        while (req > 0)
        {
            req >>= 1;
            ++log2;
        }
        next_buffer_size = 1 << log2;
    }

#ifdef DEBUG_DATA_WRITER
    LOG_MSG("calc_next_buffer_size: %10zu  -> %10zu\n",
            start, next_buffer_size);
#endif
}

// Add a new header because
// - there's none
// - data type or ctrl_info changed
// - the existing data buffer is full.
// Might switch to new DataFile.
// Will write ctrl_info out if new_ctrl_info is set,
// otherwise the new header tries to point to the existing ctrl_info.
void DataWriter::addNewHeader(bool new_ctrl_info)
{
    FileOffset ctrl_info_offset;
    AutoPtr<DataHeader> new_header;
    {
        bool       new_datafile = false;    // Need to use new DataFile?
        DataFile  *datafile = 0;
        size_t     headroom = 0;
        try
        {
            if (!header)
                new_datafile = true;            // Yes, because there's none.
            else
            {   // Check how big the current data file would get
                FileOffset file_size = header->datafile->getSize();
                headroom = header->datafile->getHeaderSize(channel_name,
                                                           dbr_type, dbr_count,
                                                           next_buffer_size);
                if (new_ctrl_info)
                    headroom += ctrl_info.getSize();
                if (file_size+headroom > file_size_limit) // Yes: reached max. size.
                    new_datafile = true;
            }

            if (new_datafile) {
#               ifdef DEBUG_DATA_WRITER
                LOG_MSG("DataWriter::addNewHeader: New Datafile\n");
#               endif
                datafile = createNewDataFile(headroom);
                new_ctrl_info = true;
            } else {
#               ifdef DEBUG_DATA_WRITER
                LOG_MSG("DataWriter::addNewHeader: adding to '%s'\n",
                        header->datafile->getFilename().c_str());
#               endif
                datafile = header->datafile->reference();
            }

            if (new_ctrl_info){
                datafile->addCtrlInfo(ctrl_info, ctrl_info_offset);
	    } else {
	      // use existing one
                ctrl_info_offset = header->data.ctrl_info_offset;
	    }

            new_header = datafile->addHeader(channel_name, 
					     dbr_type, dbr_count,
                                             period, next_buffer_size);

            datafile->release(); // now ref'ed by new_header

        } catch (GenericException &e) {

	  if (datafile) {
	    datafile->release();
	    datafile = 0;
	  }
           
	  throw GenericException(__FILE__, __LINE__,
                               "Channel '%s':\n%s",
                               channel_name.c_str(), e.what());
        }

    }
    LOG_ASSERT(new_header);
    new_header->data.ctrl_info_offset = ctrl_info_offset;
    if (header)
    {   // Link old header to new one.
        header->set_next(new_header->datafile->getBasename(),
                         new_header->offset);
 
        header->write();
        // back from new to old.
        new_header->set_prev(header->datafile->getBasename(),
                             header->offset);        
        // Update index entry for the old header.
        if (index_result) // Ignore result since block might already be in index
            index_result->getRTree()->updateLastDatablock(
                Interval(header->data.begin_time, header->data.end_time),
                header->offset, header->datafile->getBasename());
    }
    // Switch to new_header (AutoPtr, might del. current header).
    header = new_header;
    // Calc buffer size for the following header (not the current one).
    calc_next_buffer_size(next_buffer_size);
    // new header will be added to index when it's closed.
}

dbr_time_string* 
DataWriter::write1(size_t size,
		   MemoryBuffer<dbr_time_string> &cvt_buffer,
		   DataFile *datafile){
    LOG_ASSERT(datafile->is_writable());
    cvt_buffer.reserve(size);
    dbr_time_string *buffer = cvt_buffer.mem();
    return buffer;
}

void DataWriter::write2(dbr_time_string* buffer, 
			size_t size,
			const RawValue::Data *value){
    memcpy(buffer, value, size);
    SHORTToDisk(buffer->status);
    SHORTToDisk(buffer->severity);
}

void DataWriter::write3(dbr_time_string* buffer){

    if (buffer->stamp.nsec >= 1000000000L)
        throw  GenericException(__FILE__, __LINE__,
        "invalid time stamp with %zu secs, %zu nsecs.",
        (size_t)buffer->stamp.secPastEpoch, (size_t)buffer->stamp.nsec);
    epicsTimeStampToDisk(buffer->stamp);
}

void DataWriter::write4(DbrType type, DbrCount count, 
			dbr_time_string* buffer,
			size_t size, const RawValue::Data *value,
			DataFile *datafile, FileOffset offset){
    switch (type)
    {
#define TO_DISK(DBR, TYP, TIMETYP, CVT_MACRO)                                 \
    case DBR:                                                                 \
        {                                                                     \
            TYP *data __attribute__ ((unused)) = & ((TIMETYP *)buffer)->value;\
            for (size_t i=0; i<count; ++i)  CVT_MACRO (data[i]);              \
        }                                                                     \
        break;

    case DBR_TIME_CHAR:
    case DBR_TIME_STRING:
        // no conversion necessary
        break;
        TO_DISK(DBR_TIME_DOUBLE, dbr_double_t, dbr_time_double, DoubleToDisk)
        TO_DISK(DBR_TIME_FLOAT,  dbr_float_t,  dbr_time_float,  FloatToDisk)
        TO_DISK(DBR_TIME_SHORT,  dbr_short_t,  dbr_time_short,  SHORTToDisk)
        TO_DISK(DBR_TIME_ENUM,   dbr_enum_t,   dbr_time_enum,   USHORTToDisk)
        TO_DISK(DBR_TIME_LONG,   dbr_long_t,   dbr_time_long,   LONGToDisk)
    default:
        throw GenericException(__FILE__, __LINE__,
                               "Data with unknown DBR_xx %d in '%s' @ 0x%08lX",
                               type, datafile->getFilename().c_str(),
                               (unsigned long)offset);
#undef TO_DISK
    }
}

void DataWriter::write5a(DataFile *datafile, FileOffset offset){

  if (fseek(datafile->file, offset, SEEK_SET) != 0)
        throw GenericException(__FILE__, __LINE__,
                               "Data seek error in '%s' @ 0x%08lX",
                               datafile->getFilename().c_str(),
                               (unsigned long)offset);

}

void DataWriter::write5b(DataFile *datafile, FileOffset offset){

    if ((FileOffset)ftell(datafile->file) != offset)
        throw GenericException(__FILE__, __LINE__,
                               "Data seek error in '%s' @ 0x%08lX",
                               datafile->getFilename().c_str(),
                               (unsigned long)offset);

}

void DataWriter::write6(dbr_time_string* buffer, size_t size,
			DataFile *datafile, FileOffset offset){

    size_t written = fwrite(buffer, 1, size, datafile->file);
    if (written != size)
        throw GenericException(__FILE__, __LINE__,
                               "Data write error in '%s' @ 0x%08lX, %zu != %zu",
                               datafile->getFilename().c_str(),
                               (unsigned long)offset, written, size);

}

void DataWriter::write(DbrType type, DbrCount count, size_t size,
		       const RawValue::Data *value,
		       MemoryBuffer<dbr_time_string> &cvt_buffer,
		       DataFile *datafile, FileOffset offset)
{

  dbr_time_string * buffer = write1(size, cvt_buffer, datafile);

  write2(buffer, size, value);

  write3(buffer);

  write4(type, count, buffer, size, value, datafile, offset);

  write5a(datafile, offset);
  write5b(datafile, offset);

  write6(buffer, size, datafile, offset);
}


