#pragma once
#include <windows.h>
#include "ida_compat.h"
#include <commctrl.h>

namespace Data { class Item; }
class RecordSet;

#define A_VCALL(obj, off, RET, ...) \
    ((RET(__thiscall*)(void*, ##__VA_ARGS__))((*(void***)(obj))[(off) / 4]))

#include "gdi_cache.h"
LRESULT CALLBACK noEraseSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR refData);

class StudioFrame;
extern StudioFrame* g_studioFrame;
