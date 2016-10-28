#ifndef EA4_ARCHIVE_RPC_SERVICE_EA3_H
#define EA4_ARCHIVE_RPC_SERVICE_EA3_H

#include "pva/ArchiveRPCService.h"

namespace ea4 { namespace pva {

  /** Archive RPC data service based on the pvAccess RPC interface */
class ArchiveRPCServiceEA3 : public ArchiveRPCService {

  public:

    ArchiveRPCServiceEA3();

   /** Pointer definitions */
   POINTER_DEFINITIONS(ArchiveRPCServiceEA3);

  public:

   /** Primary/Only method of the pvAccess RPCService */
   // epics::pvData::PVStructurePtr 
   //  request(epics::pvData::PVStructurePtr const & args ) 
   //  throw (epics::pvAccess::RPCRequestException);

};

  }}

#endif
