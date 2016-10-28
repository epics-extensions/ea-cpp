#include "pva/ArchiveMonitorServiceRegistry.h"

using namespace std;

namespace ea4 { namespace pva {

ArchiveMonitorServiceRegistry* ArchiveMonitorServiceRegistry::theInstance = 0;

ArchiveMonitorServiceRegistry* ArchiveMonitorServiceRegistry::getInstance(){
  if(theInstance == 0) {
    theInstance = new ArchiveMonitorServiceRegistry();
  }
  return theInstance;
}

}}
