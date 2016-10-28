#include "mongo/client/dbclient.h"
#include "pva/CollectActionMONGO.h"
#include "pva/Status.h"
/*
#include "pva/ArchiveRPCServiceRegistry.h"
#include "pva/ServerConfigEA3.h"
#include "pva/Archives.h"
#include "pva/GetChannelsCommandMONGO.h"
#include "pva/GetValuesCommand2MONGO.h"
*/
#include "pva/InfoQueryMONGO.h"
#include "pva/FilterQueryMONGO.h"
#include "pva/ReadQueryMONGO.h"

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;

#define MAX_COUNT 1000000
// #define DEBUG_COMMAND
#define BENCHMARK_COMMAND

namespace ea4 { namespace pva {

CollectActionMONGO::CollectActionMONGO(){
}

PVStructurePtr CollectActionMONGO::apply(Action::Request& request){

#ifdef DEBUG_COMMAND
  std::cout << "CollectActionMONGO::apply: " << std::endl;
#endif

  PVStructurePtr nullPtr;
  PVStructurePtr result;

  // info

  if(request.query.src.id == "info") {
    InfoQueryMONGO infoQuery;
    result = infoQuery.apply(request.query.src.id, nullPtr, nullPtr);
    return result;
  }


  // process query

  result = createResult(request);
  return result;
}

PVStructurePtr 
CollectActionMONGO::createResult(Action::Request& request){

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

  PVStructurePtr result;
  list<PVStructurePtr>& queries = request.query.queries;

  // replace MongoDB collection with the file path

  string id = request.query.src.id;

  // filter: no query

  if(queries.size() == 0) {
    PVStructurePtr nullPtr;
    result = filter(id, nullPtr);
    return result;
  }

  // filter: with query

  list<PVStructurePtr>::iterator it = queries.begin();
  PVStructurePtr& filterQuery = *it;

  vector<string> names = filterQuery->getStructure()->getFieldNames();
  string filterName = filterQuery->getStringField("command")->get();

  if(filterName.compare("filter"))  {
 
    PVStructurePtr pvStatus = Status::createPVStructure();
    PVIntPtr typeField = pvStatus->getIntField("type");
    typeField->put(2); // error

    PVStringPtr messageField = pvStatus->getStringField("message");
    messageField->put("CollectActionMONGO::apply: the first query is not a filter\n");

    result = createEmptyResult(pvStatus);
    return result;
  }

  PVStructurePtr filterResult = filter(id, filterQuery);

  it++;
  if(it == queries.end()) return filterResult;

  // read: with input from filter and query

  PVStructurePtr& readQuery = *it;
  string readName = readQuery->getStringField("command")->get();

  if(readName.compare("read"))  {
    PVStructurePtr pvStatus = Status::createPVStructure();
    PVIntPtr typeField = pvStatus->getIntField("type");
    typeField->put(2); // error

    PVStringPtr messageField = pvStatus->getStringField("message");
    messageField->put("CollectActionMONGO::apply: the second query is not a read\n");

    result = createEmptyResult(pvStatus);
    return result;
  }

  result = read(id, filterResult, readQuery);

#ifdef BENCHMARK_COMMAND

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ptime2);
  clock_gettime(CLOCK_REALTIME, &ctime2);

  pdt1 = pdt1 + (ptime2 - ptime1);
  cdt1 = cdt1 + (ctime2 - ctime1);

  cout << "process query (cpu)  : " << pdt1 << std::endl;
  cout << "process query (clock): " << cdt1  << std::endl;

#endif

  return result;
}

PVStructurePtr 
CollectActionMONGO::createEmptyResult(PVStructurePtr& pvStatus){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(1);
  PVFieldPtrArray fields(1);

  names[0]  = "status";
  fields[0] = pvStatus;

  PVStructurePtr result = dataFactory->createPVStructure(names, fields);
  return result;
}

    /*

PVStructurePtr 
CollectActionMONGO::processQueries(Action::Request& request, int key){

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

  PVStructurePtr nullPtr;
  PVStructurePtr result;

  ArchiveRPCServiceRegistry* sr = ArchiveRPCServiceRegistry::getInstance();
  ArchiveRPCServicePtr service = sr->getRPCService();

  // no query, just return all MongoDB docs

   if(!request.query.queries.size()){
    result = filter(request.query.src, nullPtr);
    return result;
  }

   PVStructurePtr query = request.query.queries.front();
   string command = query->getStringField("command")->get();
   std::cout << "command: " << command << std::endl;

   result = filter(request.query.src, query);

#ifdef BENCHMARK_COMMAND

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ptime2);
  clock_gettime(CLOCK_REALTIME, &ctime2);

  pdt1 = pdt1 + (ptime2 - ptime1);
  cdt1 = cdt1 + (ctime2 - ctime1);

  cout << "process query (cpu)  : " << pdt1 << std::endl;
  cout << "process query (clock): " << cdt1  << std::endl;

#endif

  return result;
}

    */

PVStructurePtr 
CollectActionMONGO::filter(const string& id, PVStructurePtr& query){

    FilterQueryMONGO filterQuery;

    PVStructurePtr nullPtr;
    PVStructurePtr result;

    /*
    if(query.get()) {
      string qBuilder;
      query->toString(&qBuilder);
      std::cout << "filter query : " << qBuilder << std::endl;
    }
    */

    if(query.get()){
      PVStructurePtr args = query->getStructureField("args");
      result = filterQuery.apply(id, nullPtr, args);
    } else {
      result = filterQuery.apply(id, nullPtr, nullPtr);
    }

#ifdef DEBUG_COMMAND
    string builder;
    result->toString(&builder);
    std::cout << "filter: " << builder << std::endl;
#endif

    return result;
}

PVStructurePtr 
CollectActionMONGO::read(const string& id, 
		       PVStructurePtr& filterResult, 
		       PVStructurePtr& query){

  PVStructurePtr nullPtr;
  PVStructurePtr result;

  ReadQueryMONGO readQuery;
  PVStructurePtr args = query->getStructureField("args");

  /*
  if(args.get()) {
      string qBuilder;
      query->toString(&qBuilder);
      std::cout << "read args : " << qBuilder << std::endl;
  }
  */

  result = readQuery.apply(id, filterResult, args);

#ifdef DEBUG_COMMAND
    string builder;
    result->toString(&builder);
    std::cout << "read: " << builder << std::endl;
#endif

   return result;
}

