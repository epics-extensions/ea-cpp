// RegularExpression.cpp: implementation of the RegularExpression class.
//////////////////////////////////////////////////////////////////////

#include <iostream>

// System
#include <ctype.h>

// Tools
#include "tools/RegularExpression.h"
#include "tools/MsgLogger.h"
#include "tools/GenericException.h"

extern "C"
{
#ifdef WIN32
#include"gnu_regex.h"
#else
#include <regex.h>
#endif
}

stdString RegularExpression::fromGlobPattern(const stdString &glob)
{
    stdString pattern;
    size_t len = glob.length();

    pattern = "^"; // start anchor
    for (size_t i=0; i<len; ++i)
    {
        switch (glob[i])
        {
        case '*':
            pattern += ".*";
            break;
        case '?':
            pattern += '.';
            break;
        default:
            pattern += '[';   // 'x' -> "[xX]"
            pattern += tolower(glob[i]);
            pattern += toupper(glob[i]);
            pattern += ']';
        }
    }
    pattern += '$'; // end anchor
    
    return pattern;
}

// RegularExpression could have private members
//  regex_t compiled_pattern;
//  bool    is_pattern_valid;
//
// But then RegularExpression.h would have to include <regex.h>
// which is quite big and does not contain extern "C".
// Using a void pointer is also nasty but keeps
// RegularExpression.h minimal.

void RegularExpression::set(const char *pattern, bool case_sensitive) {

    _pattern = pattern;
    _case_sensitive = case_sensitive;

    try {
        _compiled_pattern = new regex_t;
    }
    catch (...) {
        _compiled_pattern = 0;
    }

    if (! _compiled_pattern){
        throw GenericException(__FILE__, __LINE__,
                               "Cannot allocate regular expression");
    }

    int flags = REG_EXTENDED | REG_NOSUB;

    if (! case_sensitive){
        flags |= REG_ICASE;
    }

    int rv = regcomp((regex_t *) _compiled_pattern, pattern, flags);

    char erbuf[100];
    if (rv != 0) {

      regerror(rv, (regex_t *) _compiled_pattern, erbuf, 100);
      // std::cout << "regcomp failed with"  << erbuf << std::endl;

      delete ((regex_t *)_compiled_pattern);
      _compiled_pattern = 0;
      throw GenericException(__FILE__, __LINE__,
	"Cannot compile regular expression '%s' '%s'", pattern, erbuf);
    }
}

RegularExpression::~RegularExpression()
{
    if (_compiled_pattern) {
        regfree((regex_t *) _compiled_pattern);
        delete ((regex_t *) _compiled_pattern);
        _compiled_pattern = 0;
    }
}

bool RegularExpression::doesMatch(const char *input)
{
    return regexec((regex_t *) _compiled_pattern, 
		   input,
                   /*nmatch*/ 0, 
		   /*pmatch[]*/ 0, 
		   /*eflags*/ 0) == 0;
}


