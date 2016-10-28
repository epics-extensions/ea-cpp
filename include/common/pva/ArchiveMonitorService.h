#ifndef EA4_ARCHIVE_MONITOR_SERVICE_H
#define EA4_ARCHIVE_MONITOR_SERVICE_H

#include <memory>
#include "pva/Def.h"
#include "pva/ArchiveService.h"

namespace ea4 { namespace pva {

class ArchiveMonitorCommand;

/** Basis class of the EA4 monitor services */
class ArchiveMonitorService : public ArchiveService {

 public:

   /** Pointer definitions */
   POINTER_DEFINITIONS(ArchiveMonitorService);

   /** destructor */
   virtual ~ArchiveMonitorService();

 public:

   /** Returns the selected Monitor command (such as \"streamValues\") */
   ArchiveMonitorCommand* const getMonitorCommand(const std::string& name) const;

 public:

   /** defines a service name (eg nsls2) */
  virtual void setServiceName(const std::string& name);

protected:

  /** commands */
  std::map<std::string, ArchiveMonitorCommand*> monCommands;

};

typedef ArchiveMonitorService::shared_pointer ArchiveMonitorServicePtr;

}}

#endif
