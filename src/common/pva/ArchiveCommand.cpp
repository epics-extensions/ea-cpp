#include "pva/ArchiveCommand.h"

#include "alarm.h"

#include "tools/MsgLogger.h"
#include "tools/epicsTimeHelper.h"

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;

namespace ea4 {  namespace pva {

StructureConstPtr ArchiveCommand::createMetaType(const CtrlInfo* info){

    FieldCreatePtr typeFactory = getFieldCreate();

    // make a structure type

    StringArray names;
    FieldConstPtrArray fields;

    names.push_back("type");
    fields.push_back(typeFactory->createScalar(pvInt));

    if (info && info->getType() == CtrlInfo::Enumerated){

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

void ArchiveCommand::setMeta(PVStructurePtr& meta, const CtrlInfo* info){

  if (info && info->getType() == CtrlInfo::Enumerated){

    // int type

    PVIntPtr typeField = meta->getIntField("type");
    typeField->put(META_TYPE_ENUM);

    // string[] states

    size_t num = info->getNumStates();

    PVStringArray::svector states;
    states.resize(num);

    for (size_t i=0; i < num; ++i) {
      info->getState(i, states[i]);
    }

   PVStringArray::const_svector cstates(freeze(states));

   //

    PVStringArrayPtr statesField = 
      static_pointer_cast<PVStringArray>(
	meta->getScalarArrayField("states", pvString)) ; 

    statesField->replace(cstates);

    return;
  }

  if (info && info->getType() == CtrlInfo::Numeric){

    PVIntPtr typeField = meta->getIntField("type");
    typeField->put(META_TYPE_NUMERIC);

    PVDoublePtr disp_highField = meta->getDoubleField("disp_high");
    disp_highField->put(info->getDisplayHigh());

    PVDoublePtr disp_lowField = meta->getDoubleField("disp_low");
    disp_lowField->put(info->getDisplayLow());

    PVDoublePtr alarm_highField = meta->getDoubleField("alarm_high");
    alarm_highField->put(info->getHighAlarm());

    PVDoublePtr alarm_lowField = meta->getDoubleField("alarm_low");
    alarm_lowField->put(info->getLowAlarm());

    PVDoublePtr warn_highField = meta->getDoubleField("warn_high");
    warn_highField->put(info->getHighWarning());

    PVDoublePtr warn_lowField = meta->getDoubleField("warn_low");
    warn_lowField->put(info->getLowWarning());

    PVIntPtr precField = meta->getIntField("prec");
    precField->put(info->getPrecision());

    PVStringPtr unitsField = meta->getStringField("units");
    unitsField->put(info->getUnits());

    return;
  }

   PVIntPtr typeField = meta->getIntField("type");
   typeField->put(META_TYPE_NUMERIC);

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

StructureConstPtr
ArchiveCommand::createDbrTimeValueType(ScalarType pv_type){

  FieldCreatePtr typeFactory = getFieldCreate();

  // create the value type

  StringArray names;
  FieldConstPtrArray fields;

  names.push_back("stat");
  fields.push_back(typeFactory->createScalar(pvInt)); 

  names.push_back("sevr");
  fields.push_back(typeFactory->createScalar(pvInt)); 

  names.push_back("secs");
  fields.push_back(typeFactory->createScalar(pvInt));

  names.push_back("nano");
  fields.push_back(typeFactory->createScalar(pvInt)); 

  names.push_back("value");
  fields.push_back(typeFactory->createScalarArray(pv_type)); 

  names.push_back("min");
  fields.push_back(typeFactory->createScalar(pvDouble));

  names.push_back("max");
  fields.push_back(typeFactory->createScalar(pvDouble)); 

  StructureConstPtr valueType =  typeFactory->createStructure(names, fields);

  return valueType;

}


void ArchiveCommand::dbr_type_to_pv_type(DbrType dbr_type, 
					 DbrCount dbr_count,
					 ScalarType& pv_type, 
					 int& pv_count){
    switch (dbr_type) {
        case DBR_TIME_CHAR:
        case DBR_TIME_SHORT:
        case DBR_TIME_ENUM:   
        case DBR_TIME_LONG:  
	  pv_type = pvInt;      // 3
	  break;
        case DBR_TIME_FLOAT:  
        case DBR_TIME_DOUBLE:
	  pv_type = pvDouble;   // 10
	  break;
    case DBR_TIME_STRING:  
	  pv_type = pvString;   // 11
	  break;
        default:              
	  pv_type = pvDouble; 
	  break;
    }
    
   pv_count = dbr_count;
}

// Given a raw sample dbr_type/count/time/data,
// map it onto xml_type/count and add to values.
// Handles the special case data == 0,
// which happens for undefined cells in a SpreadsheetReader.
void ArchiveCommand::encode_value(DbrType dbr_type, DbrCount dbr_count,
				  const epicsTime& time, 
				  const RawValue::Data* data,
				  ScalarType pv_type, 
				  int pv_count,
				  PVStructurePtr& dbrTimeValue,
				  bool with_min_max,
				  double minimum,
				  double maximum) {

  if (pv_count > dbr_count) pv_count = dbr_count;
    
  switch (pv_type) {

  case pvInt: // 3
    {
      PVIntArray::svector val_array;
      val_array.resize(pv_count);

      long l;
      for (int i=0; i < pv_count; ++i) {
	if (!data  || !RawValue::getLong(dbr_type, dbr_count, data, l, i)){
	  l = 0;
	}
	val_array[i] = l;
      }

      PVIntArray::const_svector cval_array(freeze(val_array));

      PVIntArrayPtr val_arrayField = 
	static_pointer_cast<PVIntArray>(
	     dbrTimeValue->getScalarArrayField("value", pvInt));

      val_arrayField->replace(cval_array);      
    }

    break;

  case pvDouble: // 10
    {            
      PVDoubleArray::svector val_array;
      val_array.resize(pv_count);

      double d;
      for (int i=0; i < pv_count; ++i) {                
	if (!data  || !RawValue::getDouble(dbr_type, dbr_count, data, d, i)){
	  d = 0.0;
	}
	val_array[i] = d;
      }

     PVDoubleArray::const_svector cval_array(freeze(val_array));

      PVDoubleArrayPtr val_arrayField = 
	static_pointer_cast<PVDoubleArray>(
	     dbrTimeValue->getScalarArrayField("value", pvDouble));

       val_arrayField->replace(cval_array); 
    }

    break;
        
  case pvString: // 11
    {
      PVStringArray::svector val_array;
      val_array.resize(1);
            
      if (data) {
	RawValue::getValueString(val_array[0], dbr_type, dbr_count, data, 0);
      }

      PVStringArray::const_svector cval_array(freeze(val_array));

      PVStringArrayPtr val_arrayField = 
	static_pointer_cast<PVStringArray>(
	     dbrTimeValue->getScalarArrayField("value", pvString));

      val_arrayField->replace(cval_array); 
    }

    break;
        
  }

  int secs, nano;
  epicsTime2pieces(time, secs, nano);
 
  if (data) {

     PVIntPtr statField = dbrTimeValue->getIntField("stat");
     statField->put(RawValue::getStat(data));

     PVIntPtr sevrField = dbrTimeValue->getIntField("sevr");
     sevrField->put(RawValue::getSevr(data));

     if (with_min_max){

       PVDoublePtr minField = dbrTimeValue->getDoubleField("min");
       minField->put(minimum);

       PVDoublePtr maxField = dbrTimeValue->getDoubleField("max");
       maxField->put(maximum);

     } else {

       PVDoublePtr minField = dbrTimeValue->getDoubleField("min");
       minField->put(0.0);

       PVDoublePtr maxField = dbrTimeValue->getDoubleField("max");
       maxField->put(0.0);
     }

  } else {

     PVIntPtr statField = dbrTimeValue->getIntField("stat");
     statField->put(UDF_ALARM);

     PVIntPtr sevrField = dbrTimeValue->getIntField("sevr");
     sevrField->put(INVALID_ALARM);

     PVDoublePtr minField = dbrTimeValue->getDoubleField("min");
     minField->put(0.0);

     PVDoublePtr maxField = dbrTimeValue->getDoubleField("max");
     maxField->put(0.0);
  }

  PVIntPtr secsField = dbrTimeValue->getIntField("secs");
  secsField->put(secs);

  PVIntPtr nanoField = dbrTimeValue->getIntField("nano");
  nanoField->put(nano);
}

  }}


