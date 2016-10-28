#ifndef __EA4_STORAGE_DATA_FILE_MANAGER_EA3_H
#define __EA4_STORAGE_DATA_FILE_MANAGER_EA3_H

#include "storage/DataFileRegistry.h"

namespace ea4 { namespace storage {

/**  Manager of the original data files */
class DataFileManagerEA3 : public DataFileManager {

 public:

  /** Closes all data files that are fully released.
   *
   * @return Returns the number of files which are
   * left open because there is still
   * a reference to them.
   */
   virtual unsigned int clear_cache();

};

}}

#endif
