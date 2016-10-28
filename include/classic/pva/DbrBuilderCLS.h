#ifndef EA4_DBR_BUILDER_CLS_H
#define EA4_DBR_BUILDER_CLS_H

#include <memory>
#include <vector>

#include "pva/RawValuesBuilder.h"
#include "storage/DataReader.h"

namespace ea4 { namespace pva {

class DbrBuilderCLS : public RawValuesBuilder {

public:

  DbrBuilderCLS(DataReaderPtr reader, 
		const std::string& name,
		epicsTimePtr startTime,
		epicsTimePtr endTime,
		int count);

 public:

  // 1. findFirstEntry
  // 2. getResultType
  // 3. getResult

  virtual void findFirstEntry();

  virtual int getResult(epics::pvData::PVStructurePtr& result);

 protected:

  template <class value_type, class dbr_type, class pvarray_type> 
    int getDBRs(PVStructurePtr& pvResult);

 protected:

  DataReaderPtr reader;
  const RawValue::Data* data;

};

}}

#include "DbrBuilderCLS.icpp"

#endif
