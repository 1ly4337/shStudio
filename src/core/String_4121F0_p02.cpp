#include "String.hpp"
#include <cwchar>

bool String::sub_4121F0(const wchar_t* text) const
{
    if (!text || !*text)
        return m_length != 0;
    int length = (int)wcslen(text);
    return length != m_length || _wcsnicmp(m_buffer, text, m_length) != 0;
}
