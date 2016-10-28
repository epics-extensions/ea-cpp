// epicsTimeHelper

#ifndef __EPICSTIMEHELPER_H__
#define __EPICSTIMEHELPER_H__

// epics
#include "epicsTime.h"

#include "tools/ToolsConfig.h"

/// \addtogroup Tools
/// \@{

/** Parse seconds from text, and format seconds into text.
 *  <p>
 *  Text may contain just the seconds, or "... seconds"
 *  or "... minutes", "... hours", "... days".
 *  Times may be abbreviated.
 */
class SecondParser
{
public:
    /** Parse text into number.
     *  <p>
     *
     *  @param t t
     *  @param n n
     *  @param default_factor Usually, we assume that a plain number without
     *                       suffix represents seconds.
     *                       A factor of 60*60, however, changes that default
     *                       interpretation to hours.
     *  @return true if OK. 
     */
    static bool parse(const stdString &t, double &n, double default_factor=1.0);

    /** @return Seconds formatted as string,
     *          using "... min" etc. when possible.
     */
    static stdString format(double seconds);
};

/// uninitialized (=0) class epicsTime.
extern const epicsTime nullTime;

/// Check if time is non-zero, whatever that could be.
bool isValidTime(const epicsTime &t);

/// Convert date/time string into epicsTime

/// Converts string "mm/dd/yyyy" or "mm/dd/yyyy 00:00:00" or
/// "mm/dd/yyyy 00:00:00.000000000" into epicsTime
/// Result: true for OK.
///
/// <B>Note</B>: Handles the special case of "N" where
/// N is an integer <=1000 and uses that as seconds.
/// This is useful for building test code.
bool string2epicsTime(const stdString &txt, epicsTime &time);

/// Convert epicsTime into "mm/dd/yyyy 00:00:00.000000000".

/// See note under string2epicsTime() for N<=1000.
///
///
bool epicsTime2string(const epicsTime &time, stdString &txt);

/// Similar to epicsTime2string().
const char *epicsTimeTxt(const epicsTime &time, stdString &txt);

/// Assemble/disassemble pieces.

/// year : 4-digit year like 2003
/// month: 1-12
/// day  : 1-31
/// hour : 0-23
/// min  : 0-59
/// sec  : 0-59
/// nano : 0- 999999999
/// (Note: 'man localtime' indicates that sec could be up to 61
///        to account for leap-seconds).
void epicsTime2vals(const epicsTime &time,
                    int &year, int &month, int &day,
                    int &hour, int &min, int &sec, unsigned long &nano);

/// Counterpart to epicsTime2vals().
void vals2epicsTime(int year, int month, int day,
                    int hour, int min, int sec, unsigned long nano,
                    epicsTime &time);

// Some of these are magic numbers because e.g.
// the number of seconds per month of course depends on the month.
#define secsPerMinute 60
#define secsPerHour   (60*60)
#define secsPerDay    (60*60*24)
#define secsPerMonth  (60*60*24*31)
#define secsPerYear   (60*60*24*31*365)

/// Find timestamp near 'time' which is a multiple of 'secs'.
epicsTime roundTime (const epicsTime &time, double secs);

/// Round up. (Rounding 5.5 by 0.5 will give 6.0!).
epicsTime roundTimeUp (const epicsTime &time, double secs);

/// Round down (Rounding 5.5 by 0.5 will give 5.5!).
epicsTime roundTimeDown (const epicsTime &time, double secs);

  void epicsTime2pieces(const epicsTime& t,
			int& secs, 
			int& nano);

  void pieces2epicsTime(int secs, 
			int nano, 
			epicsTime& t);



/// \@}

#endif //__EPICSTIMEHELPER_H__
