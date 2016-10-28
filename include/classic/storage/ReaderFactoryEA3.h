// -*- c++ -*-

#ifndef _EA4_READER_FACTORY_EA3_H__
#define _EA4_READER_FACTORY_EA3_H__

#include <tr1/memory>

// tools
#include "tools/ToolsConfig.h"

// storage
#include "storage/Index.h"
#include "storage/DataReader.h"

typedef std::tr1::shared_ptr<Index> IndexPtr;

namespace ea4 { namespace storage {

/// \addtogroup Storage
/// @{

/// Create one of the DataReader class instances.
class ReaderFactoryEA3 {

public:

    /// Determine what DataReader to use:
    enum How {
        Raw,     ///< Use RawDataReader
        Plotbin, ///< Use PlotReader
        Average, ///< Use AverageReader
        Linear   ///< Use LinearReader
    };

    /// String representation of how/delta.
    ///
    /// The result is suitable for display ala
    /// "Method: ...".
    /// The result is held in a static char buffer,
    /// beware of other threads calling toString.
    static const char *toString(How how, double delta);
    
    /// Create a DataReader.
    static DataReader* create(IndexPtr index, How how, double delta);
 
};

}}

/// @}

#endif
