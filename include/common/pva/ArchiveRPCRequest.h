#ifndef EA4_ARCHIVE_RPC_REQUEST_H
#define EA4_ARCHIVE_RPC_REQUEST_H

#include "pva/Def.h"
#include "pva/ArchiveRPCService.h"

namespace ea4 { namespace pva {

/** RPC processor of the EA4 service */
class ArchiveRPCRequest : public epics::pvAccess::ChannelRPC,
  public std::tr1::enable_shared_from_this<ArchiveRPCRequest>
{
 public:

  /** constructor */
  ArchiveRPCRequest(ChannelRPCRequesterPtr const & requester,
		    epics::pvAccess::Channel::shared_pointer channel); 

 public:

  /** empty */
  virtual ~ArchiveRPCRequest();

 public:

  // ChannelRequest API

  /**
    * Get a channel instance this request belongs to.
    * @return the channel instance.
    */
  inline std::tr1::shared_ptr<epics::pvAccess::Channel> 
    getChannel();

  /**
   * Cancel any pending request.
   * Completion will be reported via request's response callback:
   * <ul>
   *   <li>if cancel() request is issued after the request was already 
   *       complete, request success/failure completion will be reported 
   *       annd cancel() request ignored.</li>
   *   <li>if the request was actually canceled, cancellation completion 
   *       is reported.</li>
   * </ul>
   */
  virtual void cancel();

  /**
   * Announce next request as last request.
   * When last request will be completed (regardless of completion status) 
   * the remote and local instance will be destroyed.
   */
  virtual void lastRequest();

 public:

  // ChannelRPC API

  /** processes request */
  virtual void request(PVStructurePtr const & pvArgument);

  /** empty */
  inline void destroy();

  /** empty */
  inline void lock();

  /** empty */
  inline void unlock();

 public:

  // epicsThreadRunable API
  
  void run();

 protected:

  PVStructurePtr createEmptyResult(PVStructurePtr& pvStatus);
  PVStructurePtr createPVStatus();

 private:

  ChannelRPCRequesterPtr m_channelRPCRequester;

  PVStructurePtr pvArgument;
  epics::pvAccess::AtomicBoolean m_lastRequest;

  epics::pvAccess::Channel::shared_pointer m_channel;

};

// ChannelRequest API

inline epics::pvAccess::Channel::shared_pointer 
ArchiveRPCRequest::getChannel() {
  return m_channel;
}

inline void ArchiveRPCRequest::cancel(){
}

inline void ArchiveRPCRequest::lastRequest() {
  m_lastRequest.set();
}

// ChannelRPC API

inline void ArchiveRPCRequest::destroy() {
        // noop
}

inline void ArchiveRPCRequest::lock() {
        // noop
}

inline void ArchiveRPCRequest::unlock() {
        // noop
}


}}

#endif
