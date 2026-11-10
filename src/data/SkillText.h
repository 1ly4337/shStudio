#pragma once
#include <windows.h>

class String;

extern wchar_t g_skillTypeTextBuf[128];

const wchar_t* getSkillTypeName(short typeDetail);

int swprintf128(wchar_t* buffer, const wchar_t* format, ...);

String& appendTabbedTriple(String& s, const unsigned char* v);

inline wchar_t skillExportSeparator(int tag)
{
    if (tag <= 0x64756D70 )
    {
        if (tag == 0x64756D70  || tag == 0x2D2D2D2D  || tag == 0x63737620 )
            return L';';
        return L',';
    }
    if (tag == 0x73716C20 )
        return L',';
    if (tag == 0x74616273 )
        return L'\t';
    return L',';
}
