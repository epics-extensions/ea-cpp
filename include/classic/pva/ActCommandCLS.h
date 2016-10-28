#ifndef EA4_PVA_ACT_COMMAND_CLS_H
#define EA4_PVA_ACT_COMMAND_CLS_H

#include "pva/ActCommand.h"
#include "pva/CollectActionCLS.h"

namespace ea4 { namespace pva {

/** Command for processing actions */
class ActCommandCLS : public ActCommand {

  public:

  /** constructor */
  ActCommandCLS();

 public:

  // ArchiveCommand API

  /** factory method */
  virtual ArchiveCommandPtr createCommand();

public:

  // ArchiveRPCCommand API

  /** process the action  */
  virtual PVStructurePtr process(PVStructurePtr const & request);

 protected:

  CollectActionCLS collectAction;


};


}}

#endif
