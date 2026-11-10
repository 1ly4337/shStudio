#pragma once
#include <windows.h>
#include <commctrl.h>
#include <cwchar>
#include <cstring>
#include <cstdint>
#include "ida_compat.h"
#include "_imports_p05.h"

class StudioFrame;
extern StudioFrame* g_studioFrame;

#define VCALL(obj, off, RET, ...) \
    ((RET(__thiscall*)(void* , ##__VA_ARGS__))((*(void***)(obj))[(off) / 4]))

#pragma warning(disable : 4066 4498)

inline DWORD panelBackgroundColor()
{
    bool classic = g_classicAppearance <= 0;
    if (g_classicAppearance < 0) {
        char standard = isClassicAppearance();
        g_classicAppearance = standard != 0;
        classic = standard == 0;
    }
    if (classic)
        return 0xF0F0F0;
    return GetSysColor(COLOR_BTNFACE);
}
