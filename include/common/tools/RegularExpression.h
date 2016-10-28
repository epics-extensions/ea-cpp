// -*- c++ -*-
#ifndef _REGULAREXPRESSION_H_
#define _REGULAREXPRESSION_H_

#include "tools/ToolsConfig.h"

/// \ingroup Tools 

/** Wrapper for Unix/GNU regex library. */
class RegularExpression   {

public:

  /// Create a regular expression for a "glob" pattern:
  /// question mark - any character
  /// star          - many characters
  /// case insensitive
  static std::string fromGlobPattern(const std::string& glob);

    /// Create RegularExpression with pattern for further matches.
    ///
    /// @exception GenericException on compilation error
    RegularExpression(const char *pattern, bool case_sensitive=true) {
        set(pattern, case_sensitive);
    }

  /// Copy constructor 
    RegularExpression(const RegularExpression& rhs) {
        set(rhs._pattern.c_str(), rhs._case_sensitive);
    }

  /// Destructor
    ~RegularExpression();

    /// Test if 'input' matches current pattern.
    ///
    /// Currently uses
    /// - EXTENDED regular expression
    /// - case sensitive
    /// - input must be anchored for full-string match,
    ///   otherwise substrings will match:
    ///     abc        matches        b
    ///     abc     does not match    $b^
    ///
    /// When no pattern was supplied, anything matches!
    bool doesMatch(const char *input);

  /// Check 
    bool doesMatch(const stdString &input) { 
      return doesMatch(input.c_str()); }
    
private:

  friend class ToRemoveGNUCompilerWarning;

  void set(const char *pattern, bool case_sensitive=true);

  // Use create/reference/unreference instead of new/delete:
  // intentionally not implemented
  RegularExpression & operator = (const RegularExpression &); 

  RegularExpression();

  std::string _pattern;
  bool    _case_sensitive;
  void    *_compiled_pattern;
  int     _refs;
};

#endif 
 
