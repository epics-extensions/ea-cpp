#include "pva/ArchiveRPCClient.h"

#include "pv/pvData.h"
#include "pv/rpcService.h"
#include "pv/clientFactory.h"
#include "pv/clientContextImpl.h"
#include "pv/rpcClient.h"

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;
using namespace epics::pvAccess;

namespace ea4 { namespace pva {

ArchiveRPCClientFactory* ArchiveRPCClientFactory::theInstance = 0;

ArchiveRPCClientFactory* ArchiveRPCClientFactory::getInstance() {
  if(theInstance == 0) {
    theInstance = new ArchiveRPCClientFactory();
  }
  return theInstance;
}

ArchiveRPCClientFactory::ArchiveRPCClientFactory(){
}

RPCClient::shared_pointer 
ArchiveRPCClientFactory::createClient(const std::string& channelName){
  return RPCClient::create(channelName);
}

ArchiveCommand* const
ArchiveRPCClientFactory::getCommand(const std::string& commandName) {
  std::map<std::string, ArchiveCommand*>::const_iterator it;
  it = commands.find(commandName);
  if(it == commands.end()) return 0;
  return it->second;
}

}}
