#include "pva/ActCommand.h"

#include <sstream>
#include <string>
#include <iostream>

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;

// #define DEBUG_COMMAND

namespace ea4 { namespace pva {

string ActCommand::commandName = "act";

ActCommand::ActCommand(){
}

PVStructurePtr ActCommand::createRequest(){
  PVStructurePtr nullPtr;
  return nullPtr;
}

StructureConstPtr ActCommand::createResultType(){
  StructureConstPtr nullPtr;
  return nullPtr;
}

PVStructurePtr 
ActCommand::createRequestMsg(const Action::Request& request){

  PVDataCreatePtr df = getPVDataCreate();

  StringArray names(3);
  PVFieldPtrArray fields(3);

  PVStringPtr cmdField = 
    static_pointer_cast<PVString>(df->createPVScalar(pvString));
  cmdField->put("act");

  names[0] = "command";
  fields[0] = cmdField;

  names[1] = "query";
  fields[1] = Query::createRequestMsg(request.query);

  names[2] = "action";
  fields[2] = createActionField(request);

  PVStructurePtr msg =  df->createPVStructure(names, fields);
  return msg;
}

void ActCommand::getRequestMsg(const PVStructurePtr & msg, 
			       Action::Request& request){

  // query

  PVStructurePtr queryField = msg->getStructureField("query");
  Query::getRequestMsg(queryField, request.query);

  // action

  PVStructurePtr actionField = msg->getStructureField("action");
  request.action.cmd = actionField->getStringField("cmd")->get();
}

StructureConstPtr 
ActCommand::createRequestType(const Action::Request& request){

  FieldCreatePtr tf = getFieldCreate();

  // make a structure type

  StringArray names;
  FieldConstPtrArray fields;

  names.push_back("command");
  fields.push_back(tf->createScalar(pvString));

  StructureConstPtr queryRequestType = 
    Query::createRequestType(request.query);

  names.push_back("query");
  fields.push_back(queryRequestType->getField("query"));

  names.push_back("action");
  fields.push_back(createActionType());

  StructureConstPtr requestType =  tf->createStructure(names, fields);
  return requestType;
}

PVStructurePtr 
ActCommand::createActionField(const Action::Request& request){

  StructureConstPtr actionType = createActionType();

  PVDataCreatePtr dataFactory = getPVDataCreate();
  PVStructurePtr actionField =  dataFactory->createPVStructure(actionType);

  PVStringPtr acmdField = actionField->getStringField("cmd");
  acmdField->put(request.action.cmd.c_str());

  return actionField;
}

StructureConstPtr ActCommand::createActionType(){

  FieldCreatePtr tf = getFieldCreate();

  StringArray names;
  FieldConstPtrArray fields;

  names.push_back("cmd");
  fields.push_back(tf->createScalar(pvString));

  StructureConstPtr actionType =  tf->createStructure(names, fields);
  return actionType;
}


}}
