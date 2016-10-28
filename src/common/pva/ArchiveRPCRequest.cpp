#include "pva/ArchiveRPCRequest.h"
#include "pva/ArchiveRPCServiceRegistry.h"
#include "pva/ArchiveRPCCommand.h"
#include "pv/rpcService.h"


using namespace std;
using namespace std::tr1;
using namespace epics::pvAccess;
using namespace epics::pvData;

// #define DEBUG_COMMAND

namespace ea4 { namespace pva {

ArchiveRPCRequest::ArchiveRPCRequest(
   ChannelRPCRequester::shared_pointer const & channelRPCRequester,
   Channel::shared_pointer channel) :
  m_channelRPCRequester(channelRPCRequester),
  m_channel(channel) {
}

ArchiveRPCRequest::~ArchiveRPCRequest(){
  destroy();
}

void ArchiveRPCRequest::request(PVStructurePtr const & pvArg) {

#ifdef DEBUG_COMMAND
  std::cout << "ArchiveRPCRequest::request: " << std::endl;
#endif

  pvArgument  = pvArg;
  run();
}

void ArchiveRPCRequest::run(){

#ifdef DEBUG_COMMAND
  std::cout << "ArchiveRPCRequest::run" << std::endl;
#endif

  PVStructure::shared_pointer result;
  Status status = Status::Ok;
        
  bool ok = true;

  ArchiveRPCServiceRegistry* rpcRegistry = 
    ArchiveRPCServiceRegistry::getInstance();
  ArchiveRPCServicePtr rpcService = rpcRegistry->getRPCService();

  string commandName = rpcService->getCommandName(pvArgument);
  ArchiveCommand* command = rpcService->getRPCCommand(commandName);

  shared_ptr<ArchiveRPCCommand> rpcCommand = 
    dynamic_pointer_cast<ArchiveRPCCommand>(command->createCommand());
      
  try {
    if(rpcCommand.get() == 0) {
      ok = false;
    } else {
      result = rpcCommand->process(pvArgument);
    }
  } catch (RPCRequestException& rre) {
    status = Status(rre.getStatus(), rre.what());
    ok = false;
  } catch (std::exception& ex) {
    status = Status(Status::STATUSTYPE_FATAL, ex.what());
    ok = false;
  } catch (...) {
    // handle user unexpected errors
    status = Status(Status::STATUSTYPE_FATAL, 
    "Unexpected exception caught while calling RPCService.request(PVStructure).");
    ok = false;
  }
    
  // check null result
  if (ok && result.get() == 0) {
    status = Status(Status::STATUSTYPE_FATAL, 
		    "RPCService.request(PVStructure) returned null.");

    PVStructurePtr pvStatus = createPVStatus();
    PVIntPtr typeField = pvStatus->getIntField("type");
    typeField->put(2); // error

    PVStringPtr messageField = pvStatus->getStringField("message");
    messageField->put("RPCService.request returned null.");

    result = createEmptyResult(pvStatus);
  }
        
  m_channelRPCRequester->requestDone(status, shared_from_this(), result);
        
  if (m_lastRequest.get()) destroy();

}

PVStructurePtr 
ArchiveRPCRequest::createEmptyResult(PVStructurePtr& pvStatus){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(1);
  PVFieldPtrArray fields(1);

  names[0]  = "status";
  fields[0] = pvStatus;

  PVStructurePtr result = dataFactory->createPVStructure(names, fields);
  return result;
}

PVStructurePtr ArchiveRPCRequest::createPVStatus(){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(3);
  PVFieldPtrArray fields(3);

  names[0] = "type";
  fields[0] = dataFactory->createPVScalar(pvInt);

  names[1] = "message";
  fields[1] = dataFactory->createPVScalar(pvString);

  names[2] = "callTree";
  fields[2] = dataFactory->createPVScalar(pvString);

  PVStructurePtr result = dataFactory->createPVStructure(names, fields);
  return result;
}

}}
