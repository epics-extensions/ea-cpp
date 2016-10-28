
#include "alarm.h"

#include "VersionEA3.h"
#include "pva/ServerConfigEA3.h"
#include "pva/ArchiveRPCServiceRegistry.h"
#include "pva/ArchiveRPCServiceEA3.h"

#include "pva/ArchiveServer.h"

// #include "storage/RawDataReaderFactoryEA3.h"
#include "storage/DataFileManagerEA3.h"

// If defined, the server writes log messages into
// this file. That helps with debugging because
// otherwise your PVRPC clients are likely
// to only show "error" and you have no clue
// what's happening.
#define LOGFILE "/tmp/archserverCLS.log"

using namespace epics::pvAccess;
using namespace epics::pvData;

using namespace ea4;
using namespace ea4::storage;
using namespace ea4::pva;

int main(int argc,char *argv[]) {

  // register the file-specific reader

  //  RawDataReaderRegistry* rrr = RawDataReaderRegistry::getInstance();
  // rrr->setFactory("", new RawDataReaderFactoryEA3());

  DataFileRegistry* dfr = DataFileRegistry::getInstance();
  dfr->setManager(new DataFileManagerEA3());

  // Read the configuration file

  const char *configName = getenv("SERVERCONFIG");
  if (!configName) {
    std::cout << "SERVERCONFIG is undefined" << std::endl;
    return 1;
  }

  ServerConfigEA3* serverConfig = ServerConfigEA3::getInstance();
  serverConfig->setConfigName(configName);
  serverConfig->read();

  // Register the archive service

  ArchiveRPCService::shared_pointer rpcService(new ArchiveRPCServiceEA3());
  rpcService->setServiceName("classic");

  ArchiveRPCServiceRegistry* asr = ArchiveRPCServiceRegistry::getInstance();
  asr->setRPCService(rpcService);

  //

  ArchiveServer server;

#ifdef LOGFILE        
  MsgLogger logger(LOGFILE);
  LOG_MSG("---- ArchiveServer " EA4_SERVER_EA3_VERSION_TXT " Started ----\n");
#endif

  // Run the RPC server

  server.printInfo();
  server.run();

  return 0;
}

