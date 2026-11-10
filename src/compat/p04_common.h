#pragma once
#include <windows.h>
#include <cwchar>
#include <cstring>
#include "ida_compat.h"
#include "_imports_p04.h"

#pragma warning(disable : 4066 4250 4498)

#define P04_VCALL(obj, off, RET, ...) \
    ((RET(__thiscall*)(void*, ##__VA_ARGS__))((*(void***)(obj))[(off) / 4]))

inline char* p04_streamBase(const void* stream)
{
    const char* p = (const char*)stream;
    int off = (*(const int* const*)p)[1];
    return (char*)p + off;
}
inline int p04_streamAvailable(const void* stream)
{
    char* b = p04_streamBase(stream);
    return *(int*)(b + 4) - *(int*)(b + 8);
}

inline wchar_t p04_separator(int format)
{
    char c;
    if (format > 'dump')
    {
        if (format != 'sql ' && format == 'tabs')
            c = 9;
        else
            c = ',';
    }
    else if (format == 'dump' || format == '----' || format == 'csv ')
        c = ';';
    else
        c = ',';
    return (wchar_t)(short)c;
}
