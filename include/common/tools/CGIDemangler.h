// --------------------------------------------------------
// $Id: CGIDemangler.h,v 1.5 2006/06/05 17:01:43 kasemir Exp $
//
// Please refer to NOTICE.txt,
// included as part of this distribution,
// for legal information.
//
// Kay-Uwe Kasemir, kasemir@lanl.gov
// --------------------------------------------------------

#ifndef CGI_DEMANGLER_H
#define CGI_DEMANGLER_H

// tools
#include "tools/ToolsConfig.h"
#include "tools/NoCopy.h"

/// \ingroup Tools 

/// De-mangle CGI-type input.
/// For example, the QUERY string for CGI scripts
/// or the GET/POST text that a web server
/// receives.
///
/// All variables (name/value pairs) are placed in a std::map.
///
/// For a class that actually reads cin or QUERY_STRING,
/// see CGIInput.
class CGIDemangler
{
public:
    CGIDemangler() {}

    /// Reduce any '%%xx' (percent sign,...) escape sequences to the characters they represent.
    /// Changes the text! The resulting text may be shorter than the original.
    static void unescape(char *text);

    /// Reduce any '%%xx' (percent sign,...) escape sequences to the characters they represent.
    /// Creates an internal copy of the text for processing.
    /// @exception GenericException when the copy fails.
    static void unescape(stdString &text);

    /// Demangle string input
    /// Creates an internal copy of the text for processing.
    /// @exception GenericException when the copy fails.
    void parse(const char *input);

    /// Manually add another name/value to map
    void add(const stdString &name, const stdString &value);
    
    /// Query map
    stdString find(const stdString &name) const;

    /// Retrieve full map
    const stdMap<stdString, stdString> &getVars () const
    {   return _vars;   }

private:
    PROHIBIT_DEFAULT_COPY(CGIDemangler);
    stdMap<stdString, stdString>    _vars;

    void analyseVar(char *var);
};

#endif


