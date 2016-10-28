#include "storage/DataFileManagerEA3.h"
#include "storage/DataFile.h"

using namespace std;

namespace ea4 { namespace storage {

unsigned int  DataFileManagerEA3::clear_cache(){
  return DataFile::clear_cache();
}

}}
