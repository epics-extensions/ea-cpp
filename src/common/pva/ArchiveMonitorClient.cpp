#include "pva/ArchiveMonitorClient.h"
#include "pva/ArchiveRequestException.h"

#include "pva/StreamValuesCommand.h"

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;
using namespace epics::pvAccess;

namespace ea4 { namespace pva {

ArchiveMonitorClientFactory* ArchiveMonitorClientFactory::theInstance = 0;

ArchiveMonitorClientFactory* ArchiveMonitorClientFactory::getInstance() {
  if(theInstance == 0) {
    theInstance = new ArchiveMonitorClientFactory();
  }
  return theInstance;
}

ArchiveMonitorClientFactory::ArchiveMonitorClientFactory(){
  commands["streamValues"]  = new StreamValuesCommand();

}

ArchiveMonitorClientPtr
ArchiveMonitorClientFactory::createClient(const std::string& channelName){
return shared_ptr<ArchiveMonitorClient>(new ArchiveMonitorClient(channelName));
}

ArchiveCommand* const
ArchiveMonitorClientFactory::getCommand(const std::string& commandName) {
  std::map<std::string, ArchiveCommand*>::const_iterator it;
  it = commands.find(commandName);
  if(it == commands.end()) return 0;
  return it->second;
}

//

ArchiveMonitorClient::ArchiveMonitorClient(const std::string & channelName)
  : m_channelName(channelName), m_connected(false) {
}

void ArchiveMonitorClient::init() {
 
  ChannelProvider::shared_pointer provider = 
    getChannelProviderRegistry()->getProvider("pva");
    
  shared_ptr<ArchiveChannelRequester> 
    requester(new ArchiveChannelRequester()); 
        
  m_channelRequester = requester;
  m_channel = provider->createChannel(m_channelName, requester);
}

bool ArchiveMonitorClient::connect(double timeOut) {
  init();
  m_connected = m_channelRequester->waitUntilConnected(timeOut);            
  return m_connected;      
}

MonitorPtr 
ArchiveMonitorClient::createMonitor(MonitorRequesterPtr monRequester, 
				    PVStructurePtr pvRequest, 
				    double timeOut) {

  MonitorPtr monitor;

  bool allOK = true;

  if (m_connected || connect(timeOut)) {

    // shared_ptr<MonitorRequester> requester(new ArchiveMonitorRequester());        
    monitor = m_channel->createMonitor(monRequester, pvRequest);

  } else {

    allOK = false;
    m_channel->destroy();
    m_connected = false;
    string errMsg = "[" + m_channel->getChannelName() + "] connection timeout";
    throw ArchiveRequestException(Status::STATUSTYPE_ERROR, errMsg);
  }

  if (!allOK) {
    throw ArchiveRequestException(Status::STATUSTYPE_ERROR, 
				  "Monitor request failed");
  }

  return monitor;
}


  }}
