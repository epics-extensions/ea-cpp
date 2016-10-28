#include "pva/ArchiveRPCService.h"
#include "pva/ArchiveRequestException.h"
#include "pva/ArchiveRPCCommand.h"

using namespace std;
using namespace epics::pvData;

namespace ea4 { namespace pva {

ArchiveRPCService::~ArchiveRPCService(){

  map<std::string, ArchiveRPCCommand*>::iterator rit;
  for(rit = rpcCommands.begin(); rit != rpcCommands.end(); rit++){
    delete rit->second;
  }
  rpcCommands.clear();

}

ArchiveRPCCommand* const
ArchiveRPCService::getRPCCommand(const string& commandName) const{
  map<string, ArchiveRPCCommand*>::const_iterator it;
  it = rpcCommands.find(commandName);
  if(it == rpcCommands.end()) return 0;
  return it->second;
}

void ArchiveRPCService::setServiceName(const std::string& name){
  serviceName = name;
  channelName = "ea4.rpc:" + name;
}

}}
