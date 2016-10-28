#ifndef EA4_PVA_PVS_QUERY_CLS_H
#define EA4_PVA_PVS_QUERY_CLS_H

#include <tr1/memory>

// storage
#include "storage/AutoIndex.h"

#include "pva/Query.h"

namespace ea4 { namespace pva {

/** Queyr returning an array of the channel infos */
class PVsQueryCLS : public Query {

 public:

  /** constructor */
  PVsQueryCLS();

 public:

  virtual PVStructurePtr apply(const std::string& id, 
			       const PVStructurePtr& input,
			       const PVStructurePtr& query);

 public:

  PVStructurePtr createEmptyResult(PVStructurePtr& pvStatus);
  PVStructurePtr createResult(PVStructurePtr& pvStatus, 
			      const std::string& id, const std::string& pattern);
  PVStructurePtr createDocs(PVStructurePtr& pvStatus,
			    const std::string& id, const std::string& pattern);
  PVStructurePtr createDoc(const std::string& name,
			   const Interval& interval);
 
  std::string decodeQuery(const PVStructurePtr& query);
  std::string decodeMatchField(const PVFieldPtr& matchField);

 private:

  std::tr1::shared_ptr<Index> createIndex(const std::string& id);


};

}}

#endif


