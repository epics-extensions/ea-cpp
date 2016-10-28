#ifndef EA4_ARCHIVE_MONITOR_CLIENT_H
#define EA4_ARCHIVE_MONITOR_CLIENT_H

#include "pva/Def.h"
#include "pva/ArchiveMonitorCommand.h"
#include "pva/ArchiveChannelRequester.h"

namespace ea4 { namespace pva {

/** Monitor client of the EA4 serice */

class ArchiveMonitorClient
{

public:

  /** constructor */
  ArchiveMonitorClient(const std::string & channelName);

  /** creates monitor with a user-specific monitor requester */
  MonitorPtr createMonitor(MonitorRequesterPtr monRequester, 
			   PVStructurePtr pvRequest, 
			   double timeOut);

private:

  void init();

  bool connect(double timeOut);

  std::string m_channelName; 
  ArchiveChannelRequesterPtr m_channelRequester;
  ChannelPtr m_channel;
  bool m_connected;
};


typedef std::tr1::shared_ptr<ArchiveMonitorClient> ArchiveMonitorClientPtr;

/** Factory of ArchiveMonitorClient and repository of ArchiveMonitorCommand's */
class ArchiveMonitorClientFactory {
  
 public:

  /** returns a singleton */
  static ArchiveMonitorClientFactory* getInstance();

  /** creates a monitor client with the specified name 
      (such as \"a4.monitor:\<service name\>\" )*/
  ArchiveMonitorClientPtr createClient(const std::string& channelName);

 public:

  /** returns a selected command (such as \"streamValues\") */
  ArchiveCommand* const getCommand(const std::string& name);

 public:

  /** constructor */
  ArchiveMonitorClientFactory();

 private:

  static ArchiveMonitorClientFactory* theInstance;

  std::map<std::string, ArchiveCommand*> commands;

};



  }}

#endif
