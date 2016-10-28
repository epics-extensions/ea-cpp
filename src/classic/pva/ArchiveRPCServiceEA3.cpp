#include <time.h>
#include <sys/time.h>

// tools
#include "tools/MsgLogger.h"
#include "pva/OpenSessionCommand.h"

#include "pva/ArchiveRPCServiceEA3.h"
#include "pva/ActCommandCLS.h"

using namespace epics::pvData;
using namespace epics::pvAccess;

namespace ea4 { namespace pva {

ArchiveRPCServiceEA3::ArchiveRPCServiceEA3() {
  serviceType = "ea3";
  rpcCommands["openSession"] = new OpenSessionCommand();
  rpcCommands["act"]         = new ActCommandCLS();
}

}}


