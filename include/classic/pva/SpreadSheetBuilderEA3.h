#ifndef EA4_SPREAD_SHEET_BUILDER_EA3_H
#define EA4_SPREAD_SHEET_BUILDER_EA3_H

#include <memory>
#include <vector>

#include "pva/SpreadSheetBuilder.h"
#include "storage/ReaderFactory.h"

namespace ea4 { namespace pva {

class SpreadSheetBuilderEA3 : public SpreadSheetBuilder {

public:

  RawValuesBuilderEA3(DataReader* reader, 
		      const std::string& name,
		      const epicsTime& startTime,
		      const epicsTime& endTime,
		      int count);

 public:

  // 1. findFirstEntry
  // 2. getResultType
  // 3. getResult

  virtual void findFirstEntry();

  virtual void getResult(epics::pvData::PVStructurePtr& result);

 protected:

  template <class value_type, class dbr_type, class pvarray_type> 
    void getDBRs(PVStructurePtr& pvResult);

 protected:

  DataReader* reader;
 
  const RawValue::Data* data;

};

  }}

#include "RawValuesBuilderEA3.icpp"

#endif
