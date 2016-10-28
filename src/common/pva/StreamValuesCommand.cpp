#include "pva/StreamValuesCommand.h"
#include "tools/MsgLogger.h"

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;

#define LOGFILE

#define MAX_COUNT 1000000

namespace ea4 { namespace pva {

std::string StreamValuesCommand::commandName = "streamValues";

StreamValuesCommand::StreamValuesCommand(){
  createRequestType();
}

// ArchiveCommand API

PVStructurePtr StreamValuesCommand::createRequest(){

  PVDataCreatePtr dataFactory = getPVDataCreate();
  PVStructurePtr request =  dataFactory->createPVStructure(requestType);

  PVStringPtr commandField = request->getStringField("command");
  commandField->put(commandName);

  return request;
}

void StreamValuesCommand::createRequestType(){

  FieldCreatePtr typeFactory = getFieldCreate();

  // make a structure type

  StringArray names;
  FieldConstPtrArray fields;

  names.push_back("command");
  fields.push_back(typeFactory->createScalar(pvString));

  names.push_back("key");
  fields.push_back(typeFactory->createScalar(pvInt));

  names.push_back("name");
  fields.push_back(typeFactory->createScalar(pvString));   

  names.push_back("start");
  fields.push_back(typeFactory->createScalar(pvLong));

  names.push_back("end");
  fields.push_back(typeFactory->createScalar(pvLong));

  /*

  names.push_back("start_sec");
  fields.push_back(typeFactory->createScalar(pvInt));

  names.push_back("start_nano");
  fields.push_back(typeFactory->createScalar(pvInt));

  names.push_back("end_sec");
  fields.push_back(typeFactory->createScalar(pvInt));

  names.push_back("end_nano");
  fields.push_back(typeFactory->createScalar(pvInt));

  */

  names.push_back("count");
  fields.push_back(typeFactory->createScalar(pvInt));

  names.push_back("how");
  fields.push_back(typeFactory->createScalar(pvInt));

  requestType =  typeFactory->createStructure(names, fields);
}

void StreamValuesCommand::setPVStructure(const Request& input, 
					 PVStructurePtr & output){

    PVIntPtr keyField = output->getIntField("key");
    keyField->put(input.key);

    PVStringPtr nameField = output->getStringField("name");
    nameField->put(input.name.c_str());

    PVLongPtr startField = output->getLongField("start");
    startField->put(input.start);

    PVLongPtr endField = output->getLongField("end");
    endField->put(input.end);

    /*

    PVIntPtr start_secField = output->getIntField("start_sec");
    start_secField->put(input.start_sec);

    PVIntPtr start_nanoField = output->getIntField("start_nano");
    start_nanoField->put(input.start_nano);

    PVIntPtr end_secField = output->getIntField("end_sec");
    end_secField->put(input.end_sec);

    PVIntPtr end_nanoField = output->getIntField("end_nano");
    end_nanoField->put(input.end_nano);

    */

    PVIntPtr countField = output->getIntField("count");
    countField->put(input.count);

    PVIntPtr howField = output->getIntField("how");
    howField->put(input.how);

 }

void StreamValuesCommand::setCStructure(const PVStructurePtr & input, 
					Request& output) {

  PVIntPtr keyField        = input->getIntField("key");
  PVStringPtr nameField    = input->getStringField("name");

  PVLongPtr startField  = input->getLongField("start");
  PVLongPtr endField    = input->getLongField("end");
  /*
  PVIntPtr start_secField  = input->getIntField("start_sec");
  PVIntPtr start_nanoField = input->getIntField("start_nano");
  PVIntPtr end_secField    = input->getIntField("end_sec");
  PVIntPtr end_nanoField   = input->getIntField("end_nano");
  */
  PVIntPtr countField      = input->getIntField("count");
  PVIntPtr howField        = input->getIntField("how");

  output.key        = keyField->get();
  output.name       = nameField->get();

  output.start = startField->get();
  output.end   = endField->get();
  /*
  output.start_sec  = start_secField->get();
  output.start_nano = start_nanoField->get();
  output.end_sec    = end_secField->get();
  output.end_nano   = end_nanoField->get();
  */
  output.count      = countField->get();
  output.how        = howField->get();

#ifdef LOGFILE
    LOG_MSG("how=%d, count=%d\n", output.how, output.count);
#endif

  if (output.count < 1) output.count = 1;
  if (output.count > MAX_COUNT) output.count = MAX_COUNT;
}


  }}

