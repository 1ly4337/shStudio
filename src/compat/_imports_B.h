#pragma once
#include <windows.h>
#include <commctrl.h>
#include "ida_compat.h"

class String;

#define P08B_VFN(obj, off, RET, ...) ((RET(__thiscall*)(void*, ##__VA_ARGS__))((*(void***)(obj))[(off) / 4]))

#include "gdi_cache.h"

extern LRESULT CALLBACK tabSubclassProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
extern LRESULT CALLBACK noEraseSubclassProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
class IniFile;

class StudioFrame;
extern StudioFrame* g_studioFrame;
#include "Preferences.h"
