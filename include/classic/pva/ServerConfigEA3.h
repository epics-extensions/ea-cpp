#ifndef EA4_SERVER_CONFIG_EA3_H
#define EA4_SERVER_CONFIG_EA3_H

#include <list>
#include <tr1/memory>

// tools
#include "tools/ToolsConfig.h"

// storage
#include "storage/AutoIndex.h"

typedef std::tr1::shared_ptr<Index> IndexPtr;

namespace ea4 { namespace pva {

/** The RPC Server Configuration.
 * Reads the config from an XML file
 * that should conform to serverconfig.dtd.
 */
class ServerConfigEA3 {

 public:

  /** Returns singleton */
  static ServerConfigEA3* getInstance();

 public:

   /** Sets the name of the XML file */
   inline void setConfigName(const char* name);

   /** Returns the name of the XML file defined by 
    * the env variable 'SERVERCONFIG' */
   inline const char* getConfigName();

   /** Reads the XML file that matches serverconfig.dtd */
   bool read();

   /** Finds path for key or returns false. */
   bool find(int key, std::string& path); 

   /** Returns the selected Index file */
   IndexPtr openIndex(int key);
    
   /** Print this configuration */
   void dump();

 public:

   /** version */
   // std::string verTxt;

   /** version */
   // int ver;

 private:

   std::string filename;

   static ServerConfigEA3* theInstance;
   

};

inline void ServerConfigEA3::setConfigName(const char* n){
  filename = n;
}

inline const char* ServerConfigEA3::getConfigName(){
  return filename.c_str();
}


  }}

#endif


