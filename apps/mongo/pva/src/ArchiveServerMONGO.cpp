
#include "alarm.h"

#include "VersionEA3.h"
#include "tools/ArgParser.h"
#include "tools/MsgLogger.h"
#include "pva/ServerConfigMONGO.h"
#include "pva/ArchiveRPCServiceRegistry.h"
#include "pva/ArchiveRPCServiceMONGO.h"
// #include "pva/ArchiveMonitorServiceRegistry.h"
// #include "pva/ArchiveMonitorServiceMONGO.h"

// should go after mongo because of INVALID_SOCKET
#include "pva/ArchiveServer.h"

// #include "storage/RawDataReaderFactoryEA3.h"
#include "storage/DataFileManagerEA3.h"

// If defined, the server writes log messages into
// this file. That helps with debugging because
// otherwise your PVRPC clients are likely
// to only show "error" and you have no clue
// what's happening.
#define LOGFILE "/tmp/archserverMONGO.log"

using namespace std;;
using namespace epics::pvAccess;
using namespace epics::pvData;
using namespace ea4;
using namespace ea4::storage;
using namespace ea4::pva;

int main(int argc,char *argv[]) {

  string url = "127.0.0.1:27017";
  string dbname = "ea4::nsls2";

  CmdArgParser parser(argc, argv);
  parser.setHeader("EPICS V4 interface to MongoDB \n\n");

  CmdArgString  urlArg (parser, "url", "", "MongoDB url (default: 127.0.0.1:27017)");
  CmdArgString  dbArg  (parser, "db", "", "Database name (default: ea4::nsls2)");

  if (!parser.parse()) {
     parser.usage();
     return -1;
  }

  if(parser.getArguments().size() == 0) {
        parser.usage();
        std::cout << std::endl;
  }

  if(urlArg.isSet()){ 
    url = urlArg.get();
  }

  if(dbArg.isSet()){ 
    dbname = dbArg.get();
  } 

  // register the file-specific reader

  //  RawDataReaderRegistry* rrr = RawDataReaderRegistry::getInstance();
  // rrr->setFactory("", new RawDataReaderFactoryEA3());

  DataFileRegistry* dfr = DataFileRegistry::getInstance();
  dfr->setManager(new DataFileManagerEA3());

  // Coonect to MongoDB

  ServerConfigMONGO* serverConfig = ServerConfigMONGO::getInstance();

  string errmsg;
  if (!serverConfig->mongo.connect(url, errmsg)){
        cout << "MongoDB : " << errmsg << endl;
        return 1;
  }

  // database name

serverConfig->setDBName(dbname.c_str());
  serverConfig->init();

  // Register the archive service

  ArchiveRPCService::shared_pointer rpcService(new ArchiveRPCServiceMONGO());
  rpcService->setServiceName("mongo");

  ArchiveRPCServiceRegistry* rpcRegistry = 
    ArchiveRPCServiceRegistry::getInstance();
  rpcRegistry->setRPCService(rpcService);

  // ArchiveMonitorService::shared_pointer 
  //  monService(new ArchiveMonitorServiceMONGO());
  // monService->setServiceName("nsls2");

  // ArchiveMonitorServiceRegistry* monRegistry = 
  //   ArchiveMonitorServiceRegistry::getInstance();
  // monRegistry->setMonitorService(monService);

  ArchiveServer server;

  //

#ifdef LOGFILE        
  MsgLogger logger(LOGFILE);
  LOG_MSG("---- ArchiveServerMONGO " EA4_SERVER_EA3_VERSION_TXT " Started ----\n");
#endif

  // Run the RPC & Monitor server

  server.printInfo();
  server.run();

  return 0;
}

