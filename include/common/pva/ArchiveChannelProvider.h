#ifndef EA4_ARCHIVE_CHANNEL_PROVIDER_H
#define EA4_ARCHIVE_CHANNEL_PROVIDER_H

#include "pva/Def.h"
// #include "pva/ArchiveRPCService.h"
// #include "pva/ArchiveMonitorService.h"

namespace ea4 { namespace pva {

/** Channel provider of the EA4 service */

class ArchiveChannelProvider : 
 public virtual epics::pvAccess::ChannelProvider,
   public virtual epics::pvAccess::ChannelFind,
   public std::tr1::enable_shared_from_this<ArchiveChannelProvider> {

 public:

    POINTER_DEFINITIONS(ArchiveChannelProvider);
       
    // TODO thread pool support
    
    ArchiveChannelProvider();

 public:

    // ChannelProvider API

    /** returns ea4.pva.ChannelProvider */
    virtual std::string getProviderName();
    
    /** returns RPC or monitor channels */
    virtual ChannelFindPtr 
      channelFind(std::string const & name,
		  ChannelFindRequesterPtr const & requester);

   /**
     * Find channels.
     * @param channelFindRequester The epics::pvData::Requester.
     * @return An interface for the find.
     */
   virtual ChannelFindPtr 
     channelList(ChannelListRequesterPtr const & requester);

    /** creates RPC or monitor channels */
    virtual ChannelPtr 
      createChannel(std::string const & channelName,
		    ChannelRequesterPtr const & requester,
		    short /*priority*/);

    /** not supported */
    virtual ChannelPtr 
      createChannel(std::string const & /*channelName*/,
		    ChannelRequesterPtr const & /*requester*/,
		    short /*priority*/,
		    std::string const & /*address*/);

    /** empty */
    inline void destroy();

 public:

    // ChannelFind API

    /** Returns shared_from_this() */
    virtual ChannelProviderPtr getChannelProvider();
   
    /** empty */
    inline void cancel();

 public:

    // void registerRPCService(ArchiveRPCServicePtr const & service);
  
    // void unregisterRPCService();

    // void registerMonitorService(ArchiveMonitorServicePtr const & service);
  
    // void unregisterMonitorService();

private:

    static std::string PROVIDER_NAME;
    static epics::pvData::Status noSuchChannelStatus;

private:

    // typedef std::map<std::string, RPCServicePtr> RPCServiceMap;
    // RPCServiceMap m_services;

    // ArchiveRPCServicePtr rpcService; 
    // ArchiveMonitorServicePtr monService;   

    epics::pvData::Mutex m_mutex;
};

inline void ArchiveChannelProvider::destroy() {
}

inline void ArchiveChannelProvider::cancel() {
}

}}


#endif
