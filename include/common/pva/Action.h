#ifndef EA4_PVA_ACTION_H
#define EA4_PVA_ACTION_H

#include "pva/Def.h"
#include "pva/Query.h"

namespace ea4 { namespace pva {

/** Basis class of the IO Node actions */
class Action {

 public:

  /** Request structure */
  struct Request {

    struct Query::Request query;

    struct Action {
      std::string cmd;
    } action;

  };

 public:

  // Action API

  virtual PVStructurePtr apply(Action::Request& request) = 0;

};

}}

#endif

