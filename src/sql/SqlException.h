#pragma once
#include "Exception.h"

class SqlException : public Exception
{
public:
    SqlException() {}
    explicit SqlException(const wchar_t* fmt, ...);
};
static_assert(sizeof(SqlException) == 0x0C, "SqlException size");

class SqlCoercionException : public SqlException
{
public:
    explicit SqlCoercionException(const wchar_t* message) : SqlException(message) {}
};
static_assert(sizeof(SqlCoercionException) == 0x0C, "SqlCoercionException size");
