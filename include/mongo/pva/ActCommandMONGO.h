#ifndef EA4_PVA_ACT_COMMAND_MONGO_H
#define EA4_PVA_ACT_COMMAND_MONGO_H

#include "pva/ActCommand.h"
#include "pva/CollectActionMONGO.h"

namespace ea4 { namespace pva {

/** Command for processing the collection action */
class ActCommandMONGO : public ActCommand {

  public:

  /** constructor */
  ActCommandMONGO();

 public:

  // ArchiveCommand API

  /** factory method */
  virtual ArchiveCommandPtr createCommand();

public:

  // ArchiveRPCCommand API

  /** process the action */
  virtual PVStructurePtr process(PVStructurePtr const & request);

 protected:

  CollectActionMONGO collectAction;

};


}}

#endif
