#ifndef EA4_ARCHIVE_RPC_CHANNEL_H
#define EA4_ARCHIVE_RPC_CHANNEL_H

#include "pva/Def.h"

namespace ea4 { namespace pva {

/** RPC channel of the EA4 service */
class ArchiveRPCChannel : public virtual epics::pvAccess::Channel,
      public std::tr1::enable_shared_from_this<ArchiveRPCChannel>
{

public:    

    POINTER_DEFINITIONS(ArchiveRPCChannel);
    
    /** Constructor */
    ArchiveRPCChannel(
           ChannelProviderPtr const & provider,
           std::string const & channelName,
           ChannelRequesterPtr const & channelRequester); 

 public:

    /** m_destroyed.set() */
    virtual ~ArchiveRPCChannel();

    /** returns a channel provider (defined via constructor) */
    inline std::tr1::shared_ptr<epics::pvAccess::ChannelProvider> 
      getProvider();
    
    /** returns a channel name (defined via constructor)*/
    inline std::string getRemoteAddress();

    /** returns Channel::CONNECTED or Channel::DESTROYED */
    virtual epics::pvAccess::Channel::ConnectionState 
      getConnectionState();

    /** returns a channel name (defined via constructor) */
    inline std::string getChannelName();
 
    /** returns a channel requester (defined via constructor) */
    inline std::tr1::shared_ptr<epics::pvAccess::ChannelRequester> 
      getChannelRequester();

    /** !m_destroyed.get() */
    inline bool isConnected();

    /** returns epics::pvAccess::none */
    inline epics::pvAccess::AccessRights 
      getAccessRights(PVFieldPtr const & /*pvField*/);
 
    /** requester->getDone(notSupportedStatus, ...) */
    inline void getField(GetFieldRequesterPtr const & requester,
			 std::string const & /*subField*/);

    /** returns nullPtr */
    virtual ChannelProcessPtr
      createChannelProcess(ChannelProcessRequesterPtr const & requester,
			   PVStructurePtr const & /*pvRequest*/);

    /** returns nullPtr */
    virtual ChannelGetPtr
      createChannelGet(ChannelGetRequesterPtr const & requester,
		       PVStructurePtr const & /*pvRequest*/);
    
    /** returns nullPtr */
    virtual ChannelPutPtr 
      createChannelPut(ChannelPutRequesterPtr const & requester,
		       PVStructurePtr const & /*pvRequest*/);
    
    /** returns nullPtr */  
    virtual ChannelPutGetPtr 
      createChannelPutGet(ChannelPutGetRequesterPtr const & requester,
			  PVStructurePtr const & /*pvRequest*/);

    /** returns ArchiveRPCRequest */
    virtual ChannelRPCPtr 
      createChannelRPC(ChannelRPCRequesterPtr const & requester,
		       PVStructurePtr const & /*pvRequest*/);
 
    /** returns nullPtr */
    virtual MonitorPtr 
      createMonitor(MonitorRequesterPtr const & requester,
		    PVStructurePtr const & /*pvRequest*/);

    /** returns nullPtr */
    virtual ChannelArrayPtr 
      createChannelArray(ChannelArrayRequesterPtr const & requester,
			 PVStructurePtr const & /*pvRequest*/);

    /** prints info with a channel name and connection state */
    virtual void printInfo();

    /** prints info to the string buffer  */
    virtual void printInfo(std::ostream& out);
 
    /** returns a channel name (defined via constructor) */
    inline std::string getRequesterName();
    
    /** delegates to a channel requester */
    inline void message(std::string const & m,
			epics::pvData::MessageType mType);

    /** m_destroyed.set() */
    inline void destroy();

private:

    static epics::pvData::Status notSupportedStatus;
    static epics::pvData::Status destroyedStatus;

 private:

    std::string m_channelName;
    
    epics::pvAccess::AtomicBoolean m_destroyed;
    
    ChannelProviderPtr m_provider;
    ChannelRequesterPtr m_channelRequester;

};


inline std::tr1::shared_ptr<epics::pvAccess::ChannelProvider> 
ArchiveRPCChannel::getProvider() {
  return m_provider;
}

inline std::string 
ArchiveRPCChannel::getRemoteAddress() {
  // local
  return getChannelName();
}

inline std::string 
ArchiveRPCChannel::getChannelName(){
  return m_channelName;
}

inline std::tr1::shared_ptr<epics::pvAccess::ChannelRequester> 
ArchiveRPCChannel::getChannelRequester() {
  return m_channelRequester;
}

inline bool ArchiveRPCChannel::isConnected(){
  return !m_destroyed.get();
}

inline epics::pvAccess::AccessRights 
ArchiveRPCChannel::getAccessRights(PVFieldPtr const & /*pvField*/){
  return epics::pvAccess::none;
}

inline void
ArchiveRPCChannel::getField(GetFieldRequesterPtr const & requester,
			    std::string const & /*subField*/){
  requester->getDone(notSupportedStatus, 
		     epics::pvData::Field::shared_pointer());    
}

inline std::string ArchiveRPCChannel::getRequesterName() {
  return getChannelName();
}

inline void 
ArchiveRPCChannel::message(std::string const & m,
			   epics::pvData::MessageType mType) {
  // just delegate
  m_channelRequester->message(m, mType);
}

inline void ArchiveRPCChannel::destroy() {
  m_destroyed.set();   
} 

  }}

#endif
