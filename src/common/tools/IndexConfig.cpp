// -*- c++ -*-

#include <cstring>

// tools
#include "tools/IndexConfig.h"
#include "tools/AutoFilePtr.h"
#include "tools/FUX.h"
#include "tools/GenericException.h"

bool IndexConfig::parse(const stdString &config_name)
{
    subarchives.clear();
    // Check if file opens at all
    AutoFilePtr f(config_name.c_str(), "rt");
    if (!f)
        throw GenericException(__FILE__, __LINE__,
              "Cannot open '%s'", config_name.c_str());
    // See if it's XML
    char line[5];
    bool read_ok = fread(line, 1, 5, f) == 5;
    f.close();
    if (!read_ok  ||  strncmp(line, "<?xml", 5))
        return false;
    // It is an XML file, so see if it's an indexconfig
    FUX fux;
    FUX::Element *e, *doc = fux.parse(config_name.c_str());
    if (!(doc && doc->getName() == "indexconfig"))
        throw GenericException(__FILE__, __LINE__,
                               "File '%s' is no XML indexconfig",
                               config_name.c_str());
    stdList<FUX::Element *>::const_iterator els;
    for (els=doc->getChildren().begin(); els!=doc->getChildren().end(); ++els)
        if ((e = (*els)->find("index")))
            subarchives.push_back(e->getValue());
    return true;
}

