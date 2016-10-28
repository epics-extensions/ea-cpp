#include <time.h>
#include <sys/time.h>

// tools
#include "tools/MsgLogger.h"
#include "pva/OpenSessionCommand.h"
#include "pva/ArchiveRPCServiceMONGO.h"
#include "pva/ActCommandMONGO.h"

using namespace std;
using namespace epics::pvData;
using namespace epics::pvAccess;

namespace ea4 { namespace pva {

ArchiveRPCServiceMONGO::ArchiveRPCServiceMONGO(){
  serviceType = "mongo";
  rpcCommands["openSession"]  = new OpenSessionCommand();
  rpcCommands["act"]          = new ActCommandMONGO();
}

}}


