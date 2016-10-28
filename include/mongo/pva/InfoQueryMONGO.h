#ifndef EA4_INFO_QUERY_MONGO_H
#define EA4_INFO_QUERY_MONGO_H

#include "pva/InfoQuery.h"

namespace ea4 { namespace pva {

/** Query returning the general info: 
 * arrays of the 'status' strings, severity' strings, etc.
*/

class InfoQueryMONGO : public InfoQuery {

public:

  /** Constructor */
  InfoQueryMONGO();

 public:

  virtual PVStructurePtr apply(const std::string& id, 
			       const PVStructurePtr& input,
			       const PVStructurePtr& query);

 private:

  PVStructurePtr createEmptyResult(PVStructurePtr& pvStatus);
  PVStructurePtr createResult(PVStructurePtr& pvStatus);
  PVStructurePtr createDocs(PVStructurePtr& pvStatus);
  PVStructurePtr createDoc(PVStructurePtr& pvStatus);
  PVFieldPtr createDescField();
  PVFieldPtr createHowField();
  PVFieldPtr createStatField();
  PVFieldPtr createSevrField();
  PVStructurePtr createPVSevr(int num,
			      const char* sevr,
			      bool has_value,
			      bool txt_stat);

};

}}

#endif
