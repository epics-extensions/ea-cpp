#ifndef EA4_PVA_STATUS_H
#define EA4_PVA_STATUS_H

#include <list>
#include "pva/Def.h"

namespace ea4 { namespace pva {

/** Structure of the status result */

class Status {

 public:

  // OK - 0, Warrning - 1, Error - 2, Fatal - 3
  int type;

  std::string message;

  std::string callTree;

 public:

  static PVStructurePtr createPVStructure();
 
};

}}

#endif

