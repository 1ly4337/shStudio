#pragma once
#include <windows.h>
#include "ida_compat.h"

class String;

class StudioFrame;
extern StudioFrame* g_studioFrame;

#ifndef P08C2_VCALL
#define P08C2_VCALL(obj, off, RET, ...) \
    ((RET(__thiscall*)(void*, ##__VA_ARGS__))((*(void***)(obj))[(off) / 4]))
#endif
