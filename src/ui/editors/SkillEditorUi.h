#pragma once
#include <windows.h>
#include <commctrl.h>
#include "EditText.h"
#include "ComboBox.h"
#include "Button.h"
#include "_imports_H.h"

inline void skui_createEdit(EditText& edit, HWND parent, DWORD style, int x, int y, int cx, int cy, UINT_PTR id,
                            WPARAM limit)
{
    edit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", style, x, y, cx, cy, parent, (HMENU)id, 0, 0);
    if (edit.m_hWnd)
    {
        edit.setFontId(edit.m_fontId);
        SetWindowSubclass(edit.m_hWnd, tabSubclassProc, 0x65646974 , (DWORD_PTR)&edit);
    }
    edit.setFont(getStdFont(6));
    SendMessageW(edit.m_hWnd, EM_LIMITTEXT, limit, 0);
}

inline void skui_createCombo(ComboBox& combo, HWND parent, int x, int y, int cx, int cy, UINT_PTR id)
{
    combo.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"ComboBox", 0, (combo.m_dropLines & 3) | 0x50010000,
                                   x, y, cx, cy, parent, (HMENU)id, 0, 0);
    if (combo.m_hWnd)
        combo.setFontId(combo.m_fontId);
    else
        GetLastError();
}

inline void skui_createComboEx(ComboBox& combo, HWND parent, int x, int y, int cx, int cy, UINT_PTR id)
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

inline void skui_createButton(Button& button, HWND parent, LPCWSTR text, DWORD style, int x, int y, int cx, int cy,
                              UINT_PTR id)
{
    button.m_hWnd = CreateWindowExW(0, L"Button", text, style, x, y, cx, cy, parent, (HMENU)id, 0, 0);
    if (button.m_hWnd)
        button.setFontId(button.m_fontId);
    else
        GetLastError();
}

inline void skui_subclassCheckBox(Button& button)
{
    if (button.m_hWnd)
        SetWindowSubclass(button.m_hWnd, noEraseSubclassProc, 0x6368636B , (DWORD_PTR)&button);
}

inline void skui_addString(ComboBox& combo, LPCWSTR text)
{
    if (SendMessageW(combo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)text) != CB_ERR)
    {
        LRESULT count = SendMessageW(combo.m_hWnd, CB_GETCOUNT, 0, 0);
        combo.m_items.StringTable::put((int)count, text);
    }
}

inline LRESULT skui_curData(ComboBox& combo)
{
    if (!combo.m_hWnd)
        return 0;
    LRESULT sel = SendMessageW(combo.m_hWnd, CB_GETCURSEL, 0, 0);
    return SendMessageW(combo.m_hWnd, CB_GETITEMDATA, sel, 0);
}

template <class V>
inline void skui_clearVector(V& v)
{
    for (int i = 0; i < v.m_capacity; ++i)
        v.resetItem(&v.m_items[i]);
    v.m_count = 0;
}
