#pragma once
#include <windows.h>
#include <commctrl.h>
#include <cstdlib>
#include "EditText.h"
#include "Button.h"
#include "String.hpp"
#include "_imports_p06.h"

namespace editctl_C1
{

inline HWND label(HWND parent, LPCWSTR text, DWORD style, int x, int y, int cx, int cy, int fontIndex)
{
    HWND h = CreateWindowExW(0, L"Static", text, style, x, y, cx, cy, parent, 0, 0, 0);
    if (h)
        SendMessageW(h, WM_SETFONT, (WPARAM)getStdFont(fontIndex), 0);
    return h;
}

inline void floatEdit(EditText& edit, HWND parent, UINT id, int x, int y, int cx, int cy, double value)
{
    edit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"" , 0x50010080,
                                  x, y, cx, cy, parent, (HMENU)(UINT_PTR)id, 0, 0);
    if (edit.m_hWnd)
    {
        edit.setFontId(edit.m_fontId);
        SetWindowSubclass(edit.m_hWnd, decimalEditSubclassProc, 0x65646974 , (DWORD_PTR)&edit);
        edit.setFloat((float)value);
    }
}

inline void textEdit(EditText& edit, HWND parent, UINT id, int x, int y, int cx, int cy, DWORD style, UINT limit)
{
    edit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"" , style,
                                  x, y, cx, cy, parent, (HMENU)(UINT_PTR)id, 0, 0);
    if (edit.m_hWnd)
    {
        edit.setFontId(edit.m_fontId);
        SetWindowSubclass(edit.m_hWnd, tabSubclassProc, 0x65646974 , (DWORD_PTR)&edit);
    }
    SendMessageW(edit.m_hWnd, EM_LIMITTEXT, limit, 0);
}

inline void button(Button& btn, HWND parent, LPCWSTR text, UINT id, int x, int y, int cx, int cy)
{
    btn.m_hWnd = CreateWindowExW(0, L"Button", text, 0x50000000, x, y, cx, cy, parent,
                                 (HMENU)(UINT_PTR)id, 0, 0);
    if (btn.m_hWnd)
        btn.setFontId(btn.m_fontId);
    else
        GetLastError();
}

inline float getFloat(EditText& edit)
{
    String text;
    edit.getText(&text);
    float value = 0.0f;
    if (text.length())
        value = (float)_wtof(text.c_str());
    return value;
}

}
