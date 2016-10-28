// ReaderFactoryEA3.cpp

// Storage
#include "storage/ReaderFactoryEA3.h"
#include "storage/RawDataReaderEA3.h"
#include "storage/ShallowIndexRawDataReaderEA3.h"

#include "pva/ArchiveRPCCommand.h"

using namespace std;
using namespace ea4::pva;

namespace ea4 { namespace storage {

const char* ReaderFactoryEA3::toString(How how, double delta) {

    static char buf[100];
    switch (how)
    {
        case Raw:
            return "Raw Data";
        case Plotbin:
            sprintf(buf, "Plot-Binning, %g sec bins", delta);
            return buf;
        case Average:
            sprintf(buf, "Averaging every %g seconds", delta);
            return buf;
        case Linear:
            sprintf(buf, "Linear Interpolation every %g seconds", delta);
            return buf;
        default:
            return "<unknown>";
    };  
}

DataReader* 
ReaderFactoryEA3::create(IndexPtr index, How how, double delta) {

   DataReader* dataReader;

   try {
     dataReader = new ShallowIndexRawDataReaderEA3(index);
   } catch (...) {
        throw GenericException(__FILE__, __LINE__, 
			       "Cannot create reader for %s",
                               toString(how, delta));
   }

   /*
   try
    {
      if (rfHow == Raw  ||  delta <= 0.0){
	return new ShallowIndexRawDataReaderEA3(index);
      } else if (how == Plotbin){
	return new PlotReader(index, delta);
      } else if (how == Average){
	return new AverageReader(index, delta);
      } else {
	return new LinearReader(index, delta);
      }
    } catch (...) {
        throw GenericException(__FILE__, __LINE__, "Cannot create reader for %s",
                               toString(how, delta));
    }
   */

   return dataReader;
}

}}

