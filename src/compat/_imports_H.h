#pragma once
#include <windows.h>
#include <commctrl.h>

class String;

#include "gdi_cache.h"
extern LRESULT CALLBACK tabSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR refData);
extern LRESULT CALLBACK noEraseSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR refData);

class StudioFrame;
extern StudioFrame* g_studioFrame;
