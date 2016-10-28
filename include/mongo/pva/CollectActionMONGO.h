#ifndef EA4_COLLECT_ACTION_MONGO_H
#define EA4_COLLECT_ACTION_MONGO_H

#include "pva/Action.h"

namespace ea4 { namespace pva {

/** Collect action */
class CollectActionMONGO : public Action {

 public:

  /** constructor */
  CollectActionMONGO();

 public:

  // Action API

  PVStructurePtr apply(Action::Request& request);

 private:

  PVStructurePtr createResult(Action::Request& request);
  PVStructurePtr createEmptyResult(PVStructurePtr& pvStatus);

  /*
  PVStructurePtr processQuery(Action::Request& request, int key);
  PVStructurePtr processQueries(Action::Request& request, int key);
  */

  PVStructurePtr filter(const std::string& id, PVStructurePtr& query);
  PVStructurePtr read(const std::string& id, PVStructurePtr& filterResult, 
		      PVStructurePtr& query);

  /*
  PVStructurePtr getChannels(int key, const std::string& pattern);
  PVStructurePtr getValues(int key, PVStructurePtr channels, 
			   int64_t start, int64_t end, int limit);
  */

 private:

  // std::map< std::string, std::string> cmdNames;

};

}}

#endif
