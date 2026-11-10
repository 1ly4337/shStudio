#pragma once
#include <windows.h>

LRESULT setWindowDouble(HWND hWnd, const wchar_t* format, double value);

LRESULT CALLBACK noEraseSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
                                     DWORD_PTR refData);

bool focusTabStop(HWND hWnd);
char tabToNextSibling(HWND from);

LRESULT CALLBACK tabSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
                                 DWORD_PTR refData);
LRESULT CALLBACK integerEditSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
                                         DWORD_PTR refData);
LRESULT CALLBACK decimalEditSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
                                         DWORD_PTR refData);
