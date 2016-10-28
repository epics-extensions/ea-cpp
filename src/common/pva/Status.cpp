#include "pva/Status.h"

using namespace std;
using namespace epics::pvData;

namespace ea4 { namespace pva {

PVStructurePtr Status::createPVStructure(){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(3);
  PVFieldPtrArray fields(3);

  names[0] = "type";
  fields[0] = dataFactory->createPVScalar(pvInt);

  names[1] = "message";
  fields[1] = dataFactory->createPVScalar(pvString);

  names[2] = "callTree";
  fields[2] = dataFactory->createPVScalar(pvString);

  PVStructurePtr result = dataFactory->createPVStructure(names, fields);
  return result;
}

}}

