#include "pva/Query.h"

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;

// #define DEBUG_COMMAND

namespace ea4 { namespace pva {

Query::Request::Request(){
}

Query::Request::Request(const Query::Request& request){

  src.id = request.src.id;

  list<PVStructurePtr>::const_iterator it;
  for(it = request.queries.begin(); it != request.queries.end(); it++){
    queries.push_back(*it);
  } 
}

Query::Request& Query::Request::operator=(const Query::Request& request){

  src.id = request.src.id;

  queries.clear();

  list<PVStructurePtr>::const_iterator it;
  for(it = request.queries.begin(); it != request.queries.end(); it++){
    queries.push_back(*it);
  } 

  return *this;
}

// Helpers

PVStructurePtr 
Query::createRequestMsg(const Query::Request& request){

  PVDataCreatePtr df = getPVDataCreate();

  StringArray names(2);
  PVFieldPtrArray fields(2);

  names[0] = "src";
  fields[0] = createSrcField(request);

  names[1] = "queries";
  fields[1] = createQueriesField(request);

  PVStructurePtr queryField =  df->createPVStructure(names, fields);
  return queryField;
}

void Query::getRequestMsg(const PVStructurePtr & msg, 
			  Query::Request& query){

#ifdef DEBUG_COMMAND
    string builder;
    msg->toString(&builder);
    std::cout << "Query::getRequestMsg, msg : " << builder << std::endl;
#endif

  // query, src

  PVStructurePtr srcField = msg->getStructureField("src");
  query.src.id = srcField->getStringField("id")->get();

  // query, queries

   PVStructurePtr queriesField = msg->getStructureField("queries");
   int size = queriesField->getIntField("size")->get();

   for(int i=0; i < size; i++){
     ostringstream ss;
     ss << i;
     PVStructurePtr qField = queriesField->getStructureField(ss.str());
     query.queries.push_back(qField);
   }

}

StructureConstPtr 
Query::createRequestType(const Query::Request& request){

  FieldCreatePtr tf = getFieldCreate();

  // make a structure type

  StringArray names;
  FieldConstPtrArray fields;

  names.push_back("command");
  fields.push_back(tf->createScalar(pvString));

  names.push_back("query");
  fields.push_back(createQueryType(request));

  StructureConstPtr requestType = tf->createStructure(names, fields);
  return requestType;
}

//

StructureConstPtr 
Query::createQueryType(const Query::Request& query){

  FieldCreatePtr tf = getFieldCreate();

  // make a structure type

  StringArray names;
  FieldConstPtrArray fields;

  names.push_back("src");
  fields.push_back(createSrcType());

  names.push_back("queries");
  fields.push_back(createQueriesType(query));

  StructureConstPtr queryType =  tf->createStructure(names, fields);
  return queryType;
}

StructureConstPtr 
Query::createQueriesType(const Query::Request& query){

  FieldCreatePtr tf = getFieldCreate();

  // make a structure type

  StringArray names;
  FieldConstPtrArray fields;

  names.push_back("size");
  fields.push_back(tf->createScalar(pvInt));

  const list<PVStructurePtr>& queries = query.queries;
  list<PVStructurePtr>::const_iterator it;

  int counter = 0;
  for(it = queries.begin(); it != queries.end(); it++){
    ostringstream ss;
    ss << counter;
    names.push_back(ss.str());
    fields.push_back((*it)->getStructure());
    counter++;
  }

  StructureConstPtr queriesType =  tf->createStructure(names, fields);
  return queriesType;
}

PVStructurePtr 
Query::createQueriesField(const Query::Request& query){

  PVDataCreatePtr df = getPVDataCreate();

  const list<PVStructurePtr>& queries = query.queries; 

  StringArray names(queries.size() + 1);
  PVFieldPtrArray fields(queries.size() + 1);

  names[0] = "size";

  PVIntPtr sizeField = 
    static_pointer_cast<PVInt>(df->createPVScalar(pvInt));
  sizeField->put(queries.size());
  fields[0] = sizeField;

  int counter = 0;
  list<PVStructurePtr>::const_iterator it;
  for(it = queries.begin(); it != queries.end(); it++){
    ostringstream ss; ss << counter;
    names[counter+1] = ss.str();
    fields[counter+1] = *it; 
    counter++;
  }

  PVStructurePtr queriesField =  df->createPVStructure(names, fields);
  return queriesField;
}

StructureConstPtr Query::createSrcType(){

  FieldCreatePtr tf = getFieldCreate();

  // make a structure type

  StringArray names;
  FieldConstPtrArray fields;

  names.push_back("id");
  fields.push_back(tf->createScalar(pvString));

  StructureConstPtr srcType =  tf->createStructure(names, fields);
  return srcType;
}

PVStructurePtr 
Query::createSrcField(const Query::Request& query){

  PVDataCreatePtr df = getPVDataCreate();

  StructureConstPtr srcType = createSrcType(); 
  PVStructurePtr srcField =  df->createPVStructure(srcType);

  PVStringPtr idField = srcField->getStringField("id");
  idField->put(query.src.id.c_str());

  return srcField;
}

}}
