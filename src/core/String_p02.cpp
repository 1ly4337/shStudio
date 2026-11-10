#include "p02_common.h"

String operator+(const String& s, wchar_t c)
{
    String r(s);
    r << c;
    return r;
}

String operator+(const String& s, int n)
{
    String r(s);
    r.m_radix = s.m_radix;
    r << (long)n;
    return r;
}
