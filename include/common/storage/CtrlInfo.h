// -*- c++ -*-

#ifndef __CTRLINFO_H__
#define __CTRLINFO_H__

#include <stdio.h>
#include <string>

#include "tools/MemoryBuffer.h"
#include "storage/StorageTypes.h"

/// \addtogroup Storage
/// @{

/** CtrlInfo for numeric values.
    So far, the structure's layout matches the binary
    layout (except for the byte order of the individual elements),
    so this strcuture must not be changed! */
class NumericInfo
{
public:

    float    disp_high;  ///<  high display range
    float    disp_low;   ///<  low display range 
    float    low_warn;   ///<  low warning  
    float    low_alarm;  ///<  low alarm  
    float    high_warn;  ///<  high warning 
    float    high_alarm; ///<  high alarm  
 
    int32_t  prec;       ///<  display precision   
    char     units[1];   ///< actually as long as needed,

};

/** CtrlInfo for enumerated channels */
class EnumeratedInfo
{
public:

  int16_t num_states;       ///< state_strings holds num_states strings 
  int16_t pad;              ///<  one after the other, separated by '\0'      
  char    state_strings[1]; ///<  state strings

};

/** A glorified union of NumericInfo and EnumeratedInfo.
   type == CtrlInfo::Type
   Info::size includes the "size" and "type" field.
   The original archiver read/wrote "Info" that way,
   but didn't properly initialize it:
   size excluded size/type and was then rounded up by 8 bytes... ?! 
*/
class CtrlInfoData {
public:

  uint16_t  size; ///< size
  uint16_t  type; ///< type

  union {
        NumericInfo     analog;
        EnumeratedInfo  index;
  } value; ///< value

    // class will be as long as necessary
    // to hold the units or all the state_strings
};

/** Meta-information for values: Units, limits, etc .

    A value is archived with control information.
    Several values might share the same control information
    for efficiency.
    The control information is variable in size
    because it holds units or state strings. */
class CtrlInfo {
public:

  /** Constructor */
  CtrlInfo();

  /** Copy constructor */
  CtrlInfo(const CtrlInfo& rhs);
    
  /** Copy operator */
  CtrlInfo& operator = (const CtrlInfo& rhs);

  /** 'Equal to' operator */
  bool operator == (const CtrlInfo& rhs) const;
    
  /** 'Not equal to' operator */
  bool operator != (const CtrlInfo& rhs) const
    {   return ! (*this == rhs);    }

  /// Type defines the type of value
  typedef enum {
        Invalid = 0,   ///< Undefined
        Numeric = 1,   ///< A numeric CtrlInfo: Limits, units, ...
        Enumerated = 2 ///< An enumerated CtrlInfo: Strings
  } Type;

    /// Get the Type for this CtrlInfo
    Type getType() const;

    // Read Control Information:
    // Numeric precision, units,
    // high/low limits for display etc.:

    /** Returns precision */
    int32_t getPrecision() const;

    /** Returns units */
    const char* getUnits() const;

    /** Returns display high */
    float getDisplayHigh() const;

    /** Returns display low */
    float getDisplayLow() const;

    /** Returns high alarm */
    float getHighAlarm() const;

    /** Returns high warning */
    float getHighWarning() const;

    /** Returns low warning */
    float getLowWarning() const;

    /** Returns low alarm */
    float getLowAlarm() const;

    /// Initialize a Numeric CtrlInfo
    /// (sets Type to Numeric and then sets fields)
    void setNumeric(int32_t prec, const std::string& units,
                    float disp_low, float disp_high,
                    float low_alarm, float low_warn,
                    float high_warn, float high_alarm);

    /// Initialize an Enumerated CtrlInfo
    void setEnumerated(size_t num_states, char *strings[]);

    /// Alternative to setEnumerated:
    /// Call with total string length, including all the '\0's !
    void allocEnumerated(size_t num_states, size_t string_len);

    /// Must be called after allocEnumerated()
    /// AND must be called in sequence,
    /// i.e. setEnumeratedString (0, ..
    ///      setEnumeratedString (1, ..
    void setEnumeratedString(size_t state, const char *string);

    /// After allocEnumerated() and a sequence of setEnumeratedString ()
    /// calls, this method recalcs the total size
    /// and checks if the buffer is sufficient (Debug version only)
    void calcEnumeratedSize();

    /// Format a double value according to precision<BR>
    void formatDouble(double value, std::string& result) const;

    /// Enumerated: state string
    size_t getNumStates() const;
    
    /// Get given state as text (also handles undefined states)
    void getState(size_t state, std::string& result) const;

    /// Like strtod, strtol: try to parse,
    /// position 'next' on character following the recognized state text
    bool parseState(const char *text, const char **next, size_t &state) const;

    /// Read a CtrlInfo from a binary data file.
    /// @exception GenericExeption on error.
    //    void read(class DataFile *datafile, FileOffset offset);

    /// Write a CtrlInfo to a binary data file
    /// @exception GenericExeption on error.
    //    void write(class DataFile *datafile, FileOffset offset) const;

    /** Prints data */
    void show(FILE *f) const;

    /** Returns size */
    size_t getSize() const
    {   return _infobuf.mem()->size; }

  /** Return a const memory buffer */
  const MemoryBuffer<CtrlInfoData>& getMemoryBuffer() const {
    return  _infobuf;
  }

  /** Return a const memory buffer */
  MemoryBuffer<CtrlInfoData>& getMemoryBuffer() {
    return  _infobuf;
  }
    
protected:

    /** Returns state */
    const char *getState(size_t state, size_t &len) const;

    /** Buffer */
    MemoryBuffer<CtrlInfoData>  _infobuf;
};

/// @}

 
#endif
