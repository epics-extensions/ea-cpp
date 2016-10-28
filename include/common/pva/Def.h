
#ifndef EA4_PVA_DEF_H
#define EA4_PVA_DEF_H

// epics 4
#include "pv/serverContext.h"
#include "pv/pvAccess.h"
#include "pv/remote.h"

#include "tools/timing.h"

namespace ea4 { 

/** PvAccess-based service interface of the EA4 archiver */
namespace pva {

// epicsTime

typedef std::tr1::shared_ptr<epicsTime> epicsTimePtr;

// pvData

typedef epics::pvData::Structure::shared_pointer StructurePtr;
typedef epics::pvData::StructureConstPtr StructureConstPtr;

typedef epics::pvData::BitSet::shared_pointer
  BitSetPtr;
typedef epics::pvData::PVField::shared_pointer
  PVFieldPtr;
typedef epics::pvData::PVStructure::shared_pointer 
  PVStructurePtr;

// server context

typedef epics::pvAccess::ServerContextImpl::shared_pointer
   ServerContextImplPtr;

// channel providers

typedef epics::pvAccess::ChannelProviderFactory::shared_pointer
  ChannelProviderFactoryPtr;  
typedef epics::pvAccess::ChannelProvider::shared_pointer
  ChannelProviderPtr;  
typedef epics::pvAccess::ChannelFind::shared_pointer 
  ChannelFindPtr;

// channels

typedef epics::pvAccess::Channel::shared_pointer
  ChannelPtr;

// channel requesters

typedef epics::pvAccess::ChannelFindRequester::shared_pointer
  ChannelFindRequesterPtr;
typedef epics::pvAccess::ChannelListRequester::shared_pointer
  ChannelListRequesterPtr;
typedef epics::pvAccess::ChannelRequester::shared_pointer
  ChannelRequesterPtr;
typedef epics::pvAccess::GetFieldRequester::shared_pointer
  GetFieldRequesterPtr;
typedef epics::pvAccess::ChannelProcessRequester::shared_pointer
  ChannelProcessRequesterPtr;
typedef epics::pvAccess::ChannelGetRequester::shared_pointer
  ChannelGetRequesterPtr;
typedef epics::pvAccess::ChannelPutRequester::shared_pointer
  ChannelPutRequesterPtr;
typedef epics::pvAccess::ChannelPutGetRequester::shared_pointer
  ChannelPutGetRequesterPtr;
typedef epics::pvAccess::ChannelRPCRequester::shared_pointer
  ChannelRPCRequesterPtr;
typedef epics::pvAccess::ChannelArrayRequester::shared_pointer
  ChannelArrayRequesterPtr;

// channel requests

typedef epics::pvAccess::ChannelProcess::shared_pointer 
  ChannelProcessPtr;
typedef epics::pvAccess::ChannelGet::shared_pointer
  ChannelGetPtr;
typedef epics::pvAccess::ChannelPut::shared_pointer
  ChannelPutPtr;
typedef epics::pvAccess::ChannelPutGet::shared_pointer
  ChannelPutGetPtr;
typedef epics::pvAccess::ChannelRPC::shared_pointer
  ChannelRPCPtr;
typedef epics::pvAccess::ChannelArray::shared_pointer
  ChannelArrayPtr;

// monitors

typedef epics::pvData::MonitorRequester::shared_pointer
  MonitorRequesterPtr;
typedef epics::pvData::Monitor::shared_pointer
  MonitorPtr;
typedef epics::pvData::MonitorElement::shared_pointer
  MonitorElementPtr;

}}

#endif
