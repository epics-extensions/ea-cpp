#ifndef EA4_ARCHIVE_CHANNEL_REQUESTER_H
#define EA4_ARCHIVE_CHANNEL_REQUESTER_H

#include "pv/pvData.h"
#include "pv/rpcService.h"
#include "pv/clientFactory.h"
#include "pv/clientContextImpl.h"
#include "pv/rpcClient.h"
#include "pva/Def.h"

namespace ea4 { namespace pva {

/** Listener for connect state changes */

class ArchiveChannelRequester : public epics::pvAccess::ChannelRequester {

public:

  /** returns EA4ChannelRequester */
  virtual std::string getRequesterName();

  /** prints an error  message augmented with the requester name */
  virtual void message(std::string const & message, 
		       epics::pvData::MessageType messageType);

  /** prints an error message about the channel creation status */
  virtual void channelCreated(const epics::pvData::Status& status, 
			      ChannelPtr const & channel);

  /** m_event.signal() if the connection state == connected */
  virtual void channelStateChange(ChannelPtr const & channel, 
				  epics::pvAccess::Channel::ConnectionState cs);

 public:

  /** m_event.wait(timeOut) */
  bool waitUntilConnected(double timeOut);

 private:

  epics::pvData::Event m_event;

};

typedef std::tr1::shared_ptr<ArchiveChannelRequester> ArchiveChannelRequesterPtr;

  }}

#endif
