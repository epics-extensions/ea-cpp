#ifndef EA4_ARCHIVE_MONITOR_SERVICE_EA3_H
#define EA4_ARCHIVE_MONITOR_SERVICE_EA3_H

#include "pva/Def.h"
#include "pva/ArchiveMonitorService.h"

namespace ea4 { namespace pva {

  /** Archive RPC data service based on the pvAccess RPC interface */
class ArchiveMonitorServiceEA3 : public ArchiveMonitorService {

  public:

    ArchiveMonitorServiceEA3();

   /** Pointer definitions */
   POINTER_DEFINITIONS(ArchiveMonitorServiceEA3);

 public:

   /** Returns the introspection of the data */
   virtual  epics::pvData::StructureConstPtr 
     createResultType(PVStructurePtr const & request ) {}

};

}}

#endif
