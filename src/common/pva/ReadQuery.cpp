#include "pva/ReadQuery.h"

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;

// #define DEBUG_COMMAND

namespace ea4 { namespace pva {

PVStructurePtr ReadQuery::createEmptyResult(PVStructurePtr& pvStatus){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(1);
  PVFieldPtrArray fields(1);

  names[0]  = "status";
  fields[0] = pvStatus;

  PVStructurePtr result = dataFactory->createPVStructure(names, fields);
  return result;
}

// get channels
void ReadQuery::decodeInput(const PVStructurePtr& pvInput, Input& input){

#ifdef DEBUG_COMMAND
    string builder;
    pvInput->toString(&builder);
    std::cout << "ReadQuery::decodeInput, input : " << builder << std::endl;
#endif

  PVStructurePtr statusField = pvInput->getStructureField("status");
  int statusType = statusField->getIntField("type")->get();
  if(statusType) return;

  PVStructurePtr docsField = pvInput->getStructureField("docs");
  int size = docsField->getIntField("size")->get();
  if(!size) return;

  set<string> names;

  for(int i=0; i < size; i++){
    stringstream ss; ss << i;
    PVStructurePtr docField = docsField->getStructureField(ss.str());
    names.insert(docField->getStringField("pv")->get());
  }

  input.names.resize(names.size());

  int counter = 0;
  set<string>::iterator it;
  for(it = names.begin(); it != names.end(); it++){
    input.names[counter++] = *it;
  }

  return;
}

// get start, end, limit
void ReadQuery::decodeQuery(const PVStructurePtr& query, Request& request){

#ifdef DEBUG_COMMAND
    string builder;
    query->toString(&builder);
    std::cout << "ReadQuery::decodeQuery, query : " << builder << std::endl;
#endif

  request.limit = MAX_COUNT;

  if(query.get() == 0){   
    return;
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
      decodeMatchField(pvFields[0], request);
      break;
    }
  }

  return;
}

void ReadQuery::decodeMatchField(const PVFieldPtr& matchField, 
				 Request& request){
  int64_t tmp;

  PVStructurePtr matchDoc = static_pointer_cast<PVStructure>(matchField);

  PVLongPtr startField = matchDoc->getLongField("start");
  if(startField.get()) {

    uint64_t start = startField->get(); // usec

    int start_sec = start/1000000;
    tmp = start_sec;
    tmp *= -1000000;
    tmp += start;
    int start_nano = tmp*1000;
    epicsTime epicsStart;
    pieces2epicsTime(start_sec, start_nano, epicsStart);
    request.start.reset(new epicsTime(epicsStart));
  }

  PVLongPtr endField = matchDoc->getLongField("end");     
  if(endField.get()) {

    uint64_t end = endField->get(); // usec

    int end_sec = end/1000000;
    tmp = end_sec;
    tmp *= -1000000;
    tmp += end;
    int end_nano = tmp*1000;
    epicsTime epicsEnd;
    pieces2epicsTime(end_sec, end_nano, epicsEnd); 
    request.end.reset(new epicsTime(epicsEnd));
  }

#ifdef DEBUG_COMMAND 
    if(request.start.get()){
      string strStart;
      epicsTime2string(*(request.start.get()), strStart);
      cout << "start: " << strStart << endl;
    } else {
      cout << "start: not defined" << endl;
    }

    if(request.end.get()){
     string strEnd;
     epicsTime2string(*(request.end.get()), strEnd);
     cout << "end: " << strEnd << endl;
    } else {
       cout << "end: not defined" << endl;
    }     
#endif

  PVDoublePtr limitField = matchDoc->getDoubleField("limit");     
  if(limitField.get()) request.limit = (int) limitField->get();

  return;
}

}}
