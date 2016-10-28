#ifndef EA4_ARCHIVE_RPC_SERVICE_MONGO_H
#define EA4_ARCHIVE_RPC_SERVICE_MONGO_H

#include "pva/ArchiveRPCService.h"

namespace ea4 { namespace pva {

/** Archive RPC data service based on the pvAccess RPC interface */
class ArchiveRPCServiceMONGO : public ArchiveRPCService {

  public:

  /** constructor */
  ArchiveRPCServiceMONGO();

   /** Pointer definitions */
   POINTER_DEFINITIONS(ArchiveRPCServiceMONGO);

};

}}

#endif
