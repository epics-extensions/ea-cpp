#ifndef EA4_ARCHIVE_RPC_SERVICE_H
#define EA4_ARCHIVE_RPC_SERVICE_H

#include <memory>
#include "pva/Def.h"
#include "pva/ArchiveService.h"

namespace ea4 { namespace pva {

class ArchiveRPCCommand;

/** Basis class of the EA4 RPC services */
class ArchiveRPCService : public ArchiveService {

 public:

   /** Pointer definitions */
   POINTER_DEFINITIONS(ArchiveRPCService);

   /** destructor */
   virtual ~ArchiveRPCService();

 public:

   /** Returns the selected RPC command (such as  \"getInfo\", \"getArchives\",
      \"getChannels\", and \"getValues\")*/
   ArchiveRPCCommand* const getRPCCommand(const std::string& name) const;

 public:

   /** defines a service name (eg nsls2) */
   virtual void setServiceName(const std::string& name);

protected:

   /** commands */
   std::map<std::string, ArchiveRPCCommand*> rpcCommands;

};

typedef ArchiveRPCService::shared_pointer ArchiveRPCServicePtr;

}}

#endif
