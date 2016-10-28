#include "pva/Session.h"
#include "pv/rpcClient.h"
#include "pva/Collection.h"
#include "pva/OpenSessionCommand.h"

using namespace std;
using namespace epics::pvAccess;

namespace ea4 { namespace pva {

Session::Session() {
}

int Session::open(const string& chName, const string& id, double timeout){

  this->chName = chName;
  client = RPCClient::create(chName);

  OpenSessionCommand command;
  PVStructurePtr request = command.createRequest();

  OpenSessionCommand::setSessionName(id, request);

  PVStructurePtr response = client->request(request, timeout);

  int status;
  OpenSessionCommand::getStatus(response, status);

  return status;
  
}


PVStructurePtr Session::request(PVStructurePtr message, double timeout){
  PVStructurePtr response = client->request(message, timeout); 
  return response;
}

void Session::close(){
  this->chName.clear();
}

CollectionPtr Session::createProxy(const std::string& ns){

  CollectionPtr proxy (new Collection(this, ns));
  return proxy;
}

RPCClientPtr Session::getClient(){
   return client;
}

}}
