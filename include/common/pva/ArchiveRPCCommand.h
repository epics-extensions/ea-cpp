#ifndef EA4_ARCHIVE_RPC_COMMAND_H
#define EA4_ARCHIVE_RPC_COMMAND_H

#include "pva/ArchiveCommand.h"

namespace ea4 { namespace pva {

/** Basis class of the RPC commands */
class ArchiveRPCCommand : public ArchiveCommand {

 public:

  /** processes the request */
  virtual PVStructurePtr process(PVStructurePtr const & request) = 0;

};

}}

#endif
