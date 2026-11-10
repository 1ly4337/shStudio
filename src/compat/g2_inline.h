#pragma once
#include <windows.h>
#include <commctrl.h>
#include "Button.h"
#include "EditText.h"
#include "ComboBox.h"
#include "gdi_cache.h"
#include "_imports_p07.h"

inline HWND g2_createStatic(HWND parent, const wchar_t* text, int x, int y, int cx, int cy, int fontIndex)
{
    HWND label = CreateWindowExW(0, L"Static", text, 0x50000000u, x, y, cx, cy, parent, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(fontIndex), 0);
    return label;
}

inline void g2_createCheckBox(Button& button, HWND parent, const wchar_t* text, int x, int y, int cx, int cy,
                              UINT id)
{
    button.m_hWnd = CreateWindowExW(0, L"Button", text, 0x50000002u, x, y, cx, cy, parent,
                                    (HMENU)(UINT_PTR)id, 0, 0);
    if (button.m_hWnd)
        button.setFontId(button.m_fontId);
    else
        GetLastError();
    if (button.m_hWnd)
        SetWindowSubclass(button.m_hWnd, noEraseSubclassProc, 'chck', (DWORD_PTR)&button);
}

inline void g2_createEdit(EditText& edit, HWND parent, DWORD style, int x, int y, int cx, int cy, UINT id)
{
    edit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", style, x, y, cx, cy, parent,
                                  (HMENU)(UINT_PTR)id, 0, 0);
    if (edit.m_hWnd)
    {
        edit.setFontId(edit.m_fontId);
        SetWindowSubclass(edit.m_hWnd, tabSubclassProc, 'edit', (DWORD_PTR)&edit);
    }
}

inline void g2_createComboEx(ComboBox& combo, HWND parent, int x, int y, int cx, int cy, UINT id, short flags)
{
    combo.m_hWnd = CreateWindowExW(0, L"ComboBoxEx32", 0, (combo.m_dropLines & 3) | 0x50010000u,
                                   x, y, cx, cy, parent, (HMENU)(UINT_PTR)id, 0, 0);
    if (combo.m_hWnd)
    {
        combo.setFontId(combo.m_fontId);
        SendMessageW(combo.m_hWnd, CB_SETEXTENDEDUI, 1, 0);
        combo.m_flags = flags;
    }
    else
    {
        GetLastError();
    }
}
