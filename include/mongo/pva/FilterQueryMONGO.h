#ifndef EA4_FILTER_QUERY_MONGO_H
#define EA4_FILTER_QUERY_MONGO_H

#include "mongo/client/dbclient.h"
#include "pva/Query.h"

namespace ea4 { namespace pva {

/** MONGO command implementing the filter query */
class FilterQueryMONGO : public Query {

 public:

  FilterQueryMONGO();

 public:

  /** Process query */
  virtual PVStructurePtr apply(const std::string& id, 
			       const PVStructurePtr& input,
			       const PVStructurePtr& query);

 private:

  // prepare a MongoDB query from pvData

  mongo::BSONObj getQuery(const std::string& id, const PVStructurePtr& query);
  mongo::BSONObj createQuery(const std::string& id);

  void getPipe(const PVStructurePtr& pvPipe, mongo::BSONObjBuilder& bp);

  void appendDict(const std::string& fieldName, 
		  const PVStructurePtr& pvDict, 
		  mongo::BSONObjBuilder& b);

  void appendScalar(epics::pvData::ScalarType fieldType, 
		    const std::string& fieldName, 
		    const PVFieldPtr& pvField, 
		    mongo::BSONObjBuilder& b);

 private:

  // create pvData from the MongoDB results

  PVStructurePtr createEmptyResult(PVStructurePtr& pvStatus);
  PVStructurePtr createResult(PVStructurePtr& pvStatus, mongo::BSONObj& bQuery);
  PVStructurePtr createDocs(PVStructurePtr& pvStatus, mongo::BSONObj& bQuery);
  PVStructurePtr createPVDoc(const mongo::BSONObj& doc, bool withOID);
  PVFieldPtr createPVArray(const mongo::BSONElement& elem);
  PVFieldPtr createPVDoubleArray(const std::vector<mongo::BSONElement>& elems);
  PVFieldPtr createPVStringArray(const std::vector<mongo::BSONElement>& elems);
  PVFieldPtr createPVStructArray(const std::vector<mongo::BSONElement>& elems);
};

}}

#endif
