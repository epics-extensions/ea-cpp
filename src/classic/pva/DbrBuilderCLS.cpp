#include "pva/DbrBuilderCLS.h"

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;
using namespace epics::pvAccess;

namespace ea4 { namespace pva {

DbrBuilderCLS::DbrBuilderCLS(DataReaderPtr r, 
			     const string& name,
			     epicsTimePtr start, 
			     epicsTimePtr end, 
			     int count) 
  : RawValuesBuilder(name, start, end, count), reader(r) {
}

void DbrBuilderCLS::findFirstEntry(){

#ifdef LOGFILE
  printf("DbrBuilderCLS::findFirstEntry()\n");
  LOG_MSG("Handling '%s'\n", name.c_str());
#endif

  // std::string txt;
  // std::cout << "createDataResult" << std::endl;
  // printf("start: %s\n", epicsTimeTxt(start, txt));

  data = reader->find(name, startTime.get());
  setResultType(reader->getInfo(), reader->getType(), reader->getCount());
}

int  DbrBuilderCLS::getResult(PVStructurePtr& pvResult){
  int num_values = 0;
    switch(pvType){
      // DBR_TIME_CHAR, DBR_TIME_SHORT, DBR_TIME_ENUM, DBR_TIME_LONG
    case pvInt:
      num_values = getDBRs<int, dbr_time_long, PVIntArray>(pvResult);
      break;
      // DBR_TIME_FLOAT, DBR_TIME_DOUBLE
    case pvDouble:
      num_values = getDBRs<double, dbr_time_double, PVDoubleArray>(pvResult);
      break;
      // DBR_TIME_STRING
    case pvString:
      num_values = getDBRs<string, dbr_time_string, PVStringArray>(pvResult);
      break;
    default:
      LOG_MSG("wrong type '%d'\n", pvType);
    }

    return num_values;
}

}}
