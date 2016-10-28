// -*- c++ -*-

#ifndef __BENCHTIMER_H__
#define __BENCHTIMER_H__

// epics
#include <epicsTime.h>

#include <string>

/// \ingroup Tools

/** Start/stop type of timer. */
class BenchTimer
{
public:
    /// Constructor, also invokes start().
    BenchTimer()
    {   start(); }

    /// Start or re-start the timer.
    void start()
    {   t0 = epicsTime::getCurrent(); }

    /// Stop the timer, returns the seconds of runtime.
    double stop()
    {
        t1 = epicsTime::getCurrent();
        return runtime();
    }

    /// Runtime of the last start...stop cycle.
    double runtime()
    {   return t1 - t0; }

    /// Printable representation of runtime().
    std::string toString();
    
private:
    epicsTime t0, t1;
};

#endif
