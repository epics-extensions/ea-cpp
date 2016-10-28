#ifndef EA4_ARCHIVE_RPC_CLIENT_H
#define EA4_ARCHIVE_RPC_CLIENT_H

#include "pv/rpcClient.h"
#include "pva/ArchiveRPCCommand.h"

namespace ea4 { namespace pva {

typedef epics::pvAccess::RPCClient::shared_pointer ArchiveRPCClientPtr;

/** Factory of RPCClient and repository of ArchiveRPCCommand's */
class ArchiveRPCClientFactory {
  
 public:

  /** returns a singleton */
  static ArchiveRPCClientFactory* getInstance();

  /** creates a RPC client with the specified name 
      (such as \"a4.rpc:\<service name\>\" )*/
  ArchiveRPCClientPtr createClient(const std::string& channelName);

 public:

  /** returns a selected command (such as \"getInfo\", \"getArchives\",
      \"getChannels\", and \"getValues\") */
  ArchiveCommand* const getCommand(const std::string& name);

 public:

  /** constructor */
  ArchiveRPCClientFactory();

 private:

  static ArchiveRPCClientFactory* theInstance;

  std::map<std::string, ArchiveCommand*> commands;

};

}}

#endif
