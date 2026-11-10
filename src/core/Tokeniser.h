#pragma once
#include <windows.h>

class Tokeniser
{
public:
    Tokeniser(char separator = ',', bool quotes = false)
        : m_text(0), m_current(-1), m_length(0), m_count(0), m_separator(separator), m_quotes(quotes) {}
    virtual ~Tokeniser()
    {
        if (m_text)
        {
            ::operator delete[](m_text);
            m_text = 0;
        }
    }
    bool set(const wchar_t* text);

    wchar_t* m_text;
    int      m_current;
    int      m_length;
    int      m_count;
    char     m_separator;
    bool     m_quotes;
};
