#pragma once
#include <windows.h>
#include "String.hpp"

class Time
{
public:
    explicit Time(int unused = 0);
    explicit Time(const SYSTEMTIME& st) { *this = st; }
    virtual ~Time() {}

    Time& operator=(const SYSTEMTIME& st);
    Time& operator=(const FILETIME* fileTime);
    int compare(const Time& other) const;
    String toString(wchar_t dateTimeSep = L' ', wchar_t timeSep = L':') const;
    String toAsn1() const;
    String format();

public:
    WORD  m_year;
    BYTE  m_month;
    BYTE  m_day;
    BYTE  m_hour;
    BYTE  m_minute;
    BYTE  m_second;
    short m_tzOffset;
    bool  m_fullYear;
    bool  m_valid;
    WCHAR m_text[64];
};
static_assert(sizeof(Time) == 0x90, "Time size");
