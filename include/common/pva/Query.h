#ifndef EA4_PVA_QUERY_H
#define EA4_PVA_QUERY_H

#include <list>
#include "pva/Def.h"

namespace ea4 { namespace pva {

/** Structure of the query request */

class Query {

 public:

  /** Request structure */
  class Request {

  public:

    Request();

    Request(const Request& request);

    Request& operator=(const Request& request);

  public:

    struct Src {
      std::string id;
    } src;

    std::list<epics::pvData::PVStructurePtr> queries;

  };

 public:

  virtual PVStructurePtr apply(const std::string& id, 
			       const PVStructurePtr& input,
			       const PVStructurePtr& query) = 0;

 public:

  // Helpers

  /** defines the pvData request message {src, queries} from the Request structure */
  static PVStructurePtr createRequestMsg(const Query::Request& request);

  /** extract the Request structure from the pvData request message */
  static void getRequestMsg(PVStructurePtr const & msg, Query::Request& request);

  /** creates a request type structure {string command, Query query } */
  static  StructureConstPtr createRequestType(const Query::Request& request);

 protected:

  /** creates a Query type { Src src, Queries queries} */
  static StructureConstPtr createQueryType(const Query::Request& request);

  /** creates a Queries type: array of queries */
  static StructureConstPtr createQueriesType(const Query::Request& request);

  /** creates a Queries field */
  static PVStructurePtr createQueriesField(const Query::Request& request);

  /** creates a Src type of the Request structure */
  static StructureConstPtr createSrcType();

  /** creates a Src field  */
  static PVStructurePtr createSrcField(const Query::Request& request);

};

}}

#endif

