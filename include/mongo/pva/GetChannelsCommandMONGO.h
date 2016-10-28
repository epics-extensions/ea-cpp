#ifndef EA4_GET_CHANNELS_COMMAND_MONGO_H
#define EA4_GET_CHANNELS_COMMAND_MONGO_H

#include "pva/GetChannelsCommand.h"

namespace ea4 { namespace pva {

/** MONGO-specific command returning the array of the channel infos */
class GetChannelsCommandMONGO : public GetChannelsCommand {

public:

  // ArchiveCommand API

  /** factory method */
  virtual ArchiveCommandPtr createCommand();

public:

  // ArchiveRPCCommand API

  /** Delegates this request to the RPC service */
  virtual epics::pvData::PVStructurePtr 
    process(epics::pvData::PVStructurePtr const & request);

public:

  // QueryCommand API

  /** Delegates this request to the RPC service */
  virtual epics::pvData::PVStructurePtr process(const Action::Request& request);

 public:

  epics::pvData::PVStructurePtr process(int key, const std::string& pattern);


};

}}

#endif


