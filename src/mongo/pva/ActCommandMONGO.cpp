#include "pva/ActCommandMONGO.h"

#include <iostream>

using namespace std;
using namespace std::tr1;
using namespace ea4::pva;
using namespace epics::pvData;

namespace ea4 { namespace pva {

#define DEBUG_COMMAND

ActCommandMONGO::ActCommandMONGO(){
}

ArchiveCommandPtr ActCommandMONGO::createCommand(){
  ArchiveCommandPtr command(new ActCommandMONGO());
  return command;
}

PVStructurePtr ActCommandMONGO::process(PVStructurePtr const & msg){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  Action::Request request;
  getRequestMsg(msg, request);

#ifdef DEBUG_COMMAND
  cout << "ActCommandMONGO::process: src id: " << request.query.src.id 
       << ", query size: " << request.query.queries.size()
       << ", action: " << request.action.cmd << endl;
#endif

  PVStructurePtr response = collectAction.apply(request);

  return response;
}



}}
