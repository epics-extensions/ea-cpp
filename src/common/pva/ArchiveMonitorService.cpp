#include "pva/ArchiveMonitorService.h"
#include "pva/ArchiveRequestException.h"
#include "pva/ArchiveMonitorCommand.h"

using namespace std;
using namespace epics::pvData;

namespace ea4 { namespace pva {

ArchiveMonitorService::~ArchiveMonitorService(){

  map<std::string, ArchiveMonitorCommand*>::iterator mit;
  for(mit = monCommands.begin(); mit != monCommands.end(); mit++){
    delete mit->second;
  }
  monCommands.clear();

}

ArchiveMonitorCommand* const
ArchiveMonitorService::getMonitorCommand(const string& commandName) const{
  map<string, ArchiveMonitorCommand*>::const_iterator it;
  it = monCommands.find(commandName);
  if(it == monCommands.end()) return 0;
  return it->second;
}

void ArchiveMonitorService::setServiceName(const std::string& name){
  serviceName = name;
  channelName = "ea4.monitor:" + name;
}

  }}
