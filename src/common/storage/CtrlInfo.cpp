// epics
#include <cvtFast.h>

// tools
#include "tools/GenericException.h"
#include "tools/string2cp.h"
#include "tools/Conversions.h"

// storage
#include "storage/CtrlInfo.h"

CtrlInfo::CtrlInfo() {

    size_t additional_buffer = 10;

    _infobuf.reserve(sizeof(CtrlInfoData) + additional_buffer);
    CtrlInfoData *info = _infobuf.mem();
    info->type = Invalid;
    //sizeof (DbrCount) + sizeof(DbrType);
    info->size = 2 + 2;
}

CtrlInfo::CtrlInfo(const CtrlInfo &rhs)
{
    const CtrlInfoData *rhs_info = rhs._infobuf.mem();
    _infobuf.reserve(rhs_info->size);
    CtrlInfoData *info = _infobuf.mem();
    memcpy(info, rhs_info, rhs_info->size);
}

CtrlInfo & CtrlInfo::operator = (const CtrlInfo &rhs)
{
    const CtrlInfoData *rhs_info = rhs._infobuf.mem();
    _infobuf.reserve(rhs_info->size);
    CtrlInfoData *info = _infobuf.mem();
    memcpy(info, rhs_info, rhs_info->size);
    return *this;
}

bool CtrlInfo::operator == (const CtrlInfo &rhs) const
{
    const CtrlInfoData *rhs_info = rhs._infobuf.mem();
    const CtrlInfoData *info = _infobuf.mem();
    // memcmp over the full info should start by comparing
    // the size & type fields and work in any case,
    // but valgrind complained about use of uninitialized
    // memory, so now it's explicit:
    return info->size == rhs_info->size &&
        info->type == rhs_info->type &&
        memcmp(&info->value, &rhs_info->value,
               rhs_info->size - 2*sizeof(uint16_t)) == 0;
}

CtrlInfo::Type CtrlInfo::getType() const
{   return (CtrlInfo::Type) (_infobuf.mem()->type);}

int32_t CtrlInfo::getPrecision() const
{
    if (getType() == Numeric)
        return _infobuf.mem()->value.analog.prec;
    return 0;
}

const char *CtrlInfo::getUnits() const
{
    if (getType() == Numeric)
        return _infobuf.mem()->value.analog.units;
    return "";
}

float CtrlInfo::getDisplayHigh() const
{   return _infobuf.mem()->value.analog.disp_high; }

float CtrlInfo::getDisplayLow() const
{   return _infobuf.mem()->value.analog.disp_low; }

float CtrlInfo::getHighAlarm() const
{   return _infobuf.mem()->value.analog.high_alarm; }

float CtrlInfo::getHighWarning() const
{   return _infobuf.mem()->value.analog.high_warn; }

float CtrlInfo::getLowWarning() const
{   return _infobuf.mem()->value.analog.low_warn; }

float CtrlInfo::getLowAlarm() const
{   return _infobuf.mem()->value.analog.low_alarm; }

size_t CtrlInfo::getNumStates() const
{
    if (getType() == Enumerated)
        return _infobuf.mem()->value.index.num_states;
    return 0;
}

void CtrlInfo::setNumeric(
    int32_t prec, const stdString &units,
    float disp_low, float disp_high,
    float low_alarm, float low_warn, float high_warn, float high_alarm)
{
    size_t len = units.length();
    size_t size = sizeof(CtrlInfoData) + len;
    _infobuf.reserve(size);
    CtrlInfoData *info = _infobuf.mem();

    info->type = Numeric;
    info->size = size;
    info->value.analog.disp_high  = disp_high;
    info->value.analog.disp_low   = disp_low;
    info->value.analog.low_warn   = low_warn;
    info->value.analog.low_alarm  = low_alarm;
    info->value.analog.high_warn  = high_warn;
    info->value.analog.high_alarm = high_alarm;
    info->value.analog.prec       = prec;
    string2cp (info->value.analog.units, units, len+1);
}

void CtrlInfo::setEnumerated(size_t num_states, char *strings[])
{
    size_t i, len = 0;
    for (i=0; i<num_states; i++) // calling strlen twice for each string...
        len += strlen(strings[i]) + 1;

    allocEnumerated (num_states, len);
    for (i=0; i<num_states; i++)
        setEnumeratedString(i, strings[i]);
}

