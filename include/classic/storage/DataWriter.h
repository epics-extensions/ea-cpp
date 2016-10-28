// -*- c++ -*-

// tools
#include "tools/ToolsConfig.h"
#include "tools/AutoPtr.h"
#include "tools/NoCopy.h"

// storage
#include "storage/CtrlInfo.h"
#include "storage/RawValue.h"
#include "storage/Index.h"

/// \addtogroup Storage
/// @{

/// Writes data to storage.
///
/// The data writer interfaces between a series of
/// RawValue values and the Index/DataFile.
///
class DataWriter
{
public:
    /// Create a writer for the given index.
    ///
    /// @param index:        index file
    /// @param channel_name: name of the channel
    /// @param ctrl_info:    meta information for the channel
    /// @param dbr_type:     the dbr_time_xxx type
    /// @param dbr_count:    array size
    /// @param period:       estimated periodicity of the channel
    /// @param num_samples:  estimated number of samples
    ///                      (helps w/ buffer allocation)
    ///
    /// @exception GenericException on error.
    DataWriter(Index &index,
               const stdString &channel_name,
               const CtrlInfo &ctrl_info,
               DbrType dbr_type,
               DbrCount dbr_count,
               double period,
               size_t num_samples);

    /// Destructor.
    ///
    /// <b>Note:</b> Since one might use another DataWriter
    ///              to add to the same set of data files,
    ///              the DataWriter does not clear the DataFile cache.
    ///              Call DataFile::close_all() when done!
    ~DataWriter();

    /// Returns the last time stamp in the archive.
    ///
    /// This allows you to avoid the back-in-time error
    /// by checking before adding.
    /// The result is a null time stamp in case
    /// there's nothing in the archive, yet.
    epicsTime getLastStamp();

    /// Add a value.
    ///
    /// @return Returns true if the sample was added,
    ///         false if the sample goes back-in-time
    ///         and is thus ignored.
    /// @exception GenericException on error.
    bool add(const RawValue::Data *data);

    /// Data file size limit.
    static FileOffset file_size_limit;

    /// Base name of data files.
    /// If not set, the date and time is used.
    static stdString data_file_name_base;

    /// Write a value to binary file.
    ///
    /// Requires a buffer for the memory-to-disk format conversions.
    ///
    /// @exception GenericException on error.
    void write(DbrType type, DbrCount count,
	       size_t size, const RawValue::Data *value,
	       MemoryBuffer<dbr_time_string> &cvt_buffer,
	       class DataFile *datafile, FileOffset offset);

public:

  /// write 1
  dbr_time_string* write1(size_t size,
				 MemoryBuffer<dbr_time_string> &cvt_buffer,
				 class DataFile *datafile); 
  /// write 2
  void write2(dbr_time_string* buffer, size_t size,
	      const RawValue::Data *value);

  /// write 3
  void write3(dbr_time_string* buffer);

  /// write 4
  void write4(DbrType type, DbrCount count, 
	      dbr_time_string* buffer,
	      size_t size, const RawValue::Data *value,
	      class DataFile *datafile, FileOffset offset);

  /// write 5a
  void write5a(class DataFile *datafile, FileOffset offset);

  /// write 5b
  void write5b(class DataFile *datafile, FileOffset offset);

  /// write 6
  void write6(dbr_time_string* buffer, size_t size,
		     class DataFile *datafile, FileOffset offset);

    
private:

    PROHIBIT_DEFAULT_COPY(DataWriter);
    Index &index;
    AutoPtr<Index::Result> index_result;
    const stdString channel_name;
    const CtrlInfo &ctrl_info;
    DbrType dbr_type;
    DbrCount dbr_count;
    double period;
    size_t raw_value_size;

    void makeDataFileName(int serial, stdString &name);
    DataFile *createNewDataFile(size_t headroom);

    // Sets next_buffer_size to at least 'start',
    // so that buffers get bigger and bigger up to
    // some limit.
    void calc_next_buffer_size(size_t start);
    size_t next_buffer_size;

    AutoPtr<class DataHeader> header;
    size_t available;
    MemoryBuffer<dbr_time_string> cvt_buffer;

    void addNewHeader(bool new_ctrl_info);
};

/// @}
