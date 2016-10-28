#ifndef EA4_ARCHIVE_MONITOR_COMMAND_H
#define EA4_ARCHIVE_MONITOR_COMMAND_H

#include "pva/Def.h"
#include "pva/ArchiveCommand.h"


namespace ea4 { namespace pva {

class ArchiveMonitorRequest;

class ArchiveMonitorCommand;
typedef std::tr1::shared_ptr<ArchiveMonitorCommand> ArchiveMonitorCommandPtr;

/** Basis class of the Monitor commands */
class ArchiveMonitorCommand : public ArchiveCommand, public epicsThreadRunable {

 public: 

  // Runnable API

  /** empty */
  virtual void run () {}

  /** empty */
  virtual void show ( unsigned int level ) const {} 

  // public:
  // factory method 
  // virtual ArchiveMonitorCommandPtr createCommand() = 0;

 public:

  /** sets a monitor  */
  inline void setMonitorRequest(std::tr1::shared_ptr<ArchiveMonitorRequest> monitor);

  /** returns a monitor */
  inline std::tr1::shared_ptr<ArchiveMonitorRequest> getMonitorRequest();  

 protected:

  /** monitor of this command */
  std::tr1::shared_ptr<ArchiveMonitorRequest> monitor;

};

inline void 
ArchiveMonitorCommand::setMonitorRequest(std::tr1::shared_ptr<ArchiveMonitorRequest> m){
  monitor = m;
}

inline std::tr1::shared_ptr<ArchiveMonitorRequest> 
ArchiveMonitorCommand::getMonitorRequest() {
  return monitor;
}

}}


#endif
