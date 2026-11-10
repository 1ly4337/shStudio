#pragma once
#include <windows.h>
#include "ida_compat.h"

class String;
class StudioFrame;
class Window;
namespace Data { class Item; class Items; class ItemsSet; }

#include "gdi_cache.h"
extern LRESULT CALLBACK tabSubclassProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
extern LRESULT CALLBACK decimalEditSubclassProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
extern bool openItemEditor(Window* owner, Data::Items* items, Data::Item* item);

extern StudioFrame* g_studioFrame;
