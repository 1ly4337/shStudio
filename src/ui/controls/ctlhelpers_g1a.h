#pragma once
#include <windows.h>
#include <commctrl.h>
#include <cstdint>
#include "ida_compat.h"
#include "ComboBox.h"
#include "Button.h"
#include "CtrlHelpers.h"
#include "_imports_p09.h"
#include "_imports_g1a.h"

inline void g1a_createCombo(ComboBox& combo, HWND parent, int x, int y, int cx, int cy, UINT_PTR id)
{
    combo.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"ComboBox", 0, (combo.m_dropLines & 3) | 0x50010000,
                                   x, y, cx, cy, parent, (HMENU)id, 0, 0);
    if (combo.m_hWnd)
        combo.setFontId(combo.m_fontId);
    else
        GetLastError();
}

inline void g1a_createComboEx(ComboBox& combo, HWND parent, int x, int y, int cx, int cy, UINT_PTR id)
{
    combo.m_hWnd = CreateWindowExW(0, L"ComboBoxEx32", 0, (combo.m_dropLines & 3) | 0x50010000,
                                   x, y, cx, cy, parent, (HMENU)id, 0, 0);
    if (combo.m_hWnd)
    {
        combo.setFontId(combo.m_fontId);
        SendMessageW(combo.m_hWnd, CB_SETEXTENDEDUI, 1, 0);
        combo.m_flags = 1;
    }
    else
    {
        GetLastError();
    }
}

inline void g1a_createButton(Button& button, HWND parent, LPCWSTR text, DWORD style, int x, int y, int cx, int cy,
                             UINT_PTR id)
{
    button.m_hWnd = CreateWindowExW(0, L"Button", text, style, x, y, cx, cy, parent, (HMENU)id, 0, 0);
    if (button.m_hWnd)
        button.setFontId(button.m_fontId);
    else
        GetLastError();
}

inline void g1a_subclassCheckBox(Button& button)
{
    if (button.m_hWnd)
        SetWindowSubclass(button.m_hWnd, noEraseSubclassProc, 0x6368636B , (DWORD_PTR)&button);
}
