#include "Exception.h"
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cwchar>

wchar_t g_exceptionText[0x200];

Exception::Exception(const Exception& other)
{
    m_code = other.m_code;
    if (other.m_message)
        m_message = _wcsdup(other.m_message);
    else
        m_message = 0;
}

Exception::~Exception()
{
    if (m_message)
        free(m_message);
}

void Exception::format(const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    if (_vsnwprintf_s(g_exceptionText, 0x200, 0x200, fmt, args))
    {
        if (m_message)
            free(m_message);
        m_message = _wcsdup(g_exceptionText);
    }
    va_end(args);
}

ArrayIndexOutOfBoundsException::ArrayIndexOutOfBoundsException(long index, long count)
{
    format(L"ArrayIndexOutOfBoundsException %li out of %li", index, count);
}
