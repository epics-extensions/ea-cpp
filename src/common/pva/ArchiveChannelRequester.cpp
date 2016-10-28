#include "pva/ArchiveChannelRequester.h"

using namespace std;
using namespace epics::pvData;
using namespace epics::pvAccess;

namespace ea4 { namespace pva {

string ArchiveChannelRequester::getRequesterName() {
        return "ArchiveChannelRequester";
}

void ArchiveChannelRequester::message(string const & message, 
				      MessageType messageType) {
  cerr << "[" << getRequesterName() 
       << "] message(" << message 
       << ", " << getMessageTypeName(messageType) 
       << ")" << endl;
}

void ArchiveChannelRequester::channelCreated(const Status& status, 
					     ChannelPtr const & channel) {
  if (status.isSuccess()) {
    // show warning
    if (!status.isOK()) {
      cerr << "[" << channel->getChannelName() << "] channel create: " 
	   << status << endl;
    }
   } else {
    cerr << "[" << channel->getChannelName() 
	 << "] failed to create a channel: " 
	 << status << endl;
  }
}

void ArchiveChannelRequester::channelStateChange(ChannelPtr const & c, 
		      Channel::ConnectionState connectionState)  {
  if (connectionState == Channel::CONNECTED) {
    m_event.signal();
  }
 }

bool ArchiveChannelRequester::waitUntilConnected(double timeOut) {
  return m_event.wait(timeOut);
}


  }}
