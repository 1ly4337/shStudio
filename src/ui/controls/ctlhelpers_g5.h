#pragma once
#include <windows.h>
#include <commctrl.h>
#include "ctlhelpers_g4.h"

inline void g5_subclassCheck(Button& button)
{
    if (button.m_hWnd)
        SetWindowSubclass(button.m_hWnd, noEraseSubclassProc, 0x6368636B , (DWORD_PTR)&button);
}

inline HWND g5_createComboEx(ComboBox& combo, HWND parent, UINT_PTR id, int x, int y, int cx, int cy)
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
    return combo.m_hWnd;
}

inline LRESULT g5_comboSelData(ComboBox& combo)
{
    if (!combo.m_hWnd)
        return 0;
    LRESULT sel = SendMessageW(combo.m_hWnd, CB_GETCURSEL, 0, 0);
    return SendMessageW(combo.m_hWnd, CB_GETITEMDATA, sel, 0);
}

inline void g5_selectItemIcon(ItemEditor* e)
{
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
