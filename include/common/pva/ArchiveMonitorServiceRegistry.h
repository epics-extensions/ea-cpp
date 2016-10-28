#ifndef EA4_ARCHIVE_MONITOR_SERVICE_REGISTRY_H
#define EA4_ARCHIVE_MONITOR_SERVICE_REGISTRY_H

#include <map>
#include "pva/ArchiveMonitorService.h"

namespace ea4 { namespace pva {

/** Registry of the EA4 monitor service (defined in the main program) */
class ArchiveMonitorServiceRegistry {

 public:

  /** returns singleton */
  static ArchiveMonitorServiceRegistry* getInstance();

  /** registers a monitor service */
  inline void setMonitorService(ArchiveMonitorServicePtr as);

  /** returns a monitor service */
  inline ArchiveMonitorServicePtr getMonitorService();

 private:

  static ArchiveMonitorServiceRegistry* theInstance;

  ArchiveMonitorServicePtr service;


};

inline void ArchiveMonitorServiceRegistry::setMonitorService(ArchiveMonitorServicePtr s){
    service = s;
}

inline ArchiveMonitorServicePtr ArchiveMonitorServiceRegistry::getMonitorService(){
  return service;
}

}}


#endif
