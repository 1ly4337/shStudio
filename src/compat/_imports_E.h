#pragma once
#include <windows.h>
#include "ida_compat.h"

class String;
class StudioFrame;
class StandardNPC;
class StandardNPCs;
class NpcData;

extern StudioFrame* g_studioFrame;

#include "FileDialogs.h"

extern LRESULT CALLBACK noEraseSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR refData);
