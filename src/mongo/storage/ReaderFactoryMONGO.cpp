// ReaderFactory.cpp

// Storage
#include "storage/ReaderFactoryMONGO.h"
#include "storage/RawDataReaderMONGO.h"
// #include "storage/PlotReaderMONGO.h"
// #include "storage/AverageReaderMONGO.h"
// #include "storage/LinearReaderMONGO.h"

using namespace std;

namespace ea4 { namespace storage {

const char* ReaderFactoryMONGO::toString(How how, double delta) {

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
ReaderFactoryMONGO::create(const string& archive_name, How how, double delta) {
  DataReader* dataReader = 0;
    try {        
      if (how == Raw  ||  delta <= 0.0){
	dataReader = new RawDataReaderMONGO(archive_name);
      }
    } catch (...) {
        throw GenericException(__FILE__, __LINE__, 
			       "Cannot create reader for %s",
                               toString(how, delta));
    }

    return dataReader;
}

/*
DataReader* 
ReaderFactoryMONGO::create(const string& archive_name, How how, double delta) {
    try {        
      if (how == Raw  ||  delta <= 0.0){
	return new RawDataReaderMONGO(archive_name);
      } else if (how == Plotbin){
        return new PlotReaderMONGO(archive_name, delta);
      } else if (how == Average) {
        return new AverageReaderMONGO(archive_name, delta);
      } else {
	return new LinearReaderMONGO(archive_name, delta);
      }
    }
    catch (...) {
        throw GenericException(__FILE__, __LINE__, 
			       "Cannot create reader for %s",
                               toString(how, delta));
    }
}

*/

}}

