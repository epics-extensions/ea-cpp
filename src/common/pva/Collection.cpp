#include "pva/Collection.h"
#include "pva/ActCommand.h"
#include "pv/rpcClient.h"

using namespace std;
using namespace epics::pvData;

namespace ea4 { namespace pva {

Collection::Collection(Session* s, const string& dbns)
  : session(s) {
  request.src.id = dbns;
}

Collection::Collection(Collection* collection, PVStructurePtr query){
  if(query.get() != 0) {
    session = collection->session;
    request = collection->request;
    request.queries.push_back(query);
  }
}

CollectionPtr Collection::createProxy(PVStructurePtr query){
  CollectionPtr proxy(new Collection(this, query));
  return proxy;
}

CollectionPtr Collection::filter(PVStructurePtr query){

  PVStringPtr commandField = query->getStringField("command");
  commandField->put("filter");

  CollectionPtr proxy(new Collection(this, query));
  return proxy;
}

CollectionPtr Collection::read(PVStructurePtr query){
  PVStringPtr commandField = query->getStringField("command");
  commandField->put("read");

  CollectionPtr proxy(new Collection(this, query));
  return proxy;
}

int Collection::cache(double timeout){

  // no transformation
  // if(!parent) return 0;

  // PVStructurePtr message  = createMessage("cache", request);    
  // PVStructurePtr response = session->request(message, timeout);

  return 0;

}

PVStructurePtr Collection::collect(double timeout){

  epics::pvAccess::RPCClientPtr client = session->getClient();

  // prepare the request message

  Action::Request actionRequest;

  actionRequest.query      = request;
  actionRequest.action.cmd = "collect";

  PVStructurePtr requestMsg = ActCommand::createRequestMsg(actionRequest);

  // send the request

  PVStructurePtr response = client->request(requestMsg, timeout);
  return response;

  // extract data from the reponse message
  // BSONObj bson = ProcessCollectionCommand::getResponseMsg(responseMsg);
  // return bson;
}

/*
StructureConstPtr Collection::createMessageType(){

  FieldCreatePtr typeFactory = getFieldCreate();

  // make a structure type

  StringArray names;
  FieldConstPtrArray fields;

  names.push_back("command");
  fields.push_back(typeFactory->createScalar(pvString));

  names.push_back("bson");
  ScalarArrayConstPtr bson = typeFactory->createScalarArray(pvByte);

  StructureConstPtr msgType =  typeFactory->createStructure(names, fields);
  return msgType;
}

PVStructurePtr Collection::createMessage(const string& cmd, mongo::BSONObj request){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  //

  epics::pvData::StructureConstPtr msgType = createMessageType();
  PVStructurePtr message    =  dataFactory->createPVStructure(msgType);

  PVStringPtr commandField = message->getStringField("command");
  commandField->put(cmd);

  const int8_t* objdata = reinterpret_cast<const int8_t*>(request.objdata());
  int objsize = request.objsize();

  PVByteArrayPtr bsonField = 
    std::tr1::static_pointer_cast<PVByteArray>(message->getScalarArrayField("bson", pvByte)); 
  bsonField->put(0, objsize, objdata, 0);

  return message;
}

    */

}}
