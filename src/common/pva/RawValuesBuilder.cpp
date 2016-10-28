#include "pva/RawValuesBuilder.h"

#include "alarm.h"

using namespace std;
using namespace std::tr1;
using namespace epics::pvAccess;
using namespace epics::pvData;

namespace ea4 { namespace pva {

RawValuesBuilder::RawValuesBuilder(const std::string& n,
				   epicsTimePtr start,
				   epicsTimePtr end,
				   int c) 
  : name(n),
    startTime(start), endTime(end), 
    count(c) {

  pvType  = pvDouble;
  pvCount = 1;

  /*
  shared_stats.reset(new vector<short>(count));
  shared_sevrs.reset(new vector<short>(count));
  shared_secs.reset(new vector<int>(count));
  shared_nanos.reset(new vector<int>(count));
  */
  shared_stats.reset(new vector<short>(0));
  shared_sevrs.reset(new vector<short>(0));
  shared_secs.reset(new vector<int>(0));
  shared_nanos.reset(new vector<int>(0));
}

void RawValuesBuilder::setResultType(const CtrlInfo& inf,  
				     DbrType dbrType,  
				     DbrCount dbrCount){

  info = inf;

   setPvType(dbrType, dbrCount);

   StructureConstPtr metaType = createMetaType();

   // create the result type

   FieldCreatePtr typeFactory = getFieldCreate();

   StringArray names;
   FieldConstPtrArray fields;

   names.push_back("name");
   fields.push_back(typeFactory->createScalar(pvString)); 

   names.push_back("meta");
   fields.push_back(metaType); 

   names.push_back("type");
   fields.push_back(typeFactory->createScalar(pvInt)); 

   names.push_back("count");
   fields.push_back(typeFactory->createScalar(pvInt));

   names.push_back("size");
   fields.push_back(typeFactory->createScalar(pvInt));

   StructureConstPtr dbrsType = createDBRsType(); 
   names.push_back("dbrs");
   fields.push_back(dbrsType); 

   resultType =  typeFactory->createStructure(names, fields);

}

StructureConstPtr RawValuesBuilder::createDBRsType(){

  FieldCreatePtr typeFactory = getFieldCreate();

  StringArray names;
  FieldConstPtrArray fields;

  names.push_back("stats");
  fields.push_back(typeFactory->createScalarArray(pvShort)); 

  names.push_back("sevrs");
  fields.push_back(typeFactory->createScalarArray(pvShort)); 

  names.push_back("secs");
  fields.push_back(typeFactory->createScalarArray(pvInt)); 

  names.push_back("nanos");
  fields.push_back(typeFactory->createScalarArray(pvInt)); 

  names.push_back("values");
  fields.push_back(typeFactory->createScalarArray(pvType)); 

  StructureConstPtr dbrsType =  typeFactory->createStructure(names, fields);

  return dbrsType;
}

StructureConstPtr RawValuesBuilder::createMetaType(){

    FieldCreatePtr typeFactory = getFieldCreate();

    // make a structure type

    StringArray names;
    FieldConstPtrArray fields;

    names.push_back("type");
    fields.push_back(typeFactory->createScalar(pvInt));

    if (info.getType() == CtrlInfo::Enumerated){

      names.push_back("states");
      fields.push_back(typeFactory->createScalarArray(pvString)); 

    } else {

      names.push_back("disp_high");
      fields.push_back(typeFactory->createScalar(pvDouble));   

      names.push_back("disp_low");
      fields.push_back(typeFactory->createScalar(pvDouble));

      names.push_back("alarm_high");
      fields.push_back(typeFactory->createScalar(pvDouble));   

      names.push_back("alarm_low");
      fields.push_back(typeFactory->createScalar(pvDouble));   

      names.push_back("warn_high");
      fields.push_back(typeFactory->createScalar(pvDouble));   

      names.push_back("warn_low");
      fields.push_back(typeFactory->createScalar(pvDouble)); 

      names.push_back("prec");
      fields.push_back(typeFactory->createScalar(pvInt)); 

      names.push_back("units");
      fields.push_back(typeFactory->createScalar(pvString));  
    }

    StructureConstPtr metaType =  typeFactory->createStructure(names, fields);
    return metaType;
}


void RawValuesBuilder::setPvType(DbrType dbrType, DbrCount dbrCount){
    switch (dbrType) {
        case DBR_TIME_CHAR:
        case DBR_TIME_SHORT:
        case DBR_TIME_ENUM:   
        case DBR_TIME_LONG:  
	  pvType = pvInt;      // 3
	  break;
        case DBR_TIME_FLOAT:  
        case DBR_TIME_DOUBLE:
	  pvType = pvDouble;   // 10
	  break;
    case DBR_TIME_STRING:  
	  pvType = pvString;   // 11
	  break;
        default:              
	  pvType = pvDouble; 
	  break;
    }
    
   pvCount = dbrCount;
}

void RawValuesBuilder::setMeta(PVStructurePtr& meta){

  if (info.getType() == CtrlInfo::Enumerated){

    // int type

    PVIntPtr typeField = meta->getIntField("type");
    typeField->put(ArchiveRPCCommand::META_TYPE_ENUM);

    // string[] states

    size_t num = info.getNumStates();

    // std::vector<string> states(num);
    PVStringArray::svector states;
    states.reserve(num);

    for (size_t i=0; i < num; ++i) {
      info.getState(i, states[i]);
    }

    PVStringArray::const_svector cstates(freeze(states));

    PVStringArrayPtr statesField = 
      static_pointer_cast<PVStringArray>(
	meta->getScalarArrayField("states", pvString)) ; 

    // statesField->put(0, num, states, 0);
    statesField->replace(cstates); 

    return;
  }

  if (info.getType() == CtrlInfo::Numeric){

    PVIntPtr typeField = meta->getIntField("type");
    typeField->put(ArchiveRPCCommand::META_TYPE_NUMERIC);

    PVDoublePtr disp_highField = meta->getDoubleField("disp_high");
    disp_highField->put(info.getDisplayHigh());

    PVDoublePtr disp_lowField = meta->getDoubleField("disp_low");
    disp_lowField->put(info.getDisplayLow());

    PVDoublePtr alarm_highField = meta->getDoubleField("alarm_high");
    alarm_highField->put(info.getHighAlarm());

    PVDoublePtr alarm_lowField = meta->getDoubleField("alarm_low");
    alarm_lowField->put(info.getLowAlarm());

    PVDoublePtr warn_highField = meta->getDoubleField("warn_high");
    warn_highField->put(info.getHighWarning());

    PVDoublePtr warn_lowField = meta->getDoubleField("warn_low");
    warn_lowField->put(info.getLowWarning());

    PVIntPtr precField = meta->getIntField("prec");
    precField->put(info.getPrecision());

    PVStringPtr unitsField = meta->getStringField("units");
    unitsField->put(info.getUnits());

    return;
  }

   PVIntPtr typeField = meta->getIntField("type");
   typeField->put(ArchiveRPCCommand::META_TYPE_NUMERIC);

   PVDoublePtr disp_highField = meta->getDoubleField("disp_high");
   disp_highField->put(0.0);

   PVDoublePtr disp_lowField = meta->getDoubleField("disp_low");
   disp_lowField->put(0.0);

   PVDoublePtr alarm_highField = meta->getDoubleField("alarm_high");
   alarm_highField->put(0.0);

   PVDoublePtr alarm_lowField = meta->getDoubleField("alarm_low");
   alarm_lowField->put(0.0);

   PVDoublePtr warn_highField = meta->getDoubleField("warn_high");
   warn_highField->put(0.0);

   PVDoublePtr warn_lowField = meta->getDoubleField("warn_low");
   warn_lowField->put(0.0);

   PVIntPtr precField = meta->getIntField("prec");
   precField->put(1);

   PVStringPtr unitsField = meta->getStringField("units");
   unitsField->put("<NO DATA>");

   return;
}




  }}