    /*

PVStructurePtr 
CollectActionMONGO::processQuery(Action::Request& request, int key){

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

  PVStructurePtr result;

  ArchiveRPCServiceRegistry* sr = ArchiveRPCServiceRegistry::getInstance();
  ArchiveRPCServicePtr service = sr->getRPCService();

  string pattern;

  // no query

  if(!request.query.queries.size()) {
    return getChannels(key, pattern);
  }

  // process query

  list<PVStructurePtr>& queries = request.query.queries;
  list<PVStructurePtr>::iterator it;

  // check $match 

  for(it = queries.begin(); it != queries.end(); it++){

    PVStructurePtr& query = *it;
    
    StructureConstPtr queryType = query->getStructure();
    const StringArray& names = queryType->getFieldNames();
    if(names[0] != "$match") break;

    PVStructurePtr matchField = query->getStructureField("$match");
    PVStringPtr nameField = matchField->getStringField("name");
    if(nameField.get()) pattern = nameField->get();
  } 

  PVStructurePtr channels = getChannels(key, pattern);

#ifdef DEBUG_COMMAND
  // print channels
  if(channels.get()){
      string builder(" ");
      channels->toString(&builder);
      std::cout << "channels: " << builder << std::endl;
    }
#endif

  // check $read

  bool readFlag = false;
  int64_t start = -1;
  int64_t end = -1;
  int limit = MAX_COUNT;

  for(; it != queries.end(); it++){

    PVStructurePtr& query = *it;

    StructureConstPtr queryType = query->getStructure();
    const StringArray& names = queryType->getFieldNames();
    if(names[0] != "$read") break;

    PVStructurePtr readField = query->getStructureField("$read");

    readFlag = true;
    PVLongPtr startField = readField->getLongField("start");
    if(startField.get()) start = startField->get();
    PVLongPtr endField = readField->getLongField("end");     
    if(endField.get()) end = endField->get();
    PVDoublePtr limitField = readField->getDoubleField("limit");     
    if(limitField.get()) limit = (int) limitField->get();
  }

  if(!readFlag) return channels;

// #ifdef DEBUG_COMMAND
  std::cout << "limit: " << limit << std::endl;
// #endif

  PVStructurePtr values = getValues(key, channels, start, end, limit);

#ifdef BENCHMARK_COMMAND

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ptime2);
  clock_gettime(CLOCK_REALTIME, &ctime2);

  pdt1 = pdt1 + (ptime2 - ptime1);
  cdt1 = cdt1 + (ctime2 - ctime1);

  cout << "process query (cpu)  : " << pdt1 << std::endl;
  cout << "process query (clock): " << cdt1  << std::endl;

#endif

  return values;
}

*/

/*
PVStructurePtr 
CollectActionMONGO::getValues(int key, PVStructurePtr channels, 
			      int64_t start, int64_t end, 
			      int limit){

  ArchiveRPCServiceRegistry* sr = ArchiveRPCServiceRegistry::getInstance();
  ArchiveRPCServicePtr service = sr->getRPCService();

  ArchiveCommand* command = service->getRPCCommand("getValues2");

  shared_ptr<GetValuesCommand2MONGO> rpcCommand = 
    dynamic_pointer_cast<GetValuesCommand2MONGO>(command->createCommand());

  // prepare request 

  GetValuesCommand2::Request request;
  request.key   = key;
  request.start = start;
  request.end   = end;
  request.count = limit;
  request.how = ArchiveCommand::HOW_RAW;

  // extract names from input

  vector<GetChannelsCommand::Result> pvs;

  PVStructureArrayPtr pvsField = channels->getStructureArrayField("pvs") ;  

  GetChannelsCommand::setCStructureArray(pvsField, pvs);

  request.names.resize(pvs.size());

  for(int i=0; i < pvs.size(); i++){
    request.names[i] = pvs[i].name;
  }

  PVStructurePtr pvRequest = rpcCommand->createRequest();
  GetValuesCommand2::setPVStructure(request, pvRequest);

  PVStructurePtr result = rpcCommand->process(pvRequest);

  return result;
}

*/

}}
