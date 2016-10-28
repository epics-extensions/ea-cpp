#ifndef EA4_SERVER_CONFIG_MONGO_H
#define EA4_SERVER_CONFIG_MONGO_H

#include <list>

#include "mongo/client/dbclient.h"

// tools
#include "tools/ToolsConfig.h"


namespace ea4 { namespace pva {

/** The RPC Server Configuration.
 * Reads the config collections from MongoDB 
 */
class ServerConfigMONGO {

 public:

  /** returns singleton */
  static ServerConfigMONGO* getInstance();

  /** mongo connection */
  mongo::DBClientConnection mongo;

 public:

   /** sets the name of the mongodb database (like ea4::nsls2) */
   inline void setDBName(const char* name);

   /** returns the name of the mongodb database */
   inline const char* getDBName();

   /** readArchives, readPVs */
   void init();

 public:

   /** pv entry */
   struct PVsEntry {

     std::string name; // pv name

     int start_sec;
     int start_nano;
     int end_sec;
     int end_nano;
   };

   /** archive-pvs */
   std::map<std::string, std::map<std::string, PVsEntry> > pvs;

 protected:

   /** reads records of dbname.archives collection and initializes 
       the Archives singleton */
   bool readArchives();

   /** reads records of the pvs collection */
   bool readPVs();

 private:

   // db name, e.g. ea4::nsls2
   std::string dbname;

 private:

   static ServerConfigMONGO* theInstance;
   
};

inline void ServerConfigMONGO::setDBName(const char* name){
  dbname = name;
}

inline const char* ServerConfigMONGO::getDBName(){
  return dbname.c_str();
}


}}

#endif


