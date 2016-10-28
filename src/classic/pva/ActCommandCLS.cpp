#include "pva/ActCommandCLS.h"

#include <iostream>

using namespace std;
using namespace std::tr1;
using namespace ea4::pva;
using namespace epics::pvData;

// #define DEBUG_COMMAND

namespace ea4 { namespace pva {

ActCommandCLS::ActCommandCLS(){
}

ArchiveCommandPtr ActCommandCLS::createCommand(){
  ArchiveCommandPtr command(new ActCommandCLS());
  return command;
}

PVStructurePtr ActCommandCLS::process(PVStructurePtr const & msg){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  Action::Request request;
  getRequestMsg(msg, request);

#ifdef DEBUG_COMMAND

  cout << "ActCommandCLS::process: src id: " << request.query.src.id 
       << ", query size: " << request.query.queries.size()
       << ", action: " << request.action.cmd << endl;

#endif

  PVStructurePtr response = collectAction.apply(request);
  return response;
}



}}
