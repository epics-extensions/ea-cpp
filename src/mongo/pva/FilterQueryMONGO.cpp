
#include "pva/FilterQueryMONGO.h"
#include "tools/MsgLogger.h"

#include "pva/Archives.h"
#include "pva/ServerConfigMONGO.h"
#include "pva/Status.h"

#include <sstream>

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;
using namespace mongo;

// #define DEBUG_COMMAND

namespace ea4 { namespace pva {

FilterQueryMONGO::FilterQueryMONGO(){
}


PVStructurePtr FilterQueryMONGO::apply(const string& id, 
				       const PVStructurePtr& input,
				       const PVStructurePtr& pvQuery){

  LOG_MSG("FilterQueryMONGO::apply\n");

#ifdef DEBUG_COMMAND
  std::cout << "FilterQueryMONGO::apply" << std::endl;
#endif

  PVStructurePtr result;
  PVStructurePtr pvStatus = ea4::pva::Status::createPVStructure();

  // access the config data

  ServerConfigMONGO* config = ServerConfigMONGO::getInstance();
  const char* dbName  = config->getDBName();

  if (!dbName) {

    PVIntPtr typeField = pvStatus->getIntField("type");
    typeField->put(3); // fatal

    PVStringPtr messageField = pvStatus->getStringField("message");
    messageField->put("FilterQueryMONGO::apply: dbName is not defined\n");

    result = createEmptyResult(pvStatus);
    return result;
  }

  string ns = dbName;
  ns += ".";
  ns += id;

  LOG_MSG("config: '%s'\n", ns.c_str());

#ifdef DEBUG_COMMAND
  std::cout << "collection: " << ns << std::endl;
#endif

  int count = config->mongo.count(ns.c_str());

#ifdef DEBUG_COMMAND
  std::cout << "total number of docs " << count << std::endl;
#endif

  // Prepare the query request

  BSONObj bQuery;
  bQuery = getQuery(id, pvQuery);

#ifdef DEBUG_COMMAND
  std::cout << "query request: " << bQuery.toString() << std::endl;
#endif

  result = createResult(pvStatus, bQuery);
  return result;
}

PVStructurePtr FilterQueryMONGO::createEmptyResult(PVStructurePtr& pvStatus){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(1);
  PVFieldPtrArray fields(1);

  names[0]  = "status";
  fields[0] = pvStatus;

  PVStructurePtr result = dataFactory->createPVStructure(names, fields);
  return result;
}

PVStructurePtr FilterQueryMONGO::createResult(PVStructurePtr& pvStatus, 
					      BSONObj& bQuery){

  PVStructurePtr result;
  PVDataCreatePtr dataFactory = getPVDataCreate();

  PVStructurePtr pvDocs = createDocs(pvStatus, bQuery); 
  if(pvDocs.get() == 0) {
    result = createEmptyResult(pvStatus);
    return result;
  }

  // Status: OK
  
  pvStatus->getIntField("type")->put(0);

  // compose a result

  StringArray names(2);
  PVFieldPtrArray fields(2);

  names[0]  = "status";
  fields[0] = pvStatus;

  names[1]  = "docs";
  fields[1] = pvDocs;

  result = dataFactory->createPVStructure(names, fields);
  return result;
}

PVStructurePtr FilterQueryMONGO::createDocs(PVStructurePtr& pvStatus, 
					    BSONObj& bQuery){

  PVStructurePtr result;
  PVDataCreatePtr dataFactory = getPVDataCreate();

 // Process the query request

  ServerConfigMONGO* config = ServerConfigMONGO::getInstance();

  const string& dbName = config->getDBName();

  /*

  BSONObj queryResult;
  bool flag = config->mongo.runCommand(dbName, bQuery, queryResult);

#ifdef DEBUG_COMMAND
  std::cout << "query result: " + queryResult.toString() << std::endl;
#endif

  if(!flag){
    string message = "error with query: ";
    message += bQuery.toString();
    pvStatus->getStringField("message")->put(message);
    pvStatus->getIntField("type")->put(3); // error
    return result;
  }

  vector< BSONElement > docs = queryResult["result"].Array();

  std::list<PVFieldPtr> lFields;
  for(int i=0; i < docs.size(); i++){
    lFields.push_back(createPVDoc(docs[i].Obj(), false));
  }

  */

  string ns = dbName;
  ns += ".";
  ns += bQuery.getStringField("aggregate");

  BSONObj matchObj;
  BSONObj projectObj;

  BSONElement pipeObject = bQuery.getField("pipeline");
  if(pipeObject.ok()){
    vector<BSONElement> pipeline = pipeObject.Array();
    if(pipeline.size() > 0) {
      matchObj = pipeline[0].Obj().getField("$match").Obj();
    }
    if(pipeline.size() > 1) {
      projectObj = pipeline[1].Obj().getField("$project").Obj();
    }

  }

#ifdef DEBUG_COMMAND
  cout << "ns: " << ns << endl;
  cout << "match: " << matchObj.toString() << std::endl;
  cout << "project: " << projectObj.toString() << std::endl;
  std::cout << "query result: " << std::endl;
#endif

  auto_ptr<DBClientCursor> cursor = 
    config->mongo.query(ns.c_str(), matchObj, 0, 0, &projectObj);

  std::list<PVFieldPtr> lFields;
  int i = 0;
  while (cursor->more()){
    BSONObj p  = cursor->next();
    lFields.push_back(createPVDoc(p, false));
  }


 // allocate containers

  int size = lFields.size();
  PVIntPtr sizeField = 
    static_pointer_cast<PVInt>(dataFactory->createPVScalar(pvInt));
  sizeField->put(size);

  StringArray names(size + 1);
  PVFieldPtrArray fields(size + 1);

  names[0] = "size";
  fields[0] = sizeField;

  int counter = 0;
  std::list<PVFieldPtr>::iterator it;
  for(it = lFields.begin(); it != lFields.end(); it++){
    stringstream ss; 
    ss << counter;
    names[counter+1]  = ss.str();
    fields[counter+1] = *it;
    counter++;
  }

  result = dataFactory->createPVStructure(names, fields);
  return result;

}

PVStructurePtr FilterQueryMONGO::createPVDoc(const BSONObj& doc, 
					     bool withOID){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  int nFields = doc.nFields();

  int size = nFields;
  if(!withOID) size = nFields - 1;

  StringArray names(size);
  PVFieldPtrArray fields(size);

#ifdef DEBUG_COMMAND
  //  std::cout << "element: " << doc.toString() << std::endl;
#endif

  BSONObj::iterator it = doc.begin();
  if(!withOID) it.next();

  int i = 0;
  for(it; it.more(); ) {

    BSONElement elem = it.next();
    BSONType elemType  = elem.type();

    names[i] = elem.fieldName();

    //     std::cout << i << ", " << names[i] << ", " << elemType << std::endl;

    switch(elemType){ 
    case NumberDouble: /* 1 */
      {
        PVDoublePtr pvd = 
	  static_pointer_cast<PVDouble>(dataFactory->createPVScalar(pvDouble));
	pvd->put(elem.Double());
	fields[i] = pvd;
      }
      break;	 
    case mongo::String: /* 2 */
      {
        PVStringPtr pvstr = 
	  static_pointer_cast<PVString>(dataFactory->createPVScalar(pvString));
	pvstr->put(elem.String());
	fields[i] = pvstr;
      }
      break;
    case mongo::Object: /* 3 */
      {
	PVStructurePtr pvs = createPVDoc(elem.Obj(), true);
	fields[i] = pvs;
      }
      break;
    case mongo::Array: /* 4 */
      {
	PVFieldPtr pvArray = createPVArray(elem);
	fields[i] = pvArray;
	// PVDoublePtr pvd = 
	//   static_pointer_cast<PVDouble>(dataFactory->createPVScalar(pvDouble));
	// pvd->put(0.0);
	// fields[i] = pvd;
      }
      break;
    case mongo::jstOID: /* 7 */
     {
        PVStringPtr pvstr = 
	  static_pointer_cast<PVString>(dataFactory->createPVScalar(pvString));
	pvstr->put(elem.OID().toString());
	fields[i] = pvstr;
     }
     break;
    case mongo::Bool: /* 8 */
      {
        PVBooleanPtr pvb = 
	  static_pointer_cast<PVBoolean>(dataFactory->createPVScalar(pvBoolean));
	pvb->put(elem.boolean());
	fields[i] = pvb;
      }
      break;
    case Date: /* 9 */
      {
	Date_t date = elem.Date();
	PVDoublePtr pvd = 
	  static_pointer_cast<PVDouble>(dataFactory->createPVScalar(pvDouble));
	
	int64_t ms  = date.asInt64(); // secs*1000 + ms
	long secs   = ms/1000;        // secs
	double ts   = ms - secs*1000; // ms
	ts /= 1000;
	ts += secs;

	pvd->put(ts);
	fields[i] = pvd;
      }
      break;
    case NumberInt: /* 16 */
      {
	PVIntPtr pvi = 
	  static_pointer_cast<PVInt>(dataFactory->createPVScalar(pvInt));
	pvi->put(elem.Int());
	fields[i] = pvi;
      }
      break;
    case NumberLong:
      {
        PVLongPtr pvl = 
	  static_pointer_cast<PVLong>(dataFactory->createPVScalar(pvLong));
	pvl->put(elem.Long());
	fields[i] = pvl;
      }
      break; 
    }
    i++;
  }

  PVStructurePtr pvElement = dataFactory->createPVStructure(names, fields);
#ifdef DEBUG_COMMAND
  std::string elementTxt;
  pvElement->toString(&elementTxt);
  std::cout << "pv element: " << elementTxt << std::endl;
#endif

  return pvElement;
}

PVFieldPtr FilterQueryMONGO::createPVArray(const BSONElement& elem){

  PVDataCreatePtr dataFactory = getPVDataCreate();
  PVFieldPtr pvArray = dataFactory->createPVScalarArray(pvDouble);
 
  vector<BSONElement> elems = elem.Array();
  int size = elems.size();

  if(size == 0) return pvArray;

  BSONType elemType  = elems[0].type();
  const char* elemName = elem.fieldName();

  switch(elemType){ 
    case NumberDouble: /* 1 */
    case NumberInt: /* 16 */
    case NumberLong:
      pvArray = createPVDoubleArray(elems);
      break; 
  case mongo::Object: /* 3 */
      pvArray = createPVStructArray(elems);
      break;
   case mongo::String: /* 2 */
      pvArray = createPVStringArray(elems);
      break;
    default:
      cout << "wrong type: " << elemType << ", " << elemName << std::endl;
      return pvArray;
    }

  return pvArray;
}

PVFieldPtr FilterQueryMONGO::createPVStringArray(const vector<BSONElement>& elems){

  PVDataCreatePtr dataFactory = getPVDataCreate();
  PVStringArrayPtr pvArray = 
     std::tr1::static_pointer_cast<epics::pvData::PVStringArray>(
	   dataFactory->createPVScalarArray(pvString));

  // std::tr1::shared_ptr< std::vector<string> > 
  //   shared_values (new std::vector<string>(elems.size()));
  // std::vector<string>& values = *shared_values.get();

  PVStringArray::svector values;
  values.resize(elems.size());

  stringstream ss;

  for(int i=0; i < elems.size(); i++) {

    BSONType elemType  = elems[i].type();

    switch(elemType){ 
    case NumberDouble: /* 1 */
      ss <<  elems[i].Double();
      values[i] = ss.str();
      break;
    case mongo::String: /* 2 */
      values[i] = elems[i].String();
      break;
    case NumberInt: /* 16 */
      ss << elems[i].Int();
      values[i] = ss.str();
      break;
    case NumberLong:
      ss << elems[i].Long();
      values[i] = ss.str();
      break; 
    default:
      cout << "String: wrong type: " << elemType << std::endl;
      return pvArray;
    }
  }

  PVStringArray::const_svector cvalues(freeze(values));

  // pvArray->shareData(shared_values, shared_values->size(), shared_values->size());
  pvArray->replace(cvalues); 

  return pvArray;
}

PVFieldPtr FilterQueryMONGO::createPVDoubleArray(const vector<BSONElement>& elems){

  PVDataCreatePtr dataFactory = getPVDataCreate();
  PVDoubleArrayPtr pvArray = 
     std::tr1::static_pointer_cast<epics::pvData::PVDoubleArray>(
	   dataFactory->createPVScalarArray(pvDouble));

  // std::tr1::shared_ptr< std::vector<double> > 
  //   shared_values (new std::vector<double>(elems.size()));
  // std::vector<double>& values = *shared_values.get();
  PVDoubleArray::svector values;
  values.resize(elems.size());

  for(int i=0; i < elems.size(); i++) {

    BSONType elemType  = elems[i].type();

    switch(elemType){ 
    case NumberDouble: /* 1 */
      values[i] = elems[i].Double();
      break;
    case NumberInt: /* 16 */
      values[i] = elems[i].Int();
      break;
    case NumberLong:
      values[i] = elems[i].Long();
      break; 
    default:
      cout << "wrong type: " << elemType << std::endl;
      return pvArray;
    }
  }

  PVDoubleArray::const_svector cvalues(freeze(values));

  // pvArray->shareData(shared_values, shared_values->size(), shared_values->size());
  pvArray->replace(cvalues); 

  return pvArray;
}

PVFieldPtr FilterQueryMONGO::createPVStructArray(const vector<BSONElement>& elems){

  PVDataCreatePtr dataFactory = getPVDataCreate();
  PVFieldPtr pvArray = dataFactory->createPVScalarArray(pvDouble);
 
  int size = elems.size();

  // create array of structures

  StringArray names(size + 1);
  PVFieldPtrArray fields(size + 1);

  PVIntPtr sizeField = 
    static_pointer_cast<PVInt>(dataFactory->createPVScalar(pvInt));
  sizeField->put(size);

  names[0] = "size";
  fields[0] = sizeField;

  for(int i = 0; i < size; i++){
    stringstream ss; 
    ss << i;
    names[i+1]  = ss.str();

    PVFieldPtr pvElem = createPVDoc(elems[i].Obj(), true);
    if(pvElem.get() == 0) return pvArray;
    fields[i+1] = pvElem;
  }

  pvArray = dataFactory->createPVStructure(names, fields);

#ifdef DEBUG_COMMAND
  std::string arrayTxt;
  pvArray->toString(&arrayTxt);
  std::cout << "pv array: " << arrayTxt << std::endl;
#endif

  return pvArray;
}

// prepare query

BSONObj FilterQueryMONGO::getQuery(const std::string& id, 
				   const PVStructurePtr& query){

  BSONObjBuilder b;

  if(query.get() == 0){   
    return createQuery(id);   
  }

  PVStringPtr aggregateField = query->getStringField("aggregate");
  b.append("aggregate", aggregateField->get());

  PVStructurePtr pvPipe = query->getStructureField("pipeline");
  getPipe(pvPipe, b);

  return b.obj();
}

BSONObj FilterQueryMONGO::createQuery(const std::string& id){

  // aggregate 

  BSONObjBuilder b1;   
  b1.append("aggregate", id);

  // pipeline

  vector<BSONObj> pipeline(1);

  // $match
  
  BSONObjBuilder b2;
  b2.append("$match", BSONObj());
  pipeline[0] = b2.obj();

  b1.append("pipeline", pipeline); 

  return b1.obj();   
}

void FilterQueryMONGO::getPipe(const PVStructurePtr& pvPipe, BSONObjBuilder& bp){

  PVIntPtr sizeField = pvPipe->getIntField("size");
  int size = sizeField->get();

  vector<BSONObj> pipeline(size);

  for(int i = 0; i < size; i++) {
    ostringstream ss; ss << i;
    BSONObjBuilder b;
    PVStructurePtr elem = pvPipe->getStructureField(ss.str());
    StructureConstPtr elemType = elem->getStructure();
    const vector<string>& fnames = elemType->getFieldNames();
    const PVFieldPtrArray& pvFields = elem->getPVFields();
    appendDict(fnames[0], static_pointer_cast< PVStructure >(pvFields[0]), b);
    pipeline[i] = b.obj();
  }

  bp.append("pipeline", pipeline);
}

void FilterQueryMONGO::appendDict(const string& fieldName, 
				  const PVStructurePtr& dict, 
				  BSONObjBuilder& bp){

  BSONObjBuilder b;

  StructureConstPtr dictType = dict->getStructure();

  const vector<string>& fnames = dictType->getFieldNames();
  const PVFieldPtrArray& pvFields = dict->getPVFields();

  for(int i = 0; i < fnames.size(); i++) {

    FieldConstPtr field = dictType->getField(i);
    Type tk = field->getType();

    switch(tk) {
    case scalar:
      {
      const Scalar* s = static_cast<const Scalar*>(field.get());
      appendScalar(s->getScalarType(), fnames[i], pvFields[i], b);
      }
      break;
    case scalarArray:
    case structure:
      appendDict(fnames[i], static_pointer_cast< PVStructure >(pvFields[i]), b);
      break;
    case structureArray:    
      break;
    }
  }

  bp.append(fieldName, b.obj());
}

void FilterQueryMONGO::appendScalar(ScalarType fieldType, 
				    const string& fieldName, 
				    const PVFieldPtr& pvField, 
				    BSONObjBuilder& b){
#define FROM_PV(TK, PVTYPE, MTYPE)						  \
   case TK:                                                                       \
      {                                                                           \
     std::tr1::shared_ptr< PVTYPE > pv = static_pointer_cast< PVTYPE >(pvField);  \
     b.append(fieldName, (MTYPE) pv->get());				          \
     }                                                                            \
     break;

   switch(fieldType){
     FROM_PV(pvBoolean, PVBoolean, int)
     FROM_PV(pvByte, PVByte, int)
     FROM_PV(pvUByte, PVUByte, int)
     FROM_PV(pvShort, PVShort, int)
     FROM_PV(pvUShort, PVUShort, int)
     FROM_PV(pvInt, PVInt, int)
     FROM_PV(pvUInt, PVUInt, unsigned int)
     FROM_PV(pvLong, PVLong, long long)
     FROM_PV(pvULong, PVULong, long long)
     FROM_PV(pvFloat, PVFloat, double)
     FROM_PV(pvDouble, PVDouble, double)
     FROM_PV(pvString, PVString, const string)
   }
}


}}
