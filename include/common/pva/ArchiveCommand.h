#ifndef EA4_ARCHIVE_COMMAND_H
#define EA4_ARCHIVE_COMMAND_H

// epics 3
#include <epicsTime.h>

// epics 4
#include "pv/pvData.h"
#include "pv/thread.h"

#include "storage/CtrlInfo.h"
#include "storage/RawValue.h"

#include "pva/Def.h"

namespace ea4 { namespace pva {

class ArchiveCommand;
typedef std::tr1::shared_ptr<ArchiveCommand> ArchiveCommandPtr;

/** Basis class of the Archive commands */
class ArchiveCommand  {

 public:

  virtual ~ArchiveCommand () {}

 public:

  /** Raw data, channel by channel */
  static const int HOW_RAW = 0;

  /** Raw data in 'filled' spreadsheet */
  static const int HOW_SHEET = 1;

  /** Averaged spreadsheet, delta = (end-start)/count */
  static const int  HOW_OLD_AVERAGE = 2;

  /** Plot-binned, channel by channel, 'count' bins */
  static const int HOW_PLOTBIN  = 3;

  /** Linear interpolation spreadsheet, delta = (end-start)/count */
  static const int HOW_LINEAR = 4;

  /** Averaged spreadsheet, delta = count */
  static const int HOW_AVERAGE = 5;

  /** Meta type for the enum values */
  static const int META_TYPE_ENUM  = 0;

  /** Meta type for the numeric values */  
  static const int META_TYPE_NUMERIC = 1;

  // XML-RPC does not define fault codes.
  // The xml-rpc-c library uses -500, -501, ... (up to -510)

  /** 'serv' fault code */
  static const int ARCH_DAT_SERV_FAULT   = -600;

  /** 'no index' fault code */
  static const int ARCH_DAT_NO_INDEX     = -601;  
 
  /** 'arg error' fault code */     
  static const int ARCH_DAT_ARG_ERROR    = -602;

  /** 'data error' fault code */   
  static const int ARCH_DAT_DATA_ERROR   = -603;

 public:

  /** returns the command name */
  virtual const char* getName() const = 0;

  /** creates the request structure of this command  */
  virtual PVStructurePtr createRequest() = 0;

  /** initializes internal structures of this command based on the request*/
  virtual void setRequest(PVStructurePtr const & request) = 0;

  /** returns the introspection of the result structure */
  virtual  epics::pvData::StructureConstPtr createResultType() = 0;

  /** factory method */
  virtual ArchiveCommandPtr createCommand() = 0;

 protected:

  /** creates the Meta type structure according to info->getType (CtrlInfo::Enumerated or CtrlInfo::Numeric) */
  epics::pvData::StructureConstPtr createMetaType(const CtrlInfo* info);
  
  /** sets the Meta structure from CtrlInfo */
  void setMeta(PVStructurePtr& meta, const CtrlInfo* info);

 protected:

  /** creates dbr-time-value type, {stat(int), sevr(int), secs(int), nano(int), value(array of pv_type),
      min (double), max(double)} */
  epics::pvData::StructureConstPtr 
    createDbrTimeValueType(epics::pvData::ScalarType pv_type);

 protected:

  /** given a raw sample dbr_type/count/time/data, map it to the pvData::ScalarType and pv_count */
  void dbr_type_to_pv_type(DbrType dbr_type, 
			   DbrCount dbr_count,
			   epics::pvData::ScalarType& pv_type, 
			   int& pv_count);
  /** given a raw sample dbr_type/count/time/data, convert it into the PvData structure */
  void encode_value(DbrType dbr_type, 
			   DbrCount dbr_count,
			   const epicsTime& time, 
			   const RawValue::Data* data,
			   epics::pvData::ScalarType pv_type, 
			   int pv_count,
			   epics::pvData::PVStructurePtr& values,
			   bool with_min_max = false,
			   double minimum = 0.0,
			   double maximum = 0.0);


};

}}


#endif
