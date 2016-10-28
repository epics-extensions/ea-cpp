#include "pva/CollectActionCLS.h"

#include "pva/ServerConfigEA3.h"
#include "pva/Archives.h"
#include "pva/Status.h"
#include "pva/InfoQueryCLS.h"
#include "pva/ArchivesQueryCLS.h"
#include "pva/PVsQueryCLS.h"
#include "pva/ReadQueryCLS.h"

#include <algorithm>

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;

#define MAX_COUNT 1000000
// #define DEBUG_COMMAND
#define BENCHMARK_COMMAND

namespace ea4 { namespace pva {

CollectActionCLS::CollectActionCLS(){
}

PVStructurePtr CollectActionCLS::apply(Action::Request& request){

  PVStructurePtr nullPtr;
  PVStructurePtr result;

  // info

  if(request.query.src.id == "info") {
    InfoQueryCLS infoQuery;
    result = infoQuery.apply(request.query.src.id, nullPtr, nullPtr);
    return result;
  }

  // archives

  if(request.query.src.id == "archives") {
    ArchivesQueryCLS archivesQuery;
    result = archivesQuery.apply(request.query.src.id, nullPtr, nullPtr);
    return result;
  }

  // process query

  result = createResult(request);
  return result;
}

PVStructurePtr 
CollectActionCLS::createResult(Action::Request& request){

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
  replace(id.begin(), id.end(), '_', '/');

  // GetChannels: no query

  if(queries.size() == 0) {
    PVStructurePtr nullPtr;
    result = filter(id, nullPtr);
    return result;
  }

  // GetChannels: with query

  list<PVStructurePtr>::iterator it = queries.begin();
  PVStructurePtr& filterQuery = *it;

  vector<string> names = filterQuery->getStructure()->getFieldNames();
  string filterName = filterQuery->getStringField("command")->get();

  if(filterName.compare("filter"))  {
 
    PVStructurePtr pvStatus = Status::createPVStructure();
    PVIntPtr typeField = pvStatus->getIntField("type");
    typeField->put(2); // error

    PVStringPtr messageField = pvStatus->getStringField("message");
    messageField->put("CollectActionCLS::apply: the first query is not a filter\n");

    result = createEmptyResult(pvStatus);
    return result;
  }

  PVStructurePtr filterResult = filter(id, filterQuery);

  it++;
  if(it == queries.end()) return filterResult;

  // GetValues

  PVStructurePtr& readQuery = *it;
  string readName = readQuery->getStringField("command")->get();

  if(readName.compare("read"))  {
    PVStructurePtr pvStatus = Status::createPVStructure();
    PVIntPtr typeField = pvStatus->getIntField("type");
    typeField->put(2); // error

    PVStringPtr messageField = pvStatus->getStringField("message");
    messageField->put("CollectActionCLS::apply: the second query is not a read\n");

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
CollectActionCLS::createEmptyResult(PVStructurePtr& pvStatus){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(1);
  PVFieldPtrArray fields(1);

  names[0]  = "status";
  fields[0] = pvStatus;

  PVStructurePtr result = dataFactory->createPVStructure(names, fields);
  return result;
}

PVStructurePtr 
CollectActionCLS::filter(const string& id, PVStructurePtr& query){

    PVsQueryCLS filterQuery;

    PVStructurePtr nullPtr;
    PVStructurePtr result;

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
CollectActionCLS::read(const string& id, 
		       PVStructurePtr& filterResult, 
		       PVStructurePtr& query){

  PVStructurePtr nullPtr;
  PVStructurePtr result;

  ReadQueryCLS readQuery;
  PVStructurePtr args = query->getStructureField("args");
  result = readQuery.apply(id, filterResult, args);

#ifdef DEBUG_COMMAND
    string builder;
    result->toString(&builder);
    std::cout << "read: " << builder << std::endl;
#endif

   return result;
}

}}
