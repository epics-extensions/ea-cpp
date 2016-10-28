#ifndef EA4_INFO_QUERY_H
#define EA4_INFO_QUERY_H

// epics 3
#include <epicsTime.h>

// epics 4
#include "pv/pvData.h"
#include "pv/thread.h"

#include "storage/CtrlInfo.h"
#include "storage/RawValue.h"

#include "pva/Query.h"

namespace ea4 { namespace pva {

/** Basis class of the InfoQuery implementations */
class InfoQuery  : public Query {

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

};

}}


#endif
