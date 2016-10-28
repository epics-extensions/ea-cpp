#ifndef EA4_RAW_VALUES_BUILDER_H
#define EA4_RAW_VALUES_BUILDER_H

#include <memory>
#include <vector>

#include "pva/ArchiveRPCCommand.h"

namespace ea4 { namespace pva {

/** Basis class of the pvData (structure of arrays) adapters of 
    the raw value (dbr) readers */
class RawValuesBuilder {

 public:

  /** constructor */
  RawValuesBuilder(const std::string& name,
		   epicsTimePtr startTime,
		   epicsTimePtr endTime,
		   int count);

 public:

  /** selects the first entry */
  virtual void findFirstEntry() = 0;

  /** returns the result type (based on the first entry) */
  inline  epics::pvData::StructureConstPtr getResultType();

  /** returns results */
  virtual int getResult(epics::pvData::PVStructurePtr& result) = 0;

 protected:

  /** defines the pvData result type from meta data */
  void setResultType(const CtrlInfo& info,  DbrType dbrType,  DbrCount dbrCount);

  /** defines pvType and pvCount */
  void setPvType(DbrType dbrType,  DbrCount dbrCount);

  /** creates the pvData meta type structure (based on CtrlInfo->getType) */
  epics::pvData::StructureConstPtr createMetaType();

  /** create the pvData DBR type structure (based on pvType) */
  epics::pvData::StructureConstPtr createDBRsType();

 protected:

  /** defines the meta data structure from CtrlInfo */
  void setMeta(PVStructurePtr& meta);

 protected:

  // input

  /** pv name */
  std::string name;

  /** start time */
  epicsTimePtr startTime;

  /** end time */
  epicsTimePtr endTime;

  /** number of elements */
  int count;

 protected:

  // result type based on the first entry

  /** ctrl info used for defining meta data */
  CtrlInfo info;

  /** pvType */
  epics::pvData::ScalarType pvType;

  /** pvCount */
  int pvCount;

  /** result type */
  epics::pvData::StructureConstPtr resultType;

protected:

  // associated data */

  /** vector of stats */
  std::tr1::shared_ptr< std::vector<short> > shared_stats;

  /** vector of sevrs */
  std::tr1::shared_ptr< std::vector<short> > shared_sevrs;

  /** vector of secs */
  std::tr1::shared_ptr< std::vector<int> >   shared_secs;

  /** vector of nanos */
  std::tr1::shared_ptr< std::vector<int> >   shared_nanos;

};

inline  epics::pvData::StructureConstPtr 
RawValuesBuilder::getResultType(){
  return resultType;
}

}}

#endif
