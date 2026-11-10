#include <windows.h>
#include <cwchar>
#include "String.hpp"

bool String::equalsNoCase(const String& other) const
{
    return m_length == other.m_length && _wcsnicmp(m_buffer, other.m_buffer, m_length) == 0;
}

bool String::startsWith(const String& prefix) const
{
    const wchar_t* p = prefix.m_buffer;
    if (!p)
        return false;
    size_t len = wcslen(p);
    return len <= m_length && wcsncmp(m_buffer, p, len) == 0;
}
