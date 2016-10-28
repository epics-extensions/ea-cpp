#ifndef EA4_STREAM_VALUES_COMMAND_H
#define EA4_STREAM_VALUES_COMMAND_H

#include <sstream>

#include "pva/ArchiveMonitorCommand.h"

namespace ea4 { namespace pva {

/** Basis class of the Monitor command returning the channel values */
class StreamValuesCommand : public ArchiveMonitorCommand {

public:

  /** Request structure of the streamValues command */
  struct Request {
    int key;
    std::string name;
    // int start_sec;
    // int start_nano;
    // int end_sec;
    // int end_nano;
    int64_t start;
    int64_t end;
    int count;
    int how;
  };

 public:

  /** constructor */
  StreamValuesCommand();

  public:

  // ArchiveCommand API

  /** returns 'streamValues' */
  inline const char* getName() const;

  /** returns the request structure */
  virtual epics::pvData::PVStructurePtr createRequest();

  /** initializes internal structures of this command based on the request (empty) */
  virtual void setRequest(PVStructurePtr const & request) {}

  /** returns the introspection of the result structure */
  inline  epics::pvData::StructureConstPtr createResultType();

  /** factory method (empty) */
  virtual ArchiveCommandPtr createCommand() {} 

  public:

  /** defines the pvData generic container from the Request structure */
  static void setPVStructure(const Request& input, 
			     epics::pvData::PVStructurePtr & output);

  /** defines the Request structure from the pvData generic container */
  static void setCStructure(const epics::pvData::PVStructurePtr & input, 
			    Request& output);

 protected:

  /** creates a request type structure */
  void createRequestType();

 protected:

  /** command name: streamValues */
  static std::string commandName;

  /** {string command, 
   *  int key, string name, 
   *  int32 start_sec, int32 start_nano, 
   *  int32 end_sec, int32 end_nano, 
   *  int32 count, int32 how } */
  epics::pvData::StructureConstPtr requestType;  

  /** result type (defined by setRequest) */
  epics::pvData::StructureConstPtr resultType;  

};

inline const char* 
StreamValuesCommand::getName() const { 
   return commandName.c_str(); 
}

inline  epics::pvData::StructureConstPtr 
StreamValuesCommand::createResultType(){
  return resultType;
}

  }}

#endif


