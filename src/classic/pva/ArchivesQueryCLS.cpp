#include "pva/ArchivesQueryCLS.h"
#include "pva/ServerConfigEA3.h"

#include "tools/ToolsConfig.h"
#include "storage/AutoIndex.h"

#include "pva/Status.h"

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;

// #define DEBUG_COMMAND

namespace ea4 { namespace pva {

ArchivesQueryCLS::ArchivesQueryCLS(){
}


PVStructurePtr ArchivesQueryCLS::apply(const std::string& id, 
				       const PVStructurePtr& input,
				       const PVStructurePtr& query){

  LOG_MSG("ArchivesQuery::apply\n");

  PVStructurePtr result;
  PVStructurePtr pvStatus = Status::createPVStructure();
 
  // access the config data

  Archives* archives = Archives::getInstance();
  int size = archives->archives.size();

  if (!size) {

    PVIntPtr typeField = pvStatus->getIntField("type");
    typeField->put(1); // Warning

    PVStringPtr messageField = pvStatus->getStringField("message");
    messageField->put("ArchivesQueryCLA::apply: no archives\n");

    result = createEmptyResult(pvStatus);
    return result;
  }

  result = createResult(pvStatus);
  return result;
}

PVStructurePtr ArchivesQueryCLS::createEmptyResult(PVStructurePtr& pvStatus){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(1);
  PVFieldPtrArray fields(1);

  names[0]  = "status";
  fields[0] = pvStatus;

  PVStructurePtr result = dataFactory->createPVStructure(names, fields);
  return result;
}

PVStructurePtr ArchivesQueryCLS::createResult(PVStructurePtr& pvStatus){

  PVStructurePtr result;
  PVDataCreatePtr dataFactory = getPVDataCreate();

  PVStructurePtr pvDocs = createDocs(pvStatus); 
  if(pvDocs.get() == 0) {
    result = createEmptyResult(pvStatus);
    return result;
  }

  // Status: OK
  
  pvStatus->getIntField("type")->put(0);

  // compose a result

  StringArray names(2);
  PVFieldPtrArray fields(2);

  names[0]  = "status";
  fields[0] = pvStatus;

  names[1]  = "docs";
  fields[1] = pvDocs;

  result = dataFactory->createPVStructure(names, fields);
  return result;
}

PVStructurePtr ArchivesQueryCLS::createDocs(PVStructurePtr& pvStatus){

  PVStructurePtr result;
  PVDataCreatePtr dataFactory = getPVDataCreate();

  // allocate containers

  Archives* archives = Archives::getInstance();
  int size = archives->archives.size();

  StringArray names(size + 1);
  PVFieldPtrArray fields(size + 1);

  names[0] = "size";
  PVIntPtr sizeField = 
    static_pointer_cast<PVInt>(dataFactory->createPVScalar(pvInt));
  sizeField->put(size); 
  fields[0] = sizeField;

  int i = 0;
  map<int, Archives::ArchivesEntry>::const_iterator it;
  for ( it = archives->archives.begin(); it != archives->archives.end(); ++it){

    const Archives::ArchivesEntry& entry = it->second;

#ifdef DEBUG_COMMAND
    std::cout << entry.key << ", " 
    	      << entry.name.c_str() << ", " 
    	      << entry.path.c_str() << std::endl;
#endif

    stringstream ss; ss << i;
    names[i+1] = ss.str();

    PVStructurePtr pvDoc = createDoc(pvStatus, entry); 
    fields[i+1] = pvDoc;
    i++;
  }

  result = dataFactory->createPVStructure(names, fields);
  return result; 
}

PVStructurePtr ArchivesQueryCLS::createDoc(PVStructurePtr& pvStatus,
					   const Archives::ArchivesEntry& entry){

  PVStructurePtr result;
  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(3);
  PVFieldPtrArray fields(3);

  names[0] = "key";
  PVIntPtr keyField = 
    static_pointer_cast<PVInt>(dataFactory->createPVScalar(pvInt));
  keyField->put(entry.key);
  fields[0] = keyField;

  names[1] = "name";
  PVStringPtr nameField = 
    static_pointer_cast<PVString>(dataFactory->createPVScalar(pvString));
  nameField->put(entry.name.c_str());
  fields[1] = nameField;

  names[2] = "path";
  PVStringPtr pathField = 
    static_pointer_cast<PVString>(dataFactory->createPVScalar(pvString));
  pathField->put(entry.path.c_str());
  fields[2] = pathField;

  result = dataFactory->createPVStructure(names, fields);
  return result;
}

}}


