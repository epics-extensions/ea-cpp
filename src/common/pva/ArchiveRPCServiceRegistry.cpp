#include "pva/ArchiveRPCServiceRegistry.h"

using namespace std;

namespace ea4 { namespace pva {

ArchiveRPCServiceRegistry* ArchiveRPCServiceRegistry::theInstance = 0;

ArchiveRPCServiceRegistry* ArchiveRPCServiceRegistry::getInstance(){
  if(theInstance == 0) {
    theInstance = new ArchiveRPCServiceRegistry();
  }
  return theInstance;
}

}}
