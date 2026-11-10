#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include "Time.hpp"
#include "_imports_p09.h"
#include "_imports_g5.h"

Time::Time(int )
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    m_hour = (BYTE)st.wHour;
    m_year = st.wYear;
    m_minute = (BYTE)st.wMinute;
    m_month = (BYTE)st.wMonth;
    m_second = (BYTE)st.wSecond;
    m_day = (BYTE)st.wDay;
    m_tzOffset = 0;
    m_fullYear = true;
    m_valid = true;
}

Time& Time::operator=(const SYSTEMTIME& st)
{
    m_year = st.wYear;
    m_month = (BYTE)st.wMonth;
    m_day = (BYTE)st.wDay;
    m_hour = (BYTE)st.wHour;
    m_minute = (BYTE)st.wMinute;
    m_second = (BYTE)st.wSecond;
    m_tzOffset = 0;
    m_fullYear = true;
    m_valid = true;
    return *this;
}

Time& Time::operator=(const FILETIME* fileTime)
{
    FILETIME localTime;
    SYSTEMTIME st;
    FileTimeToLocalFileTime(fileTime, &localTime);
    memset(&st, 0, sizeof(st));
    FileTimeToSystemTime(&localTime, &st);
    m_hour = (BYTE)st.wHour;
    m_year = st.wYear;
    m_minute = (BYTE)st.wMinute;
    m_month = (BYTE)st.wMonth;
    m_second = (BYTE)st.wSecond;
    m_day = (BYTE)st.wDay;
    m_tzOffset = 0;
    m_fullYear = true;
    m_valid = true;
    return *this;
}

int Time::compare(const Time& other) const
{
    if (!m_valid)
        return -(other.m_valid != 0);
    if (!other.m_valid)
        return 1;
    if (m_year > other.m_year)
        return 1;
    if (m_year < other.m_year)
        return -1;
    if (m_month > other.m_month)
        return 1;
    if (m_month < other.m_month)
        return -1;
    if (m_day > other.m_day)
        return 1;
    if (m_day < other.m_day)
        return -1;
    if (m_hour > other.m_hour)
        return 1;
    if (m_hour < other.m_hour)
        return -1;
    if (m_minute > other.m_minute)
        return 1;
    if (m_minute < other.m_minute)
        return -1;
    if (m_second > other.m_second)
        return 1;
    return -(m_second < other.m_second);
}

String Time::toString(wchar_t dateTimeSep, wchar_t timeSep) const
{
    wchar_t buffer[32];
    swprintf_s(buffer, 32, L"%04d-%02d-%02d%c%02d%c%02d%c%02d",
               m_year, m_month, m_day, dateTimeSep, m_hour, timeSep, m_minute, timeSep, m_second);
    return String(buffer, (unsigned)-1);
}

String Time::toAsn1() const
{
    short offset = m_tzOffset;
    wchar_t buffer[32];
    if (m_fullYear)
    {
        if (offset)
            swprintf_s(buffer, 32, L"%04d%02d%02d%02d%02d%02d%c%02d%02d",
                       m_year, m_month, m_day, m_hour, m_minute, m_second,
                       2 * (offset < 0) + '+', abs(offset / 60), abs(offset % 60));
        else
            swprintf_s(buffer, 32, L"%04d%02d%02d%02d%02d%02dZ",
                       m_year, m_month, m_day, m_hour, m_minute, m_second);
    }
    else if (offset)
    {
        swprintf_s(buffer, 32, L"%02d%02d%02d%02d%02d%02d%c%02d%02d",
                   m_year % 100, m_month, m_day, m_hour, m_minute, m_second,
                   2 * (offset < 0) + '+', abs(offset / 60), abs(offset % 60));
    }
    else
    {
        swprintf_s(buffer, 32, L"%02d%02d%02d%02d%02d%02dZ",
                   m_year % 100, m_month, m_day, m_hour, m_minute, m_second);
    }
    return String(buffer, (unsigned)-1);
}

String Time::format()
{
    SYSTEMTIME date;
    date.wYear = m_year;
    date.wMonth = m_month;
    date.wDayOfWeek = 0;
    date.wDay = m_day;
    date.wHour = m_hour;
    date.wMinute = m_minute;
    date.wSecond = m_second;
    date.wMilliseconds = 0;

    String result;
    int count = GetDateFormatW(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &date, 0, m_text, 64);
    if (!count)
        count = GetDateFormatW(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &date, 0, m_text, 64);
    if (count >= 0)
    {
        m_text[count] = 0;
        result << m_text << L' ';
    }
    count = GetTimeFormatW(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &date, 0, m_text, 64);
    if (!count)
        count = GetTimeFormatW(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &date, 0, m_text, 64);
    if (count >= 0)
    {
        m_text[count] = 0;
        result << m_text;
    }
    return result;
}
