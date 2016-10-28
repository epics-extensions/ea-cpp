#include <time.h>
#include <sys/time.h>

// tools
#include "tools/MsgLogger.h"

#include "pva/ArchiveMonitorServiceEA3.h"


using namespace epics::pvData;
using namespace epics::pvAccess;

namespace ea4 { namespace pva {

ArchiveMonitorServiceEA3::ArchiveMonitorServiceEA3() {
  serviceType = "ea3";
}


  }}


