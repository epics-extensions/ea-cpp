#ifndef EA4_PVA_READ_QUERY_CLS_H
#define EA4_PVA_READ_QUERY_CLS_H

#include <sstream>
#include <tr1/memory>

#include "storage/DataReader.h"
#include "pva/ReadQuery.h"
#include "pva/RawValuesBuilder.h"


typedef std::tr1::shared_ptr<Index> IndexPtr;

namespace ea4 { namespace pva {

/** Query returning the pv values */
class ReadQueryCLS : public ReadQuery {

 public:

  /** constructor */
  ReadQueryCLS();

 public:

  PVStructurePtr apply(const std::string& id, 
		       const PVStructurePtr& pvInput,
		       const PVStructurePtr& query);

 protected:

  PVStructurePtr createResult(PVStructurePtr& pvStatus,
			      const std::string& id,
			      const Input& input,
			      const Request& query);
  PVStructurePtr createDocs(PVStructurePtr& pvStatus, 
			    const std::string& id,
			    const Input& input,
			    const Request& query);
  PVStructurePtr createDoc(PVStructurePtr& pvStatus, 
			   const std::string& id,
			   const std::string& pvName,
			   const Request& query);

 protected:

  IndexPtr createIndex(const std::string& id);
  DataReaderPtr  createDataReader(IndexPtr index);

};

}}

#endif


