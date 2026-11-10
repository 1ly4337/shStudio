#pragma once
#include <windows.h>

class Exception
{
public:
    Exception() : m_code(0), m_message(0) {}
    Exception(const Exception& other);
    virtual ~Exception();

    void format(const wchar_t* fmt, ...);

    int      m_code;
    wchar_t* m_message;
};
static_assert(sizeof(Exception) == 0x0C, "Exception size");

extern wchar_t g_exceptionText[0x200];

class OutOfMemoryException : public Exception
{
public:
    OutOfMemoryException() {}
    OutOfMemoryException(long requested, const wchar_t* who)
    {
        format(L"OutOfMemoryException: %li requested by %s", requested, who);
    }
};

class NullPointerException : public Exception
{
public:
    NullPointerException() {}
    explicit NullPointerException(const wchar_t* where)
    {
        format(L"NullPointerException: in %li", where);
    }
};

class IllegalArgumentException : public Exception
{
public:
    IllegalArgumentException() {}
    IllegalArgumentException(int paramIndex, const wchar_t* method)
    {
        format(L"IllegalArgumentException: parameter #%i of %s is invalid", paramIndex, method);
    }
    explicit IllegalArgumentException(const wchar_t* method)
    {
        format(L"IllegalArgumentException: %s invoked with invalid param(s)", method);
    }
};

class ArrayIndexOutOfBoundsException : public Exception
{
public:
    ArrayIndexOutOfBoundsException() {}
    ArrayIndexOutOfBoundsException(long index, long count);
};
