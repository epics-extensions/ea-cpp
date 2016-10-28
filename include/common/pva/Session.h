#ifndef EA4_ION_SESSION_H
#define EA4_ION_SESSION_H

#include <string>
#include <map>

#include "pva/Def.h"

namespace epics { namespace pvAccess {

class RPCClient;
typedef std::tr1::shared_ptr<epics::pvAccess::RPCClient> RPCClientPtr;

}}

namespace ea4 { namespace pva {

class Collection;
typedef std::tr1::shared_ptr<Collection> CollectionPtr;

/** Class representing a client session of the ION processing */

class Session  {

 public:

  /** constructor */
  Session();

 public:

  /** open a session */
  int open(const std::string& chName, const std::string& id, double timeout);

  PVStructurePtr request(PVStructurePtr message, double timeout);

  /** closes this session */
  void close();

 public:

  // factory methods

  /** creates a proxy collection */
  CollectionPtr createProxy(const std::string& dbns);

 public:

  std::tr1::shared_ptr<epics::pvAccess::RPCClient> getClient();

 protected:

  /** RPC client of the ION service */
  std::tr1::shared_ptr<epics::pvAccess::RPCClient> client;

  /** channel name of the ION service */
  std::string chName;

  std::map<int, CollectionPtr > proxies;

};



}}

#endif
