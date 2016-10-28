#ifndef EA4_PVA_OPEN_SESSION_COMMAND_H
#define EA4_PVA_OPEN_SESSION_COMMAND_H

#include "pva/ArchiveRPCCommand.h"

namespace ea4 { namespace pva {

/** OpenSession command */
class OpenSessionCommand : public ArchiveRPCCommand {

  public:

  /** constructor */
  OpenSessionCommand();

public:

  // ArchiveCommand API

  /** returns 'openSession' */
  virtual const char* getName()const { return commandName.c_str(); }

  /** creates the request structure */
  virtual PVStructurePtr createRequest();

  /** initializes internal structures of this command based 
      on the request (empty) */
  virtual void setRequest(PVStructurePtr const & request) {}

  /** returns the introspection of the result structure */
  inline StructureConstPtr createResultType();

  /** factory method */
  virtual ArchiveCommandPtr createCommand();

public:

  // ArchiveRPCCommand API

  /** creates and registers an ION session  */
  virtual PVStructurePtr process(PVStructurePtr const & request);

 public:

  // Helpers

  /** defines the session name of the request msg */
  static void setSessionName(const std::string& id, PVStructurePtr& request);

  /** extracts the session name from the request msg*/
  static void getSessionName(const PVStructurePtr& request, std::string& id);

  /** defines the status of the result msg */
  static void setStatus(int status, PVStructurePtr& result);

  /** extracts the status from the result msg*/
  static void getStatus(const PVStructurePtr& result, int& status);

protected:

  /** creates a request type structure {string command, string name } */
  void createRequestType();

  /** creates a response type structure {int status }*/
  void createResponseType();

 protected:

  /** command name: openSession */
  static std::string commandName;

  /** {string command, string name } */
  StructureConstPtr requestType;  

  /** {int status } */
  StructureConstPtr responseType; 

};

inline StructureConstPtr OpenSessionCommand::createResultType(){
  return responseType;
}


}}

#endif
