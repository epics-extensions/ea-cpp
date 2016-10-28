// -*- c++ -*-

#ifndef _EA4_READER_FACTORY_MONGO_H__
#define _EA4_READER_FACTORY_MONGO_H__

// Tools
#include "tools/ToolsConfig.h"

// Storage
#include "storage/DataReader.h"

namespace ea4 { namespace storage {

/// \addtogroup Storage
/// @{

/// Create one of the DataReader class instances.
class ReaderFactoryMONGO
{

public:

    /// Determine what DataReader to use:
    enum How
    {
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
    static DataReader* create(const std::string& archive_name, 
			      How how, double delta);
};

/// @}

  }}

#endif
