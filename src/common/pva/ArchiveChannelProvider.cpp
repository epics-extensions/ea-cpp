#include "pva/ArchiveChannelProvider.h"
#include "pva/ArchiveRPCChannel.h"
#include "pva/ArchiveMonitorChannel.h"

#include "pva/ArchiveRPCServiceRegistry.h"
#include "pva/ArchiveMonitorServiceRegistry.h"

using namespace std;
using namespace epics::pvAccess;
using namespace epics::pvData;

// #define DEBUG_COMMAND

namespace ea4 { namespace pva {

string ArchiveChannelProvider::PROVIDER_NAME = "ea4.pva.ChannelProvider";
Status ArchiveChannelProvider::noSuchChannelStatus(Status::STATUSTYPE_ERROR, "no such channel");

ArchiveChannelProvider::ArchiveChannelProvider() {
}

string ArchiveChannelProvider::getProviderName() {
  return PROVIDER_NAME;
}

ChannelFind::shared_pointer 
ArchiveChannelProvider::channelFind(
   string const & channelName,
   ChannelFindRequester::shared_pointer const & requester){

  bool rpcFound = false;    
  bool monFound = false;

  ArchiveRPCServiceRegistry* rpcRegistry = 
    ArchiveRPCServiceRegistry::getInstance();
  ArchiveRPCServicePtr rpcService = rpcRegistry->getRPCService();

  ArchiveMonitorServiceRegistry* monRegistry = 
    ArchiveMonitorServiceRegistry::getInstance();
  ArchiveMonitorServicePtr monService = monRegistry->getMonitorService();

  {
     Lock guard(m_mutex);
     if(rpcService.get() != 0 && (channelName == rpcService->getChannelName())) rpcFound = true;
     if(monService.get() != 0 && (channelName == monService->getChannelName())) monFound = true;
  }

  bool found = ((rpcFound == true) || (monFound == true));
       
  ChannelFind::shared_pointer thisPtr(shared_from_this());
  requester->channelFindResult(Status::Ok, thisPtr, found);
  return thisPtr;

}

ChannelFind::shared_pointer 
ArchiveChannelProvider::channelList(
  ChannelListRequester::shared_pointer const & requester) {
        
  if (!requester.get())
    throw std::runtime_error("null requester");

  ArchiveRPCServiceRegistry* rpcRegistry = 
    ArchiveRPCServiceRegistry::getInstance();
  ArchiveRPCServicePtr rpcService = rpcRegistry->getRPCService();

  ArchiveMonitorServiceRegistry* monRegistry = 
    ArchiveMonitorServiceRegistry::getInstance();
  ArchiveMonitorServicePtr monService = monRegistry->getMonitorService();

  // NOTE: this adds only active channels, not all (especially RPC ones)
  PVStringArray::svector channelNames;

  {
    Lock guard(m_mutex);

    if(rpcService.get() != 0) {
	channelNames.reserve(1);
	channelNames.push_back(rpcService->getChannelName());
    }
    if(monService.get() != 0) {
      channelNames.reserve(1);
      channelNames.push_back(monService->getChannelName());
    }
  }
    
  ChannelFind::shared_pointer thisPtr(shared_from_this());
  requester->channelListResult(Status::Ok, thisPtr, 
			       freeze(channelNames), 
			       true);
  return thisPtr;
}

Channel::shared_pointer 
ArchiveChannelProvider::createChannel(
            string const & channelName,
            ChannelRequester::shared_pointer const & requester,
            short /*priority*/){

  bool rpcFound = false;    
  bool monFound = false;

  ArchiveRPCServiceRegistry* rpcRegistry = 
    ArchiveRPCServiceRegistry::getInstance();
  ArchiveRPCServicePtr rpcService = rpcRegistry->getRPCService();

  ArchiveMonitorServiceRegistry* monRegistry = 
    ArchiveMonitorServiceRegistry::getInstance();
  ArchiveMonitorServicePtr monService = monRegistry->getMonitorService();


  {
     Lock guard(m_mutex);
     if(rpcService.get() != 0 && (channelName == rpcService->getChannelName())) rpcFound = true;
     if(monService.get() != 0 && (channelName == monService->getChannelName())) monFound = true;
  }

#ifdef DEBUG_COMMAND
  std::cout << "ArchiveChannelProvider::createChannel: " << channelName 
	    << ", rpc:" << rpcFound << ", monitor: " << monFound <<  std::endl;
#endif

  if(rpcFound) {            
    Channel::shared_pointer rpcChannel(new ArchiveRPCChannel(
                shared_from_this(),
                channelName,
                requester));
    requester->channelCreated(Status::Ok, rpcChannel);
    return rpcChannel;
  }

  if(monFound) {            
    Channel::shared_pointer monChannel(new ArchiveMonitorChannel(
                shared_from_this(),
                channelName,
                requester));
    requester->channelCreated(Status::Ok, monChannel);
    return monChannel;
  }

  Channel::shared_pointer nullChannel;
  requester->channelCreated(noSuchChannelStatus, nullChannel);
  return nullChannel;

}

Channel::shared_pointer 
ArchiveChannelProvider::createChannel(
        string const & /*channelName*/,
        ChannelRequester::shared_pointer const & /*channelRequester*/,
        short /*priority*/,
        string const & /*address*/){
        // this will never get called by the pvAccess server
        throw std::runtime_error("not supported");
}

// ChannelFind API

std::tr1::shared_ptr<ChannelProvider> 
ArchiveChannelProvider::getChannelProvider() {
  return shared_from_this();
}

// 

/*

void ArchiveChannelProvider::registerRPCService(
	       ArchiveRPCServicePtr const & service){
        Lock guard(m_mutex);
        // m_services[serviceName] = service;
	rpcService = service;
}

void ArchiveChannelProvider::unregisterRPCService() {
        Lock guard(m_mutex);
        // m_services.erase(serviceName);
	rpcService.reset();
}

void ArchiveChannelProvider::registerMonitorService(
	ArchiveMonitorServicePtr const & service){
        Lock guard(m_mutex);
        // m_services[serviceName] = service;
	monService = service;
}

void ArchiveChannelProvider::unregisterMonitorService() {
        Lock guard(m_mutex);
        // m_services.erase(serviceName);
	monService.reset();
}

*/

  }}
