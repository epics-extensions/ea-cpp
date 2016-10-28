// system
#include <stdio.h>
#include <iostream>

// tools
#include "tools/MsgLogger.h"
#include "tools/FUX.h"
#include "tools/AutoPtr.h"

// storage
#include "storage/AutoIndex.h"

// pvaccess
#include "pva/Archives.h"
#include "pva/ServerConfigEA3.h"

using namespace std;
using namespace std::tr1;

// #define DEBUG_COMMAND

namespace ea4 { namespace pva {

ServerConfigEA3* ServerConfigEA3::theInstance = 0;

ServerConfigEA3*  ServerConfigEA3::getInstance() {
  if(theInstance == 0) {
    theInstance = new ServerConfigEA3();
  }
  return theInstance;
}


bool ServerConfigEA3::read() {

// We depend on the XML parser to validate.
// The asserts are only the ultimate way out.
    
    FUX fux;
    FUX::Element *arch, *doc = fux.parse(filename.c_str());

    if (!doc){
      LOG_MSG("ServerConfig: Cannot parse '%s'\n", filename.c_str());
        return false;
    }

    LOG_ASSERT(doc->getName() == "serverconfig");

    list<FUX::Element *>::const_iterator archs, e;

    Archives* archives = Archives::getInstance();
    archives->archives.clear();

    for ( archs=doc->getChildren().begin(); 
	  archs!=doc->getChildren().end(); ++archs) {

      Archives::ArchivesEntry entry;
      entry.type = "ea3";

        arch = *archs;
        LOG_ASSERT(arch->getName() == "archive");

        e = arch->getChildren().begin();
        LOG_ASSERT((*e)->getName() == "key");
        entry.key = atoi((*e)->getValue().c_str());

        ++e;
        LOG_ASSERT((*e)->getName() == "name");
        entry.name = (*e)->getValue();

        ++e;
        LOG_ASSERT((*e)->getName() == "path");
        entry.path = (*e)->getValue();

        archives->archives[entry.key] = entry;
	archives->keys[entry.name] = entry.key;
    }

    return true;
}

void ServerConfigEA3::dump() {

   Archives* archives = Archives::getInstance();

   map<int, Archives::ArchivesEntry>::const_iterator it;

   for ( it = archives->archives.begin(); it != archives->archives.end(); ++it){
     printf("key %d: '%s' in '%s'\n",
	   it->second.key, 
	   it->second.name.c_str(), 
	   it->second.path.c_str());
   }
}

bool ServerConfigEA3::find(int key, string& path) {

  Archives* archives = Archives::getInstance();

  map<int, Archives::ArchivesEntry>::const_iterator it;
  it = archives->archives.find(key);

  if(it == archives->archives.end()){
    return false;
  }

  path = it->second.path;
  return true;
}

// Return open index for given key or 0
IndexPtr ServerConfigEA3::openIndex(int key) {

  IndexPtr index;
        
    string index_name;
        
    if (!find(key, index_name)) {
      // xmlrpc_env_set_fault_formatted(env, ARCH_DAT_NO_INDEX,
      // "Invalid key %d", key);
      return index;
    } 

#ifdef DEBUG_COMMAND
    std::cout << "Open index: " << key << ", " << index_name << std::endl;
#endif

    LOG_MSG("Open index, key %d = '%s'\n", key, index_name.c_str());

  try {
     
    index.reset(new AutoIndex());
    // AutoPtr<Index> index(new AutoIndex());
    index->open(index_name);
    // return index.release();
    return index;

  } catch (GenericException &e) {
    // xmlrpc_env_set_fault_formatted(env, ARCH_DAT_NO_INDEX,
    // "%s", e.what());
  }
  
  return index;
}

}}


