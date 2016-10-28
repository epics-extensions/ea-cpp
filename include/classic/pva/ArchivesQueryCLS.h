#ifndef EA4_PVA_ARCHIVES_QUERY_CLS_H
#define EA4_PVA_ARCHIVES_QUERY_CLS_H

#include "pva/Query.h"
#include "pva/Archives.h"

namespace ea4 { namespace pva {

 /** Command returning the array of the index files */
class ArchivesQueryCLS : public Query {

public:

  /** Constructor */
  ArchivesQueryCLS();

 public:

  virtual PVStructurePtr apply(const std::string& id, 
			       const PVStructurePtr& input,
			       const PVStructurePtr& query);

private:

  PVStructurePtr createEmptyResult(PVStructurePtr& pvStatus);
  PVStructurePtr createResult(PVStructurePtr& pvStatus);
  PVStructurePtr createDocs(PVStructurePtr& pvStatus);
  PVStructurePtr createDoc(PVStructurePtr& pvStatus, 
			   const Archives::ArchivesEntry& entry);

};

}}

#endif
