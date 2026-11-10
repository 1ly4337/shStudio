#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include <cstdio>
#include <cstring>
#include "CtrlHelpers.h"
#include "_imports_p09.h"
#include "_imports_g6.h"

LRESULT setWindowDouble(HWND hWnd, const wchar_t* format, double value)
{
    wchar_t buffer[34];
    if (!format)
    {
        if (value < 1.0)
        {
            swprintf_s(buffer, 0x20, L"%.4f", value);
            return SendMessageW(hWnd, WM_SETTEXT, 0, (LPARAM)buffer);
        }
        if (value < 100.0)
        {
            swprintf_s(buffer, 0x20, L"%.3f", value);
            return SendMessageW(hWnd, WM_SETTEXT, 0, (LPARAM)buffer);
        }
        format = L"%.2f";
        if (value >= 1000.0)
            format = L"%.1f";
    }
    swprintf_s(buffer, 0x20, format, value);
    return SendMessageW(hWnd, WM_SETTEXT, 0, (LPARAM)buffer);
}

LRESULT CALLBACK noEraseSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
                                     DWORD_PTR refData)
{
    if (refData && msg == WM_ERASEBKGND)
        return 0;
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

bool focusTabStop(HWND hWnd)
{
    if (!hWnd || (GetWindowLongW(hWnd, GWL_STYLE) & WS_TABSTOP) == 0 || !IsWindowEnabled(hWnd))
        return false;
    char className[16];
    memset(className, 0, sizeof(className));
    GetClassNameA(hWnd, className, 16);
    *(DWORD*)className &= 0xDFDFDFDF;
    if (*(DWORD*)className == 0x54494445)
        SendMessageW(hWnd, EM_SETSEL, 0, -1);
    if (*(DWORD*)className == 0x48434952)
    {
        CHARRANGE range;
        range.cpMin = 0;
        range.cpMax = -1;
        SendMessageW(hWnd, EM_EXSETSEL, 0, (LPARAM)&range);
    }
    SetFocus(hWnd);
    return true;
}

char tabToNextSibling(HWND from)
{
    SHORT shift = GetKeyState(VK_LSHIFT);
    HWND parent = GetParent(from);
    if (shift < 0)
    {
        HWND previous = 0;
        for (HWND w = FindWindowExW(parent, from, 0, 0); w != from; w = FindWindowExW(parent, w, 0, 0))
        {
            if (!w)
            {
                w = FindWindowExW(parent, 0, 0, 0);
                if (w == from)
                    break;
                if (!w)
                    continue;
            }
            if ((GetWindowLongW(w, GWL_STYLE) & WS_TABSTOP) != 0)
            {
                HWND candidate = previous;
                if (IsWindowEnabled(w))
                    candidate = w;
                previous = candidate;
            }
        }
        return (char)focusTabStop(previous);
    }

    UINT_PTR result = (UINT_PTR)FindWindowExW(parent, from, 0, 0);
    for (HWND w = (HWND)result; (HWND)result != from; w = (HWND)result)
    {
        if (!w)
        {
            result = (UINT_PTR)FindWindowExW(parent, 0, 0, 0);
            w = (HWND)result;
            if ((HWND)result == from || !result)
                break;
        }
        bool focused = focusTabStop(w);
        result = (result & ~(UINT_PTR)0xFF) | (focused ? 1 : 0);
        if (focused)
            break;
        result = (UINT_PTR)FindWindowExW(parent, w, 0, 0);
    }
    return (char)result;
}

LRESULT CALLBACK tabSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
                                 DWORD_PTR refData)
{
    if (msg != WM_KEYDOWN || wParam != VK_TAB)
        return DefSubclassProc(hWnd, msg, wParam, lParam);
    tabToNextSibling(hWnd);
    return 0;
}

LRESULT CALLBACK integerEditSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
                                         DWORD_PTR refData)
{
    if (msg == WM_KEYDOWN)
    {
        if (wParam == VK_TAB)
        {
            tabToNextSibling(hWnd);
            return 0;
        }
    }
    else if (msg == WM_CHAR && GetKeyState(VK_CONTROL) >= 0 && (wParam < '0' || wParam > '9') && wParam != 8 &&
             wParam != 12 && wParam != '-')
    {
        return 0;
    }
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK decimalEditSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
                                         DWORD_PTR refData)
{
    if (msg == WM_KEYDOWN)
    {
        if (wParam == VK_TAB)
        {
            tabToNextSibling(hWnd);
            return 0;
        }
    }
    else if (msg == WM_CHAR && GetKeyState(VK_CONTROL) >= 0 && (wParam < '0' || wParam > '9') && wParam != '.' &&
             wParam != 8 && wParam != 12 && wParam != '-')
    {
        return 0;
    }
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}
