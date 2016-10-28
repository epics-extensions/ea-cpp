#ifndef EA4_PVA_ACT_COMMAND_H
#define EA4_PVA_ACT_COMMAND_H

#include "pva/Action.h"
#include "pva/ArchiveRPCCommand.h"

namespace ea4 { namespace pva {

/** Basis class of commands processing actions */
class ActCommand : public ArchiveRPCCommand {

  public:

  /** constructor */
  ActCommand();

 public:

  // ArchiveCommand API

  /** returns 'act' */
  inline const char* getName() const;

  /** returns null (deprecated) */
  virtual PVStructurePtr createRequest();

  /** (deprecated) */
  virtual void setRequest(PVStructurePtr const & request) {}

  /** returns null (deprecated) */
  StructureConstPtr createResultType();

  /** factory method */
  // virtual ArchiveCommandPtr createCommand();

public:

  // ArchiveRPCCommand API

  /** process  the request */
  // virtual PVStructurePtr process(PVStructurePtr const & request);

 public:

  // Helpers

  /** defines the pvData request message from the Request structure */
  static PVStructurePtr createRequestMsg(const Action::Request& request);

  /** extract the Request structure from the pvData request message */
  static void getRequestMsg(PVStructurePtr const & msg, Action::Request& request);

  /** creates a request type structure 
    * { String command, Query query, Action action } 
  */
  static  StructureConstPtr createRequestType(const Action::Request& request);



 protected:

  /** creates an Action field  */
  static PVStructurePtr createActionField(const Action::Request& request);

  /** creates an Action type of the Request structure */
  static StructureConstPtr createActionType();

 protected:

  /** command name: act */
  static std::string commandName;

};

inline const char* ActCommand::getName() const { 
  return commandName.c_str(); 
}



}}

#endif
