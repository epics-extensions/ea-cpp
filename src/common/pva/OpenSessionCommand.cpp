#include "pva/OpenSessionCommand.h"

#include <iostream>

using namespace std;
using namespace ea4::pva;
using namespace epics::pvData;

namespace ea4 { namespace pva {

string OpenSessionCommand::commandName = "openSession";

OpenSessionCommand::OpenSessionCommand(){
  createRequestType();
  createResponseType();
}

ArchiveCommandPtr OpenSessionCommand::createCommand(){
  ArchiveCommandPtr command(new OpenSessionCommand());
  return command;
}

PVStructurePtr OpenSessionCommand::process(PVStructurePtr const & request){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  string name;
  getSessionName(request, name);

  int status = 1;
  PVStructurePtr response =  dataFactory->createPVStructure(responseType);
  setStatus(status, response);

  return response;
}

void OpenSessionCommand::setSessionName(const string& name, 
					PVStructurePtr& request){

    PVStringPtr nameField = request->getStringField("name");
    nameField->put(name.c_str());
}

void OpenSessionCommand::getSessionName(const PVStructurePtr & request, 
					string& name){
  name = request->getStringField("name")->get();  
}

void OpenSessionCommand::setStatus(int status, 
				   PVStructurePtr& result){

    PVIntPtr statusField = result->getIntField("status");
    statusField->put(status);
 }

void OpenSessionCommand::getStatus(const PVStructurePtr & result, 
				   int& status){
  status = result->getIntField("status")->get();  
}

void OpenSessionCommand::createRequestType(){

  FieldCreatePtr typeFactory = getFieldCreate();

  // make a structure type

  StringArray names;
  FieldConstPtrArray fields;

  names.push_back("command");
  fields.push_back(typeFactory->createScalar(pvString));

  names.push_back("name");
  fields.push_back(typeFactory->createScalar(pvString));

  requestType =  typeFactory->createStructure(names, fields);
}

void OpenSessionCommand::createResponseType(){

  FieldCreatePtr typeFactory = getFieldCreate();

  // make a structure type

  StringArray names;
  FieldConstPtrArray fields;

  names.push_back("status");
  fields.push_back(typeFactory->createScalar(pvInt));

  responseType =  typeFactory->createStructure(names, fields);
}

PVStructurePtr OpenSessionCommand::createRequest(){

  PVDataCreatePtr dataFactory = getPVDataCreate();
  PVStructurePtr request =  dataFactory->createPVStructure(requestType);

  PVStringPtr commandField = request->getStringField("command");
  commandField->put(commandName);

  return request;
}


}}