void CtrlInfo::allocEnumerated(size_t num_states, size_t string_len)
{
    // actually this is too big...
    size_t size = sizeof(CtrlInfoData) + string_len;
    _infobuf.reserve(size);
    CtrlInfoData *info = _infobuf.mem();

    info->type = Enumerated;
    info->size = size;
    info->value.index.num_states = num_states;
    char *enum_string = info->value.index.state_strings;
    *enum_string = '\0';
}

// Must be called after allocEnumerated()
// AND must be called in sequence,
// i.e. setEnumeratedString (0, ..
//      setEnumeratedString (1, ..
void CtrlInfo::setEnumeratedString(size_t state, const char *string)
{
    CtrlInfoData *info = _infobuf.mem();
    if (info->type != Enumerated  ||
        state >= (size_t)info->value.index.num_states)
        return;

    char *enum_string = info->value.index.state_strings;
    size_t i;
    for (i=0; i<state; i++) // find this state string...
        enum_string += strlen(enum_string) + 1;
    strcpy(enum_string, string);
}

// After allocEnumerated() and a sequence of setEnumeratedString ()
// calls, this method recalcs the total size
// and checks if the buffer is sufficient (Debug version only)
void CtrlInfo::calcEnumeratedSize ()
{
    size_t i, len, total=sizeof(CtrlInfoData);
    CtrlInfoData *info = _infobuf.mem();
    char *enum_string = info->value.index.state_strings;
    for (i=0; i<(size_t)info->value.index.num_states; i++)
    {
        len = strlen(enum_string) + 1;
        enum_string += len;
        total += len;
    }

    info->size = total;
    LOG_ASSERT(total <= _infobuf.capacity());
}

void CtrlInfo::formatDouble(double value, stdString &result) const
{
    if (getType() != Numeric)
    {
        result = "<enumerated>";
        return;
    }
    char buf[200];
    if (cvtDoubleToString(value, buf, getPrecision()) >= 200)
        result = "<too long>";
    else
        result = buf;
}

const char *CtrlInfo::getState(size_t state, size_t &len) const
{
    if (getType() != Enumerated)
        return 0;

    const CtrlInfoData *info = _infobuf.mem();
    const char *enum_string = info->value.index.state_strings;
    size_t i=0;

    do
    {
        len = strlen(enum_string);
        if (i == state)
            return enum_string;
        enum_string += len + 1;
        ++i;
    }
    while (i < (size_t)info->value.index.num_states);
    len = 0;
    
    return 0;
}

void CtrlInfo::getState(size_t state, stdString &result) const
{
    size_t len;
    const char *text = getState(state, len);
    if (text)
    {
        result.assign(text, len);
        return;
    }

    char buffer[80];
    sprintf(buffer, "<Undef: %u>", (unsigned int)state);
    result = buffer;
}

bool CtrlInfo::parseState(const char *text,
                         const char **next, size_t &state) const
{
    const char *state_text;
    size_t  i, len;

    for (i=0; i<getNumStates(); ++i)
    {
        state_text = getState(i, len);
        if (! state_text)
        {
            LOG_MSG("CtrlInfo::parseState: missing state %zu", i);
            return false;
        }
        if (!strncmp(text, state_text, len))
        {
            state = i;
            if (next)
                *next = text + len;
            return true;
        }
    }
    return false;
}


void CtrlInfo::show(FILE *f) const
{
    if (getType() == Numeric)
    {
        fprintf(f, "CtrlInfo: Numeric\n");
        fprintf(f, "Display : %g ... %g\n", getDisplayLow(), getDisplayHigh());
        fprintf(f, "Alarm   : %g ... %g\n", getLowAlarm(), getHighAlarm());
        fprintf(f, "Warning : %g ... %g\n", getLowWarning(), getHighWarning());
        fprintf(f, "Prec    : %ld '%s'\n", (long)getPrecision(), getUnits());
    }
    else if (getType() == Enumerated)
    {
        fprintf(f, "CtrlInfo: Enumerated\n");
        fprintf(f, "States:\n");
        size_t i, len;
        for (i=0; i<getNumStates(); ++i)
        {
            fprintf(f, "\tstate='%s'\n", getState(i, len));
        }
    }
    else
        fprintf(f, "CtrlInfo: Unknown\n");
}

