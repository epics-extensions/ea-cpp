#include "pva/ReadQueryCLS.h"
#include "pva/Archives.h"
#include "pva/Status.h"

#include "pva/ServerConfigEA3.h"
#include "storage/ReaderFactoryEA3.h"
#include "pva/DbrBuilderCLS.h"

#include <algorithm>

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;

#define LOGFILE
// #define DEBUG_COMMAND
#define BENCHMARK_COMMAND

#define MAX_COUNT 1000000

using namespace std;
using namespace ea4::storage;

namespace ea4 { namespace pva {

ReadQueryCLS::ReadQueryCLS(){
}

PVStructurePtr ReadQueryCLS::apply(const std::string& id, 
				   const PVStructurePtr& pvInput,
				   const PVStructurePtr& pvQuery){

#ifdef LOGFILE
  LOG_MSG("ReadQueryCLS::apply\n");
#endif

  PVStructurePtr result;
  PVStructurePtr pvStatus = Status::createPVStructure();

  // configName

  ServerConfigEA3* config = ServerConfigEA3::getInstance();
  const char* configName  = config->getConfigName();

  if (!configName) {

    PVIntPtr typeField = pvStatus->getIntField("type");
    typeField->put(3); // fatal

    PVStringPtr messageField = pvStatus->getStringField("message");
    messageField->put("ReadQueryCLS::apply: configName is not defined\n");

    result = createEmptyResult(pvStatus);
    return result;
  }

  LOG_MSG("config: '%s'\n", configName);

 
  // input

  Input input;
  decodeInput(pvInput, input);
  if(!input.names.size()){
    PVIntPtr typeField = pvStatus->getIntField("type");
    typeField->put(1); // Warning

    PVStringPtr messageField = pvStatus->getStringField("message");
    messageField->put("ReadQueryCLS::apply: input (from filter) is empty \n");

    result = createEmptyResult(pvStatus);
    return result;
  }

  // query

  Request query;
  decodeQuery(pvQuery, query);

#ifdef BENCHMARK_COMMAND
    
  timespec ctime0, ctime1, ctime2, cdt1;
  timespec ptime0, ptime1, ptime2, pdt1; 

  clock_gettime(CLOCK_REALTIME, &ctime0);
  cdt1 = ctime0 - ctime0;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ptime0);
  pdt1 = ptime0 - ptime0;

  clock_gettime(CLOCK_REALTIME, &ctime1);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ptime1);

#endif

  result = createResult(pvStatus, id, input, query);

#ifdef BENCHMARK_COMMAND

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ptime2);
  clock_gettime(CLOCK_REALTIME, &ctime2);

  pdt1 = pdt1 + (ptime2 - ptime1);
  cdt1 = cdt1 + (ctime2 - ctime1);

  cout << "creating response phase (cpu)  : " << pdt1 << std::endl;
  cout << "creating response phase (clock): " << cdt1  << std::endl;

#endif

  return result;

}

PVStructurePtr ReadQueryCLS::createResult(PVStructurePtr& pvStatus, 
					  const string& id,
					  const Input& input,
					  const Request& query){

  PVStructurePtr result;
  PVDataCreatePtr dataFactory = getPVDataCreate();

  PVStructurePtr pvDocs = createDocs(pvStatus, id, input, query); 
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


PVStructurePtr ReadQueryCLS::createDocs(PVStructurePtr& pvStatus, 
					const string& id, 
					const Input& input,
					const Request& query){

  PVStructurePtr result;
  PVDataCreatePtr dataFactory = getPVDataCreate();

  // allocate containers

  int size = input.names.size();

  StringArray names(size + 1);
  PVFieldPtrArray fields(size + 1);

  PVIntPtr sizeField = 
    static_pointer_cast<PVInt>(dataFactory->createPVScalar(pvInt));
  sizeField->put(size);

  names[0] = "size";
  fields[0] = sizeField; 

#ifdef LOGFILE
   string txt;
   LOG_MSG("ReadQueryCLS::apply:\n");
   if(query.start.get()){
     LOG_MSG("Start:  %s\n", epicsTimeTxt(*(query.start.get()), txt));
   } else {
     LOG_MSG("Start:  not defined\n");
   }
   if(query.end.get()){
     LOG_MSG("End  :  %s\n", epicsTimeTxt(*(query.end.get()), txt));
   } else {
     LOG_MSG("End  :  not defined\n");
   }
#endif

 // read data

  try {

    for(int i= 0; i < input.names.size(); i++){

      stringstream ss; ss << i;
      names[i+1] = ss.str().c_str();

      PVStructurePtr doc = createDoc(pvStatus, id, input.names[i], query);
      if(doc.get() == 0) return result;

      fields[i+1] = doc;
    }

  } catch (GenericException &e) {

#ifdef LOGFILE
      LOG_MSG("Error:\n%s\n", e.what());
#endif
      PVIntPtr typeField = pvStatus->getIntField("type");
      typeField->put(2); // error

      PVStringPtr messageField = pvStatus->getStringField("message");
      messageField->put(e.what());

      result = createEmptyResult(pvStatus);
      return result;
  }

  // compose a response  

  result = dataFactory->createPVStructure(names, fields);

#ifdef DEBUG_COMMAND
  std::cout << "size: " << fields.size() << std::endl;
#endif

  return result;
}

PVStructurePtr ReadQueryCLS::createDoc(PVStructurePtr& pvStatus, 
				       const string& id,
				       const string& pvName,
				       const Request& query){

  PVStructurePtr result;
  PVDataCreatePtr dataFactory = getPVDataCreate();

  DbrBuilderCLS* builder = 0;

  try {

    IndexPtr indexPtr = createIndex(id);
    if(indexPtr.get() == 0) return result;

    DataReaderPtr readerPtr = createDataReader(indexPtr);
    if(readerPtr.get() == 0) return result;

    builder = new DbrBuilderCLS(readerPtr, pvName, 
				query.start, query.end, query.limit);

    builder->findFirstEntry();

    StructureConstPtr resultType = builder->getResultType();
    result = dataFactory->createPVStructure(resultType);
    int nsamples = builder->getResult(result);

#ifdef DEBUG_COMMAND
    cout << "ReadQueryCLS::createDoc, entry: " << pvName
	 << ", samples: " << nsamples << endl;
#endif

    indexPtr->close();

  }catch (GenericException &e) {

#ifdef LOGFILE
      LOG_MSG("Error:\n%s\n", e.what());
#endif
      if(builder) delete builder;

      PVIntPtr typeField = pvStatus->getIntField("type");
      typeField->put(2); // error

      PVStringPtr messageField = pvStatus->getStringField("message");
      messageField->put(e.what());

      result = createEmptyResult(pvStatus);
      return result;
  }

  if(builder) delete builder;
  return result;
}


// IndexPtr 
// ReadQueryCLS::createIndex(int key){

   // access the config data

//   ServerConfigEA3* config = ServerConfigEA3::getInstance();
//   IndexPtr index = config->openIndex(key);
//   return index;
// }

// Return open index for given key or 0
IndexPtr ReadQueryCLS::createIndex(const string& index_name) {

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

DataReaderPtr 
ReadQueryCLS::createDataReader(IndexPtr index){

  ReaderFactoryEA3::How rfHow = ReaderFactoryEA3::Raw;
  double delta = 0.0;
       
  DataReaderPtr dataReader(ReaderFactoryEA3::create(index, rfHow, delta));
  return dataReader;
}


}}

