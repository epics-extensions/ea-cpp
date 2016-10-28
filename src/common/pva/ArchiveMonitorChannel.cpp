#include "pva/ArchiveMonitorChannel.h"
#include "pva/ArchiveMonitorRequest.h"

using namespace std;
using namespace std::tr1;
using namespace epics::pvAccess;
using namespace epics::pvData;

namespace ea4 { namespace pva {

Status ArchiveMonitorChannel::notSupportedStatus(Status::STATUSTYPE_ERROR, 
       "only Monitor requests are supported by this channel");
Status ArchiveMonitorChannel::destroyedStatus(Status::STATUSTYPE_ERROR, 
					      "channel destroyed");

ArchiveMonitorChannel::ArchiveMonitorChannel(
           ChannelProvider::shared_pointer const & provider,
           string const & channelName,
           ChannelRequester::shared_pointer const & channelRequester) :
           m_provider(provider),
           m_channelName(channelName),
           m_channelRequester(channelRequester){
}

ArchiveMonitorChannel::~ArchiveMonitorChannel(){
  destroy();
}

Channel::ConnectionState ArchiveMonitorChannel::getConnectionState(){
  return isConnected() ? Channel::CONNECTED : Channel::DESTROYED;
}

ChannelProcess::shared_pointer 
ArchiveMonitorChannel::createChannelProcess(
            ChannelProcessRequester::shared_pointer const & requester,
            PVStructure::shared_pointer const & /*pvRequest*/) {
  ChannelProcess::shared_pointer nullPtr;
  requester->channelProcessConnect(notSupportedStatus, nullPtr);
  return nullPtr; 
}

ChannelGet::shared_pointer 
ArchiveMonitorChannel::createChannelGet(
            ChannelGetRequester::shared_pointer const & requester,
            PVStructure::shared_pointer const & /*pvRequest*/){
  ChannelGet::shared_pointer nullPtr;
  // requester->channelGetConnect(notSupportedStatus, 
  //			       nullPtr,
  //			       PVStructure::shared_pointer());
  return nullPtr; 
}

ChannelPut::shared_pointer 
ArchiveMonitorChannel::createChannelPut(
            ChannelPutRequester::shared_pointer const & requester,
            PVStructure::shared_pointer const & /*pvRequest*/){
  ChannelPut::shared_pointer nullPtr;
  //  requester->channelPutConnect(notSupportedStatus, 
  //			       nullPtr,
  //			       PVStructure::shared_pointer(), 
  //			       BitSet::shared_pointer());
  return nullPtr; 
}


ChannelPutGet::shared_pointer 
ArchiveMonitorChannel::createChannelPutGet(
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
ArchiveMonitorChannel::createChannelRPC(
            ChannelRPCRequester::shared_pointer const & requester,
            PVStructure::shared_pointer const & /*pvRequest*/){
    ChannelRPC::shared_pointer nullPtr;
    requester->channelRPCConnect(notSupportedStatus, nullPtr);
    return nullPtr;
}

Monitor::shared_pointer 
ArchiveMonitorChannel::createMonitor(
            MonitorRequester::shared_pointer const & requester,
            PVStructure::shared_pointer const & pvRequest){

  // std::cout << "createMonitor" << std::endl;
  // return MockMonitor::create(m_name, requester, m_pvStructure, pvRequest);

     
  if (requester.get() == 0)
    throw std::invalid_argument("monitorRequester == null");

  if (m_destroyed.get()){
    StructureConstPtr nullStr;
    Monitor::shared_pointer nullPtr;
    requester->monitorConnect(destroyedStatus, nullPtr, nullStr);
    return nullPtr;
  }

  // string str;
  // pvRequest->toString(&str);
  // std::cout << str;
  // std::cout << std::endl;

  ArchiveMonitorRequestPtr monitor(new ArchiveMonitorRequest(m_channelName, 
				   requester, pvRequest));

  requester->monitorConnect(Status::Ok, monitor, monitor->getResultType());

  return monitor;
}

ChannelArray::shared_pointer 
ArchiveMonitorChannel::createChannelArray(
            ChannelArrayRequester::shared_pointer const & requester,
            PVStructure::shared_pointer const & /*pvRequest*/){
  ChannelArray::shared_pointer nullPtr;
  // requester->channelArrayConnect(notSupportedStatus, 
  //				 nullPtr, 
  //				 PVArray::shared_pointer());
  return nullPtr; 
}

void ArchiveMonitorChannel::printInfo(){
  std::cout << "Monitor: " << getChannelName() << " [" 
	    << Channel::ConnectionStateNames[getConnectionState()] << "]" 
	    << std::endl;
}

void ArchiveMonitorChannel::printInfo(ostream& out){
  out << "MonitorChannel: ";
  out << getChannelName();
  out << " [";
  out << Channel::ConnectionStateNames[getConnectionState()];
  out << "]";
}

}}
