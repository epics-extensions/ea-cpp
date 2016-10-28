#ifndef EA4_PVA_READ_QUERY_H
#define EA4_PVA_READ_QUERY_H

#include <sstream>
#include <tr1/memory>

#include "tools/epicsTimeHelper.h"
#include "pva/Query.h"

#define MAX_COUNT 1000000

namespace ea4 { namespace pva {

/** Basis class of the ReadQuery implementations */
class ReadQuery  : public Query {

 public:

  struct Input {
    std::vector<std::string> names;
  };

  struct Request {
    epicsTimePtr start;
    epicsTimePtr end;
    int limit;
  };

 protected: 

  PVStructurePtr createEmptyResult(PVStructurePtr& pvStatus);

  // get channels
  void decodeInput(const PVStructurePtr& pvInput, Input& input);

  // get start, end, limit
  void decodeQuery(const PVStructurePtr& pvQuery, Request& request);
  void decodeMatchField(const PVFieldPtr& matchField, Request& request);
};

}}


#endif
