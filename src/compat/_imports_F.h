#pragma once
#include <windows.h>

class String;

extern String sub_41E180(const String& lhs, long value);
extern String sub_41E0C0(const String& lhs, wchar_t ch);

extern LRESULT CALLBACK tabSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR refData);
extern LRESULT CALLBACK noEraseSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR refData);
