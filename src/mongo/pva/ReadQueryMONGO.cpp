#include "mongo/client/dbclient.h"

#include "pva/ReadQueryMONGO.h"
#include "pva/Archives.h"
#include "pva/Status.h"

#include "pva/ServerConfigMONGO.h"
#include "storage/ReaderFactoryMONGO.h"
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

ReadQueryMONGO::ReadQueryMONGO(){
}

PVStructurePtr ReadQueryMONGO::apply(const std::string& id, 
				   const PVStructurePtr& pvInput,
				   const PVStructurePtr& pvQuery){

#ifdef LOGFILE
  LOG_MSG("ReadQueryMONGO::apply\n");
#endif

  PVStructurePtr result;
  PVStructurePtr pvStatus = Status::createPVStructure();

  // configName

  ServerConfigMONGO* config = ServerConfigMONGO::getInstance();
  const char* dbName  = config->getDBName();

  if (!dbName) {

    PVIntPtr typeField = pvStatus->getIntField("type");
    typeField->put(3); // fatal

    PVStringPtr messageField = pvStatus->getStringField("message");
    messageField->put("ReadQueryMONGO::apply: dbName is not defined\n");

    result = createEmptyResult(pvStatus);
    return result;
  }

  LOG_MSG("db name: '%s'\n", dbName);

 
  // input

  Input input;
  decodeInput(pvInput, input);
  if(!input.names.size()){
    PVIntPtr typeField = pvStatus->getIntField("type");
    typeField->put(1); // Warning

    PVStringPtr messageField = pvStatus->getStringField("message");
    messageField->put("ReadQueryMONGO::apply: input (from filter) is empty \n");

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

PVStructurePtr ReadQueryMONGO::createResult(PVStructurePtr& pvStatus, 
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


PVStructurePtr ReadQueryMONGO::createDocs(PVStructurePtr& pvStatus, 
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
   LOG_MSG("ReadQueryMONGO::apply:\n");
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

PVStructurePtr ReadQueryMONGO::createDoc(PVStructurePtr& pvStatus, 
				       const string& id,
				       const string& pvName,
				       const Request& query){

  PVStructurePtr result;
  PVDataCreatePtr dataFactory = getPVDataCreate();

  DbrBuilderCLS* builder = 0;

  try {

    DataReaderPtr readerPtr = createDataReader(id);
    if(readerPtr.get() == 0) return result;

    builder = new DbrBuilderCLS(readerPtr, pvName, 
				query.start, query.end, query.limit);

    builder->findFirstEntry();

    StructureConstPtr resultType = builder->getResultType();
    result = dataFactory->createPVStructure(resultType);

    int nsamples = builder->getResult(result);

#ifdef DEBUG_COMMAND
    cout << "ReadQueryMONGO::createDoc, entry: " << pvName
	 << ", samples: " << nsamples << endl;
#endif

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

DataReaderPtr 
ReadQueryMONGO::createDataReader(const string& id){

  ReaderFactoryMONGO::How rfHow = ReaderFactoryMONGO::Raw;
  double delta = 0.0;
       
  DataReaderPtr dataReader(ReaderFactoryMONGO::create(id, rfHow, delta));
  return dataReader;
}


}}

