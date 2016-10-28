#include "pva/PVsQueryCLS.h"
#include "pva/ServerConfigEA3.h"
#include "pva/Archives.h"
#include "pva/Status.h"

#include <map>
#include "tools/RegularExpression.h"

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;

// #define DEBUG_COMMAND

namespace ea4 { namespace pva {

PVsQueryCLS::PVsQueryCLS(){
}

PVStructurePtr PVsQueryCLS::apply(const string& id, 
				  const PVStructurePtr& input,
				  const PVStructurePtr& query){

  LOG_MSG("PVsQueryCLS::apply\n");

  PVStructurePtr result;
  PVStructurePtr pvStatus = Status::createPVStructure();

  ServerConfigEA3* config = ServerConfigEA3::getInstance();
  const char* configName  = config->getConfigName();

  // configName

  if (!configName) {

    PVIntPtr typeField = pvStatus->getIntField("type");
    typeField->put(3); // fatal

    PVStringPtr messageField = pvStatus->getStringField("message");
    messageField->put("PVsQueryCLS::apply: configName is not defined\n");

    result = createEmptyResult(pvStatus);
    return result;
  }

  string pattern = decodeQuery(query);

  result = createResult(pvStatus, id, pattern);
  return result;
}

PVStructurePtr PVsQueryCLS::createEmptyResult(PVStructurePtr& pvStatus){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(1);
  PVFieldPtrArray fields(1);

  names[0]  = "status";
  fields[0] = pvStatus;

  PVStructurePtr result = dataFactory->createPVStructure(names, fields);
  return result;
}

PVStructurePtr PVsQueryCLS::createResult(PVStructurePtr& pvStatus, 
					 const string& id, 
					 const string& pattern){

  PVStructurePtr result;
  PVDataCreatePtr dataFactory = getPVDataCreate();

  PVStructurePtr pvDocs = createDocs(pvStatus, id, pattern); 
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

PVStructurePtr PVsQueryCLS::createDocs(PVStructurePtr& pvStatus, 
				       const string& id, 
				       const string& pattern){

#ifdef DEBUG_COMMAND
  std::cout << "PVsQueryCLS::createDocs" << std::endl;
#endif

  LOG_MSG("PVsQueryCLS::createDocs\n");

  PVStructurePtr result;
  PVDataCreatePtr dataFactory = getPVDataCreate();

  // pattern

  string pattern_txt = "<no pattern>";
  if(pattern.size() > 0)  pattern_txt = pattern;

 // process query

  AutoPtr<RegularExpression> regex;
  map<string, Interval> channels;

  try {

    if (pattern.size() > 0){
      regex.assign(new RegularExpression(pattern.c_str()));
    }

    IndexPtr index = createIndex(id);
    if(index.get() == 0) {
      pvStatus->getStringField("message")->put("index is not defined");
      pvStatus->getIntField("type")->put(2); // error
      return result;
    }

    // Put all names in binary tree

    AutoPtr<Index::NameIterator> ni(index->iterator());

    for (/**/;  ni && ni->isValid(); ni->next()){
     
      if (regex && !regex->doesMatch(ni->getName())) {
	continue; // skip what doesn't match regex
      }

#ifdef DEBUG_COMMAND
      std::cout << "PVsQueryCLS::apply()" << ni->getName() << std::endl;
#endif
            
      AutoPtr<Index::Result> result(index->findChannel(ni->getName()));

      if (result){
	channels[ni->getName()] =  result->getRTree()->getInterval();
      } else { // Is this an error?
	channels[ni->getName()] =  Interval();
      }
    }

    index->close();

  } catch (GenericException &e) {

    LOG_MSG("get_names(%s, '%s') -> error '%s'\n",
	    id.c_str(),
	    pattern_txt.c_str(),
	    (char *) e.what());
    pvStatus->getStringField("message")->put(e.what());
    pvStatus->getIntField("type")->put(2); // error
    return result;
  }

  LOG_MSG("get_names(%s, '%s') -> %d names\n",
	  id.c_str(),
	  pattern_txt.c_str(),
	  channels.size());

  // check size

  int size = channels.size();
  if(!size) {
      pvStatus->getStringField("message")->put("no channels");
      pvStatus->getIntField("type")->put(1); // warning
      return result;
  }

  // perpare the result 

  StringArray names(size + 1);
  PVFieldPtrArray fields(size + 1);

  names[0]  = "size";
  PVIntPtr sizeField = 
    static_pointer_cast<PVInt>(dataFactory->createPVScalar(pvInt));
  sizeField->put(size);
  fields[0] = sizeField;

  int indexPV = 0;
  std::map<string, Interval>::iterator it;
  for(it = channels.begin(); it != channels.end(); it++){
    stringstream ss; ss << indexPV;
    names[indexPV+1] = ss.str();
    PVStructurePtr infoPV = createDoc(it->first, it->second);
    fields[indexPV+1] = infoPV;
    indexPV++;
  }
 
  result = dataFactory->createPVStructure(names, fields);
  return result; 
}

PVStructurePtr PVsQueryCLS::createDoc(const string& name,
				      const Interval& interval){

  PVStructurePtr result;
  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(3);
  PVFieldPtrArray fields(3);

  // name

  names[0] = "pv";
  PVStringPtr nameField =
    static_pointer_cast<PVString>(dataFactory->createPVScalar(pvString));
  nameField->put(name);
  fields[0] = nameField;

  // start

  names[1] = "start"; 
  
  int ss, sn;
  epicsTime2pieces(interval.getStart(), ss, sn);

  double st = sn;
  st /= 1000000000;
  st += ss;

  PVDoublePtr stField = 
    static_pointer_cast<PVDouble>(dataFactory->createPVScalar(pvDouble));
  stField->put(st);
  fields[1] = stField;

  // end

  names[2] = "end"; 

  int es, en;
  epicsTime2pieces(interval.getEnd(), es, en); 

  double et = en;
  et /= 1000000000;
  et += es;

  PVDoublePtr etField = 
    static_pointer_cast<PVDouble>(dataFactory->createPVScalar(pvDouble));
  etField->put(et);
  fields[2] = etField;

  result = dataFactory->createPVStructure(names, fields);
  return result; 
}

// get pattern

string PVsQueryCLS::decodeQuery(const PVStructurePtr& query){

  string pattern;

  if(query.get() == 0){   
    pattern = ".*";
    return pattern;
  }

  PVStructurePtr pvPipe = query->getStructureField("pipeline");
  
  PVIntPtr sizeField = pvPipe->getIntField("size");
  int size = sizeField->get();

  for(int i = 0; i < size; i++) {
    stringstream ss; ss << i;
    PVStructurePtr elem = pvPipe->getStructureField(ss.str());
    const vector<string>& fnames = elem->getStructure()->getFieldNames();
    const PVFieldPtrArray& pvFields = elem->getPVFields();
    if(fnames[0] == "$match") {
      pattern = decodeMatchField(pvFields[0]);
      break;
    }
  }

  if(!pattern.length()){
    pattern = ".*";
  }

  return pattern;
}

string PVsQueryCLS::decodeMatchField(const PVFieldPtr& matchField){

  string pattern;

  PVStructurePtr matchDoc = static_pointer_cast<PVStructure>(matchField);

  FieldConstPtr field      = matchDoc->getStructure()->getField("pv");
  epics::pvData::Type type = field->getType();

  if(type == scalar){
    Scalar* scalarField = (Scalar*)(field.get());
    ScalarType scalarType = scalarField->getScalarType();
    if(scalarType == pvString){
      pattern = matchDoc->getStringField("pv")->get();    
    }
    return pattern;
  } else if (type == structure) {
    PVStructurePtr pvField = matchDoc->getStructureField("pv");
    PVStringPtr regexField = pvField->getStringField("$regex");
    if(regexField.get() == 0){
      return pattern;
    } else {
      return regexField->get();
    }
  }

  return pattern;
}

// index 

// IndexPtr PVsQueryCLS::createIndex(int key){

   // access the config data

//   ServerConfigEA3* config = ServerConfigEA3::getInstance();
//   IndexPtr index = config->openIndex(key);
//   return index;
// }

// Return open index for given key or 0
IndexPtr PVsQueryCLS::createIndex(const string& index_name) {

  IndexPtr index;        

#ifdef DEBUG_COMMAND
    std::cout << "Open index: " << index_name << std::endl;
#endif

    Archives* archives = Archives::getInstance();

    map<string, int>::iterator ikey = archives->keys.find(index_name);
    if(ikey == archives->keys.end()) return index;

    map<int, Archives::ArchivesEntry>::iterator ie = archives->archives.find(ikey->second);
    if(ie == archives->archives.end()) return index;

    string path = ie->second.path;
    
    LOG_MSG("Open index, path, '%s'\n", path.c_str());

  try {
     
    index.reset(new AutoIndex());
    // AutoPtr<Index> index(new AutoIndex());
    index->open(path);
    // return index.release();
    return index;

  } catch (GenericException &e) {
    // xmlrpc_env_set_fault_formatted(env, ARCH_DAT_NO_INDEX,
    // "%s", e.what());
    std::cout << "index file " << index_name << " is not defined " << std::endl;
  }

  return index;
}


}}

