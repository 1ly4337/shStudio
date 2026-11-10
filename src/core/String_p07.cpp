#include "String.hpp"
#include "_imports_p07.h"
#include <cwchar>

bool String::differsNoCase(const String& other) const
{
    return m_length != other.m_length || _wcsnicmp(m_buffer, other.m_buffer, m_length) != 0;
}
