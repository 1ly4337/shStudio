#include "Tokeniser.h"
#include <cstring>
#include <cwchar>
#include <new>

bool Tokeniser::set(const wchar_t* text)
{
    if (!text)
        return false;
    if (m_text)
    {
        ::operator delete[](m_text);
        m_text = 0;
    }
    m_count = 0;
    m_current = -1;
    unsigned length = (unsigned)wcslen(text);
    m_length = (int)length;
    m_text = (wchar_t*)::operator new[](((unsigned __int64)(length + 1) >> 31) != 0 ? (size_t)-1 : 2 * (length + 1));
    memcpy(m_text, text, 2 * m_length);
    m_text[m_length] = 0;
    bool inQuotes = false;
    for (int i = 0; i < m_length; ++i)
    {
        wchar_t* p = &m_text[i];
        if ((int)(unsigned short)*p == (int)m_separator && (!inQuotes || !m_quotes))
        {
            ++m_count;
            *p = 0;
        }
        if (m_text[i] == L'"')
            inQuotes = !inQuotes;
    }
    if (m_length > 0 && (int)(unsigned short)m_text[m_length - 1] != (int)m_separator)
        ++m_count;
    return true;
}
