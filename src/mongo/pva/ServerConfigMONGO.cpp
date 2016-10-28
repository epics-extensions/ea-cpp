// system
#include <stdio.h>
#include <iostream>

// tools
#include "tools/MsgLogger.h"
#include "tools/FUX.h"
#include "tools/AutoPtr.h"

// storage
#include "storage/AutoIndex.h"
#include "storage/Interval.h"
#include "storage/RawValue.h"

// pvaccess
#include "pva/Archives.h"
#include "pva/ServerConfigMONGO.h"

// #include "storage/RawDataReaderMONGO.h"

using namespace std;
using namespace mongo;

namespace ea4 { namespace pva {

ServerConfigMONGO* ServerConfigMONGO::theInstance = 0;

ServerConfigMONGO*  ServerConfigMONGO::getInstance() {
  if(theInstance == 0) {
    theInstance = new ServerConfigMONGO();
  }
  return theInstance;
}

void ServerConfigMONGO::init(){

  readArchives();
  readPVs();

  // RawDataReaderMONGO rawReader;
  // rawReader.setArchiveName("RadMon_2013");

  // try {
  //  const RawValue::Data* data = rawReader.find("BR-AM{RadMon:11}DoseRate-I", 0);
  //  if(data) std::cout << "data is not null" << std::endl;
  // } catch (GenericException& e) {
  // std::cout << e.what() << std::endl;
  // exit(0);
  // }

  // DatablockMONGO datablock("RadMon_2013");
  // datablock.search("BR-AM{RadMon:11}DoseRate-I", 0);
  // datablock.getNextDatablock();
}

bool ServerConfigMONGO::readArchives() {

  string ns = string(dbname) + ".archives";

  int count = mongo.count(ns.c_str());

  Archives* archives = Archives::getInstance();
  archives->archives.clear();

  //  LOG_MSG("Read archives: %s, %d\n", ns.c_str(), count);

  {
    auto_ptr<DBClientCursor> cursor = 
      mongo.query(ns.c_str() , BSONObj() );
    while ( cursor->more() ) {

      Archives::ArchivesEntry entry;

      BSONObj doc = cursor->next();
      entry.key  = doc.getIntField("key");
      entry.name = doc.getStringField("name");
      entry.path = doc.getStringField("path");

      archives->archives[entry.key] = entry;
      archives->keys[entry.name] = entry.key;
    }

  }

  return true;
}

bool ServerConfigMONGO::readPVs() {

  string ns = string(dbname) + ".pvs";

  int count = mongo.count(ns.c_str());

  //  LOG_MSG("Read pvs: %s, %d\n", ns.c_str(), count);

  {
    auto_ptr<mongo::DBClientCursor> cursor = 
      mongo.query(ns.c_str() , mongo::BSONObj() );
    while ( cursor->more() ) {

            PVsEntry entry;
	    BSONObj doc = cursor->next();

	    string archive = doc.getStringField("archive");
	    entry.name = doc.getStringField("pv");

	    unsigned long long start  = doc.getField("start").Date().millis;
	    entry.start_sec = start/1000;
	    entry.start_nano = (start - entry.start_sec*1000)*1000000;

	    unsigned long long end    = doc.getField("end").Date().millis;
	    entry.end_sec = end/1000;
	    entry.end_nano = (end - entry.end_sec*1000)*1000000;
	    pvs[archive][entry.name] = entry;
    }

  }

  return true;
}

}}


