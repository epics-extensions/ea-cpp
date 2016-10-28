#include "pva/InfoQueryCLS.h"

// EPICS_REVISION, EPICS_MODIFICATION, 
// EPICS_VERSION_STRING
#include "epicsVersion.h"

// ALARM_NSTATUS, ALARM_NSEV
#include "alarm.h"

// tools
#include "tools/ToolsConfig.h"

// ARCH_EST_REPEAT, ..., ARCH_DISABLED
#include "storage/RawValue.h"
#include "storage/AutoIndex.h"

// 
#include "VersionEA3.h"
#include "pva/Status.h"
#include "pva/ServerConfigEA3.h"

#if (EPICS_REVISION == 14 && EPICS_MODIFICATION >= 11)
extern const char *epicsAlarmConditionStrings[ALARM_NSTATUS];
extern const char *epicsAlarmSeverityStrings[ALARM_NSEV];
#else
extern const char *alarmStatusString[];
extern const char *alarmSeverityString[];
#endif

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;

namespace ea4 { namespace pva {

InfoQueryCLS::InfoQueryCLS(){
}

PVStructurePtr InfoQueryCLS::apply(const std::string& id, 
				   const PVStructurePtr& input,
				   const PVStructurePtr& query){

  LOG_MSG("InfoQueryCLS::apply\n");

  PVStructurePtr result;
  PVStructurePtr pvStatus = Status::createPVStructure();

  ServerConfigEA3* config = ServerConfigEA3::getInstance();
  const char* configName  = config->getConfigName();

  if (!configName) {

    PVIntPtr typeField = pvStatus->getIntField("type");
    typeField->put(3); // fatal

    PVStringPtr messageField = pvStatus->getStringField("message");
    messageField->put("InfoQueryCLS::apply: configName is not defined\n");

    result = createEmptyResult(pvStatus);
    return result;
  }

  LOG_MSG("config: '%s'\n", configName);

  result = createResult(pvStatus);
  return result;
}

PVStructurePtr InfoQueryCLS::createEmptyResult(PVStructurePtr& pvStatus){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(1);
  PVFieldPtrArray fields(1);

  names[0]  = "status";
  fields[0] = pvStatus;

  PVStructurePtr result = dataFactory->createPVStructure(names, fields);
  return result;
}

PVStructurePtr InfoQueryCLS::createResult(PVStructurePtr& pvStatus){

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

PVStructurePtr InfoQueryCLS::createDocs(PVStructurePtr& pvStatus){

  PVStructurePtr result;
  PVDataCreatePtr dataFactory = getPVDataCreate();

 // compose a result

  StringArray names(2);
  PVFieldPtrArray fields(2);

  names[0]  = "size";
  PVIntPtr sizeField = 
    static_pointer_cast<PVInt>(dataFactory->createPVScalar(pvInt));
  sizeField->put(1);
  fields[0] = sizeField;

  names[1]  = "0";
  PVStructurePtr pvDoc = createDoc(pvStatus); 
  fields[1] = pvDoc;

  result = dataFactory->createPVStructure(names, fields);
  return result; 
}

PVStructurePtr InfoQueryCLS::createDoc(PVStructurePtr& pvStatus){

  PVStructurePtr result;
  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(5);
  PVFieldPtrArray fields(5);

  names[0]  = "ver";
  PVIntPtr verField = 
    static_pointer_cast<PVInt>(dataFactory->createPVScalar(pvInt));
  verField->put(EA4_SERVER_EA3_VERSION);
  fields[0] = verField;

  names[1]  = "desc";
  fields[1] = createDescField();

  names[2]  = "how";
  fields[2] = createHowField();

  names[3]  = "stat";
  fields[3] = createStatField();

  names[4]  = "sevr";
  fields[4] = createSevrField();

  result = dataFactory->createPVStructure(names, fields);
  return result; 
}

PVFieldPtr InfoQueryCLS::createDescField(){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  ServerConfigEA3* config = ServerConfigEA3::getInstance();
  const char* configName  = config->getConfigName();

  char description[500];
  sprintf(description, 
	  "Channel Archiver Data Server V%d,\n"
	  "for " EPICS_VERSION_STRING ",\n"
	  "built " __DATE__ ", " __TIME__ "\n"
	  "from sources for version '%s' \n"
	  "Config: '%s'\n",
	  EA4_SERVER_EA3_VERSION, 
	  EA4_SERVER_EA3_VERSION_TXT, 
	  config->getConfigName());

  PVStringPtr descField = 
    static_pointer_cast<PVString>(dataFactory->createPVScalar(pvString));
  descField->put(description);

  return descField;
}

PVFieldPtr InfoQueryCLS::createHowField(){

  PVDataCreatePtr df = getPVDataCreate();

  // std::vector<string> hows(6);
  PVStringArray::svector hows;
  hows.resize(6);

  hows[0] = "raw";
  hows[1] = "spreadsheet";
  hows[2] = "average (w/ count)";
  hows[3] = "plot-binning";
  hows[4] = "linear";
  hows[5] = "average";

  PVStringArray::const_svector chows(freeze(hows));

  PVStringArrayPtr howField = 
    static_pointer_cast<PVStringArray>(df->createPVScalarArray(pvString)) ; 

  // howField->put(0, 6, hows, 0);
  howField->replace(chows); 

  return howField;
}

PVFieldPtr InfoQueryCLS::createStatField(){

  PVDataCreatePtr df = getPVDataCreate();

  // Interface to alarm.h is different in EPICS R3.14.11

  // std::vector<string> stats(lastEpicsAlarmCond + 1);
  PVStringArray::svector stats;
  stats.resize(lastEpicsAlarmCond + 1);

  // 'status': array of all status string.
  for (int i=0; i <= lastEpicsAlarmCond; ++i){
    #if (EPICS_REVISION == 14 && EPICS_MODIFICATION >= 11)
    stats[i] = epicsAlarmConditionStrings[i];
    #else 
    stats[i] = alarmStatusString[i];
    #endif
  }

  PVStringArray::const_svector cstats(freeze(stats));

  PVStringArrayPtr statField = 
    static_pointer_cast<PVStringArray>(df->createPVScalarArray(pvString)) ; 

  // statField->put(0, lastEpicsAlarmCond + 1, stats, 0);
  statField->replace(cstats); 

  return statField;
}

PVFieldPtr InfoQueryCLS::createSevrField() {

  PVStructurePtr result;
  PVDataCreatePtr dataFactory = getPVDataCreate();

  // allocate containers

  int size = lastEpicsAlarmSev + 1 + 5;

  StringArray names(size + 1);
  PVFieldPtrArray fields(size + 1);

  // create instances

  names[0] = "size";
  PVIntPtr sizeField = 
    static_pointer_cast<PVInt>(dataFactory->createPVScalar(pvInt));
  sizeField->put(size); 
  fields[0] = sizeField;

  // 'severity': array of all severity strings.

  for (int i=0; i <= lastEpicsAlarmSev; ++i) {
    stringstream ss; ss << i;
    names[i+1] = ss.str();

    #if (EPICS_REVISION == 14 && EPICS_MODIFICATION >= 11)
    fields[i+1] = createPVSevr(i, epicsAlarmSeverityStrings[i], 1, 1);
    #else 
    fields[i+1] = createPVSevr(i, alarmSeverityString[i], 1, 1);  
    #endif
  }

  // ... including the archive-specific ones.

  int index = lastEpicsAlarmSev + 1;

  stringstream ss1; ss1 << index;
  names[index+1] = ss1.str();
  fields[index+1] = createPVSevr(ARCH_EST_REPEAT, "Est_Repeat", 1, 0); 
  index++;

  stringstream ss2; ss2 << index;
  names[index+1] = ss2.str();
  fields[index+1] = createPVSevr(ARCH_REPEAT, "Repeat", 1, 0); 
  index++;

  stringstream ss3; ss3 << index;
  names[index+1] = ss3.str();
  fields[index+1] = createPVSevr(ARCH_DISCONNECT, "Disconnected", 0, 1); 
  index++;

  stringstream ss4; ss4 << index;
  names[index+1] = ss4.str();
  fields[index+1] = createPVSevr(ARCH_STOPPED, "Archive_Off", 0, 1); 
  index++;

  stringstream ss5; ss5 << index;
  names[index+1]  = ss5.str();
  fields[index+1] = createPVSevr(ARCH_DISABLED, "Archive_Disabled", 0, 1); 

  result = dataFactory->createPVStructure(names, fields);
  return result; 
}

PVStructurePtr InfoQueryCLS::createPVSevr(int num,
					  const char* sevr,
					  bool has_value,
					  bool txt_stat){
  PVStructurePtr result;
  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(4);
  PVFieldPtrArray fields(4);

  names[0] = "num";
  PVIntPtr numField = 
    static_pointer_cast<PVInt>(dataFactory->createPVScalar(pvInt));
  numField->put(num);
  fields[0] = numField;

  names[1] = "sevr";
  PVStringPtr sevrField = 
    static_pointer_cast<PVString>(dataFactory->createPVScalar(pvString));
  sevrField->put(sevr);
  fields[1] = sevrField;

  names[2] = "has_value";
  PVIntPtr has_valueField = 
    static_pointer_cast<PVInt>(dataFactory->createPVScalar(pvInt));
  has_valueField->put(has_value);
  fields[2] = has_valueField;

  names[3] = "txt_stat";
  PVIntPtr txt_statField = 
    static_pointer_cast<PVInt>(dataFactory->createPVScalar(pvInt));
  txt_statField->put(txt_stat);
  fields[3] = txt_statField;

  result = dataFactory->createPVStructure(names, fields);
  return result; 
}

				
}}



