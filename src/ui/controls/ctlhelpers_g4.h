#pragma once
#include <windows.h>
#include <commctrl.h>
#include "ComboBox.h"
#include "EditText.h"
#include "Button.h"
#include "_imports_p07.h"

inline HWND g4_createLabel(const wchar_t* text, int x, int y, int cx, int cy, HWND parent, int fontId)
{
    HWND label = CreateWindowExW(0, L"Static", text, WS_CHILD | WS_VISIBLE, x, y, cx, cy, parent, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(fontId), 0);
    return label;
}

inline HWND g4_createCombo(ComboBox& combo, HWND parent, UINT_PTR id, int x, int y, int cx, int cy)
{
    combo.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"ComboBox", 0, (combo.m_dropLines & 3) | 0x50010000,
                                   x, y, cx, cy, parent, (HMENU)id, 0, 0);
    if (combo.m_hWnd)
        combo.setFontId(combo.m_fontId);
    else
        GetLastError();
    return combo.m_hWnd;
}

inline void g4_addComboString(ComboBox& combo, const wchar_t* text)
{
    if (SendMessageW(combo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)text) != -1)
    {
        LRESULT count = SendMessageW(combo.m_hWnd, CB_GETCOUNT, 0, 0);
        combo.m_items.put(count, text);
    }
}

inline HWND g4_createEdit(EditText& edit, HWND parent, UINT_PTR id, const wchar_t* text, DWORD style,
                          int x, int y, int cx, int cy)
{
    edit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", text, style, x, y, cx, cy, parent, (HMENU)id, 0, 0);
    if (edit.m_hWnd)
    {
        edit.setFontId(edit.m_fontId);
        SetWindowSubclass(edit.m_hWnd, tabSubclassProc, 0x65646974 , (DWORD_PTR)&edit);
    }
    return edit.m_hWnd;
}

inline HWND g4_createButton(Button& button, HWND parent, UINT_PTR id, const wchar_t* text, DWORD style,
                            int x, int y, int cx, int cy)
{
    button.m_hWnd = CreateWindowExW(0, L"Button", text, style, x, y, cx, cy, parent, (HMENU)id, 0, 0);
    if (button.m_hWnd)
        button.setFontId(button.m_fontId);
    else
        GetLastError();
    return button.m_hWnd;
}

#include "ItemEditor.h"
#include "ClientData.h"
#include "StudioFrame.h"

inline ClientData* g4_clientData()
{
    return reinterpret_cast<ClientData*>(reinterpret_cast<BYTE*>(g_studioFrame) + 0x230);
}

inline void g4_createIconPicker(ItemEditor* e)
{
    e->m_icon.create(e->m_hWnd, (HMENU)0x65, 0, 23, 34, 34, 32, 0);

    int count = FIELD(int, &e->m_icon.m_bitmaps, 0x10);
    int* ids = FIELD(int*, &e->m_icon.m_bitmaps, 0x08);
    int sel = 0;
    for (; sel < count; ++sel, ++ids)
        if (*ids == FIELD(unsigned char, e->m_item, 0x57))
            break;
    if (sel >= count)
        sel = -1;
    e->m_icon.m_selected = sel;
    e->m_icon.showIcon(sel);
    FIELD(int, &e->m_icon.m_dropDown, 0x48) = sel;
}

inline void g4_setupIconPicker(ItemEditor* e)
{
    ImageStrip* icons = g4_clientData()->getItemIcons(FIELD(unsigned char, e->m_item, 0x54));
    if (icons)
    {
        int count = FIELD(int, icons, 0x20);
        if (count > 81)
            e->m_icon.m_columns = 16;
        else if (count > 36)
            e->m_icon.m_columns = 9;
        e->m_icon.m_bitmaps = *(icons);
        e->m_icon.m_cells = *(icons);
    }
    g4_createIconPicker(e);
}

inline bool g4_createModal(ItemEditor* e, HWND parent, int cx, int cy, const wchar_t* title)
{
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, cx, cy);
    if (!e->create(parent, 0, &rc, 0x80C80000, 0x10101, title))
        return false;
    HWND top = Modal_getTopLevelWindow(parent);
    e->Modal::m_owner = top;
    Window_centerOn(e->m_hWnd, top);
    e->show(true);
    return true;
}
