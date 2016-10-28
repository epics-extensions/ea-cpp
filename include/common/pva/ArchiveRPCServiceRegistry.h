#ifndef EA4_ARCHIVE_RPC_SERVICE_REGISTRY_H
#define EA4_ARCHIVE_RPC_SERVICE_REGISTRY_H

#include <map>
#include "pva/ArchiveRPCService.h"

namespace ea4 { namespace pva {

/** Registry of the EA4 rpc service (defined in the main program) */
class ArchiveRPCServiceRegistry {

 public:

  /** returns singleton */
  static ArchiveRPCServiceRegistry* getInstance();

  /** registers a rpc service */
  inline void setRPCService(ArchiveRPCServicePtr as);

  /** returns a rpc service */
  inline ArchiveRPCServicePtr getRPCService();

 private:

  static ArchiveRPCServiceRegistry* theInstance;

  ArchiveRPCServicePtr service;


};

inline void ArchiveRPCServiceRegistry::setRPCService(ArchiveRPCServicePtr s){
    service = s;
}

inline ArchiveRPCServicePtr ArchiveRPCServiceRegistry::getRPCService(){
  return service;
}

}}


#endif
