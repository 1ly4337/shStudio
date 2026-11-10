#pragma once
#include <windows.h>

static inline wchar_t w3_separatorFor(int fmt)
{
    if (fmt <= 0x64756D70 )
    {
        if (fmt == 0x64756D70  || fmt == 0x2D2D2D2D  || fmt == 0x63737620 )
            return L';';
        return L',';
    }
    if (fmt == 0x73716C20  || fmt != 0x74616273 )
        return L',';
    return L'\t';
}
