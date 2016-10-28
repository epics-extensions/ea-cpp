#include "pva/ArchiveRPCChannel.h"
#include "pva/ArchiveRPCRequest.h"

using namespace std;
using namespace std::tr1;
using namespace epics::pvAccess;
using namespace epics::pvData;

// #define DEBUG_COMMAND

namespace ea4 { namespace pva {

Status ArchiveRPCChannel::notSupportedStatus(Status::STATUSTYPE_ERROR, 
	   "only channelRPC requests are supported by this channel");
Status ArchiveRPCChannel::destroyedStatus(Status::STATUSTYPE_ERROR, 
					  "channel destroyed");

ArchiveRPCChannel::ArchiveRPCChannel(
           ChannelProvider::shared_pointer const & provider,
           string const & channelName,
           ChannelRequester::shared_pointer const & channelRequester) :
           m_provider(provider),
           m_channelName(channelName),
           m_channelRequester(channelRequester) {
}

ArchiveRPCChannel::~ArchiveRPCChannel(){
  destroy();
}

Channel::ConnectionState ArchiveRPCChannel::getConnectionState(){
  return isConnected() ? Channel::CONNECTED : Channel::DESTROYED;
}

ChannelProcess::shared_pointer 
ArchiveRPCChannel::createChannelProcess(
            ChannelProcessRequester::shared_pointer const & requester,
            PVStructure::shared_pointer const & /*pvRequest*/) {
  ChannelProcess::shared_pointer nullPtr;
  requester->channelProcessConnect(notSupportedStatus, nullPtr);
  return nullPtr; 
}

ChannelGet::shared_pointer 
ArchiveRPCChannel::createChannelGet(
            ChannelGetRequester::shared_pointer const & requester,
            PVStructure::shared_pointer const & /*pvRequest*/){
  ChannelGet::shared_pointer nullPtr;
  // requester->channelGetConnect(notSupportedStatus, 
  //			       nullPtr,
  //			       PVStructure::shared_pointer());
  return nullPtr; 
}

ChannelPut::shared_pointer 
ArchiveRPCChannel::createChannelPut(
            ChannelPutRequester::shared_pointer const & requester,
            PVStructure::shared_pointer const & /*pvRequest*/){
  ChannelPut::shared_pointer nullPtr;
  // requester->channelPutConnect(notSupportedStatus, 
  //			       nullPtr,
  //			       PVStructure::shared_pointer());
  return nullPtr; 
}


ChannelPutGet::shared_pointer 
ArchiveRPCChannel::createChannelPutGet(
            ChannelPutGetRequester::shared_pointer const & requester,
            PVStructure::shared_pointer const & /*pvRequest*/){
  ChannelPutGet::shared_pointer nullPtr;
  PVStructure::shared_pointer nullStructure;
  // requester->channelPutGetConnect(notSupportedStatus, 
  //				  nullPtr, 
  //				  nullStructure, 
  //				  nullStructure);
  return nullPtr; 
}

ChannelRPC::shared_pointer 
ArchiveRPCChannel::createChannelRPC(
            ChannelRPCRequester::shared_pointer const & requester,
            PVStructure::shared_pointer const & /*pvRequest*/){
  // nothing expected to be in pvRequest

#ifdef DEBUG_COMMAND
  std::cout << "ArchiveRPCChannel::createChannelRPC" << std::endl;
#endif
        
  if (requester.get() == 0)
    throw std::invalid_argument("channelRPCRequester == null");

  if (m_destroyed.get()){
    ChannelRPC::shared_pointer nullPtr;
    requester->channelRPCConnect(destroyedStatus, nullPtr);
    return nullPtr;
  }
        
  ChannelRPC::shared_pointer 
    archiveRPC(new ArchiveRPCRequest(requester, shared_from_this()));
  requester->channelRPCConnect(Status::Ok, archiveRPC);

  return archiveRPC;
}

Monitor::shared_pointer 
ArchiveRPCChannel::createMonitor(
            MonitorRequester::shared_pointer const & requester,
            PVStructure::shared_pointer const & /*pvRequest*/){
  Monitor::shared_pointer nullPtr;
  requester->monitorConnect(notSupportedStatus, 
			    nullPtr, 
			    Structure::shared_pointer());
  return nullPtr; 
}

ChannelArray::shared_pointer 
ArchiveRPCChannel::createChannelArray(
            ChannelArrayRequester::shared_pointer const & requester,
            PVStructure::shared_pointer const & /*pvRequest*/){
  ChannelArray::shared_pointer nullPtr;
  // requester->channelArrayConnect(notSupportedStatus, 
  //				 nullPtr, 
  //				 PVArray::shared_pointer());
  return nullPtr; 
}

void ArchiveRPCChannel::printInfo(){
  std::cout << "RPCChannel: " << getChannelName() << " [" 
	    << Channel::ConnectionStateNames[getConnectionState()] << "]" 
	    << std::endl;
}

void ArchiveRPCChannel::printInfo(ostream& out){
  out << "RPCChannel: ";
  out << getChannelName();
  out << " [";
  out << Channel::ConnectionStateNames[getConnectionState()];
  out << "]";
}

}}
