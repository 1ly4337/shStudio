#pragma once
#include <windows.h>

class StudioFrame;

extern StudioFrame* g_studioFrame;

extern wchar_t* g_skillColumnNames[100];
extern DWORD    g_skillColumnTags[100];

#include "gdi_cache.h"

extern LRESULT CALLBACK tabSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR refData);
