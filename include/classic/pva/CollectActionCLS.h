#ifndef EA4_COLLECT_ACTION_CLS_H
#define EA4_COLLECT_ACTION_CLS_H

#include "pva/Action.h"

namespace ea4 { namespace pva {

/** Collect action */
class CollectActionCLS : public Action {

 public:

  /** constructor */
  CollectActionCLS();

 public:

  // Action API

  PVStructurePtr apply(Action::Request& request);

 private:

  PVStructurePtr createResult(Action::Request& request);
  PVStructurePtr createEmptyResult(PVStructurePtr& pvStatus);

  PVStructurePtr filter(const std::string& id, PVStructurePtr& query);
  PVStructurePtr read(const std::string& id, PVStructurePtr& filterResult, 
		      PVStructurePtr& query);
};

}}

#endif
