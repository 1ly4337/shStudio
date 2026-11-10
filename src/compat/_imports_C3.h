#pragma once
#include <windows.h>
#include "ida_compat.h"

class String;
class RecordSet;

#define C3_VCALL(obj, off, RET, ...) \
    ((RET(__thiscall*)(void*, ##__VA_ARGS__))((*(void***)(obj))[(off) / 4]))

class StudioFrame;
extern StudioFrame* g_studioFrame;
class ContentProvider; extern ContentProvider* g_contentProvider;
extern LPCWSTR off_59A770[];

extern String sub_41E180(const String& s, long v);
class MonsterModels;
