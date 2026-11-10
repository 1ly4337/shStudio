#pragma once
#include <windows.h>
#include "ida_compat.h"

#define P10_VCALL(obj, off, RET, ...) \
    ((RET(__thiscall*)(void*, ##__VA_ARGS__))((*(void***)(obj))[(off) / 4]))
