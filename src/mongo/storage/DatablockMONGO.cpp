#include "storage/DatablockMONGO.h"

#include "tools/timing.h"
#include "tools/epicsTimeHelper.h"
#include "pva/ServerConfigMONGO.h"

// #define DEBUG_DATABLOCK

using namespace std;
using namespace mongo;
using namespace ea4;
using namespace ea4::pva;

namespace ea4 { namespace storage {

Interval DatablockMONGO::emptyInterval;

DatablockMONGO::DatablockMONGO() {
}

void DatablockMONGO::setArchiveName(const string& archive){  
  clear();
  archiveName = archive;
}

bool DatablockMONGO::checkPvName(const string& name){

  ServerConfigMONGO* config = ServerConfigMONGO::getInstance();

  map<string, map<string, ServerConfigMONGO::PVsEntry> >::iterator ipvs;

  // for(ipvs = config->pvs.begin(); ipvs != config->pvs.end(); ipvs++){
  //  std::cout << ipvs->first << std::endl;
  // }

  ipvs = config->pvs.find(archiveName);
  if(ipvs == config->pvs.end())  return false;

  map<string, ServerConfigMONGO::PVsEntry>::iterator it;

  // for(it = ipvs->second.begin(); it != ipvs->second.end(); it++){
  //  std::cout << it->first << std::endl;
  // }

  it = ipvs->second.find(name);
  if(it == ipvs->second.end()) return false;

  return true;
}

bool DatablockMONGO::search(const string& pv, const epicsTime *start){

  clear();

  pvName = pv;

  if(!checkPvName(pvName)) return false;

  if(!readFileDocs()) return false;
  if(!selectFid(start)) return false;
  if(!readBlockDocs(fid->second.filename)) return false;
  bool result = selectBid(start);
  return result;
}

// RTree::Datablock API

bool DatablockMONGO::isValid() const {
  return (bid != blockDocs.end());
}
           
const Interval& DatablockMONGO::getInterval() const {
  if(bid == blockDocs.end()) return emptyInterval;
  return bid->second.interval;
}

bool DatablockMONGO::getNextChainedBlock(){
  return getNextDatablock();
}

void DatablockMONGO::beginFid() {
  fid = fileDocs.begin();
  data_filename = fid->second.filename;
}

void DatablockMONGO::nextFid() {
  fid++;
  if(fid != fileDocs.end()) data_filename = fid->second.filename;
  else data_filename = "";
}

void DatablockMONGO::prevFid() {
  if(fid == fileDocs.begin()) {
    endFid();
    return;
  }
  fid--;
  data_filename = fid->second.filename;
}

void DatablockMONGO::endFid() {
  fid = fileDocs.end();
  data_filename = "";
}

void DatablockMONGO::beginBid() {
  bid = blockDocs.begin();
  data_offset = bid->second.offset;
}

void DatablockMONGO::nextBid() {
  bid++;
  if(bid != blockDocs.end()) data_offset = bid->second.offset;
  else data_offset = 0;
}

void DatablockMONGO::prevBid() {
  if(bid == blockDocs.begin()) {
    endBid();
    return;
  }
  bid--;
  data_offset = bid->second.offset;
}

void DatablockMONGO::endBid() {
  bid = blockDocs.end();
  data_offset = 0;
}
	    
bool DatablockMONGO::getNextDatablock(){
  if(bid == blockDocs.end()) return false;
  nextBid();
  if(bid != blockDocs.end()) return true;

  nextFid();
  if(fid == fileDocs.end()) return false;
  return readBlockDocs(fid->second.filename);
}

bool DatablockMONGO::getPrevDatablock(){
  if(bid == blockDocs.end()) return false;
  --bid;
  if(bid != blockDocs.end()) return true;

  prevFid();
  if(fid == fileDocs.end()) return false;
  return readBlockDocs(fid->second.filename);
}

// Private methods

bool DatablockMONGO::readFileDocs(){

  timespec ctime0, ctime1, ctime2, cdt1;
  timespec ptime0, ptime1, ptime2, pdt1; 

  clock_gettime(CLOCK_REALTIME, &ctime0);
  cdt1 = ctime0 - ctime0;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ptime0);
  pdt1 = ptime0 - ptime0;

  clock_gettime(CLOCK_REALTIME, &ctime1);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ptime1);  

  ServerConfigMONGO* config = ServerConfigMONGO::getInstance();

  string ns = string(config->getDBName()) + "." + archiveName;

  BSONObj bsonQuery = BSON("pv" << pvName);
  Query   query = Query(bsonQuery);
  BSONObj projection = BSON("file" << 1 << "start" << 1 << "end" << 1);

  try {

    // ns, Query query, int nToReturn=0, int nToSkip=0, BSONObj* fieldsToReturn=0
    auto_ptr<DBClientCursor> cursor = config->mongo.query(ns.c_str() , query, 0, 0,  &projection);
    int counter = 0;

    while ( cursor->more() ) {
      BSONObj doc = cursor->next();
      addFileDoc(doc);
    }

  } catch (DBException  &e) {
    LOG_MSG("DatablockMONGO:::readFileDocs() -> error '%s'\n",
	    (char *) e.what());
    clear();
    return false;
  }

  if(fileDocs.size() == 0) return false;

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ptime2);
  clock_gettime(CLOCK_REALTIME, &ctime2);

  pdt1 = pdt1 + (ptime2 - ptime1);
  cdt1 = cdt1 + (ctime2 - ctime1);

#ifdef DEBUG_DATABLOCK

  cout << "DatablockMONGO::readFileDocs (cpu)  : " << pdt1 << std::endl;
  cout << "DatablockMONGO::readFileDocs (clock): " << cdt1  << std::endl; 
  cout << "DatablockMONGO::readFileDocs:, filedocs: " << fileDocs.size() << std::endl;

#endif

  beginFid();

  return true;
}

void DatablockMONGO::addFileDoc(const BSONObj& obj){

  FileDoc fileDoc;

  // filename
  fileDoc.filename = obj.getStringField("file");

  // start    
  unsigned long long start_millis  = obj.getField("start").Date().millis;
  int start_sec = start_millis/1000;
  int start_nano = (start_millis - start_sec*1000)*1000000;
  epicsTime start;
  pieces2epicsTime(start_sec, start_nano, start);
  fileDoc.interval.setStart(start);

  // end
  unsigned long long end_millis  = obj.getField("end").Date().millis;
  int end_sec = end_millis/1000;
  int end_nano = (end_millis - end_sec*1000)*1000000;
  epicsTime end;
  pieces2epicsTime(end_sec, end_nano, end);
  fileDoc.interval.setEnd(end);

  fileDocs.insert(pair<epicsTime, FileDoc>(start, fileDoc));
}

bool DatablockMONGO::selectFid(const epicsTime *start){

  if(fid == fileDocs.end()) return false;

  if (!start) {
    beginFid();
    return true;
  }

  std::map<epicsTime, FileDoc>::iterator it = fileDocs.lower_bound(*start);
  if(it != fileDocs.begin()) fid = --it;
  data_filename = fid->second.filename;

  // std::string timeStr;
  // epicsTime2string(*start, timeStr);

  if((fid->first > *start) && (fid != fileDocs.begin())) prevFid();

  return true;
}

bool DatablockMONGO::readBlockDocs(const string& fileName){

  ServerConfigMONGO* config = ServerConfigMONGO::getInstance();

  string ns = string(config->getDBName()) + "." + archiveName;

#ifdef DEBUG_DATABLOCK
  std::cout << "DatablockMONGO::readBlockDocs: " << ns << ", " << pvName << ", " 
	    << fileName << std::endl;
#endif

  BSONObj bsonQuery = BSON("pv" << pvName << "file" << fileName);
  Query query = Query(bsonQuery);

  // 

  timespec ctime0, ctime1, ctime2, cdt1;
  timespec ptime0, ptime1, ptime2, pdt1; 

  clock_gettime(CLOCK_REALTIME, &ctime0);
  cdt1 = ctime0 - ctime0;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ptime0);
  pdt1 = ptime0 - ptime0;

  clock_gettime(CLOCK_REALTIME, &ctime1);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ptime1);  

  //

  try {

    BSONObj doc = config->mongo.findOne(ns.c_str() , query);

    vector<BSONElement> entries = doc.getField("entries").Array();
 
    blockDocs.clear();

    for(int i=0; i < entries.size(); i++){
      BSONObj entry = entries[i].embeddedObject();
      addBlockDoc(entry);
    } 

  } catch (DBException  &e) {
      LOG_MSG("DatablockMONGO:::readBlockDocs() -> error '%s'\n",
	    (char *) e.what());
      blockDocs.clear();
      endBid();
      return false;
  }

  // 

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ptime2);
  clock_gettime(CLOCK_REALTIME, &ctime2);

  pdt1 = pdt1 + (ptime2 - ptime1);
  cdt1 = cdt1 + (ctime2 - ctime1);

#ifdef DEBUG_DATABLOCK

  cout << "DatablockMONGO::readBlockDocs (cpu)  : " << pdt1 << std::endl;
  cout << "DatablockMONGO::readBlockDocs (clock): " << cdt1  << std::endl;
  cout << "DatablockMONGO::readBlockDocs:, blockdocs: " << blockDocs.size() << std::endl;

#endif

  if(blockDocs.size() == 0) {
    endBid();
    return false;
  }

  beginBid();

  return true;
}

void DatablockMONGO::addBlockDoc(const BSONObj& obj){

  BlockDoc blockDoc;

  blockDoc.offset = obj.getField("offset").Long();
  // bdoc.getField("offset").Int();

  int start_secs  = obj.getField("start_secs").Int();
  int start_nanos = obj.getField("start_nanos").Int();

  epicsTime startTime;
  pieces2epicsTime(start_secs, start_nanos, startTime);

  int end_secs  = obj.getField("end_secs").Int();
  int end_nanos = obj.getField("end_nanos").Int();

  epicsTime endTime;
  pieces2epicsTime(end_secs, end_nanos, endTime);
  
  blockDoc.interval.setStart(startTime);
  blockDoc.interval.setEnd(endTime);

  blockDocs.insert(pair<epicsTime, BlockDoc>(startTime, blockDoc));  
}

bool DatablockMONGO::selectBid(const epicsTime *start){

  if(bid == blockDocs.end()) return false;

  if (!start) {
    beginBid();
    return true;
  }

  std::map<epicsTime, BlockDoc>::iterator it;
  it = blockDocs.lower_bound(*start);
  if(it != blockDocs.begin()) bid = --it;
  data_offset = bid->second.offset;


  if((bid->first > *start) && (bid != blockDocs.begin())) {
    prevBid();
  } 


  return true;
}

void DatablockMONGO::clear(){
  blockDocs.clear();
  endBid();

  fileDocs.clear();
  endFid();

  pvName.clear();
}

  }}
