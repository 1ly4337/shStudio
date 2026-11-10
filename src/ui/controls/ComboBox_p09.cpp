#include <windows.h>
#include <commctrl.h>
#include <new>
#include <cstring>
#include <cwchar>
#include "ComboBox.h"
#include "String.hpp"
#include "_imports_p09.h"
#include "_imports_g6.h"

#include "StringTable.h"

String* ComboBox::getText(String* out)
{
    LRESULT selection = SendMessageW(m_hWnd, CB_GETCURSEL, 0, 0);
    if (selection == CB_ERR)
    {
        Control32::getText(out);
        return out;
    }
    LRESULT length = SendMessageW(m_hWnd, CB_GETLBTEXTLEN, selection, 0);
    String text;
    if (length > 0)
    {
        text.reserve((unsigned)(length + 1));
        wchar_t* buffer = text.m_buffer;
        unsigned copied = (unsigned)SendMessageW(m_hWnd, CB_GETLBTEXT, selection, (LPARAM)buffer);
        if (copied < text.m_capacity)
        {
            text.m_length = copied;
            buffer[copied] = 0;
        }
    }
    new (out) String(text);
    return out;
}

bool ComboBox::addString(const wchar_t* text)
{
    if (SendMessageW(m_hWnd, CB_ADDSTRING, 0, (LPARAM)text) == CB_ERR)
        return false;
    LRESULT count = SendMessageW(m_hWnd, CB_GETCOUNT, 0, 0);
    m_items.StringTable::put((int)count, text);
    return true;
}

bool ComboBox::addItem(const wchar_t* text, int data)
{
    if (!LOBYTE(m_flags))
        return false;
    if (!HIBYTE(m_flags))
    {
        int count = m_items.m_count;
        int index = 0;
        if (count > 0)
        {
            StringTable::Entry** entry = m_items.m_items;
            while (!*entry || (*entry)->id != data)
            {
                ++index;
                ++entry;
                if (index >= count)
                    goto insert;
            }
            if (index != -1)
                return false;
        }
    }
insert:
    COMBOBOXEXITEMW item;
    memset(&item, 0, sizeof(item));
    item.mask = CBEIF_TEXT | CBEIF_LPARAM;
    item.iItem = (INT_PTR)SendMessageW(m_hWnd, CB_GETCOUNT, 0, 0);
    item.pszText = (LPWSTR)text;
    item.cchTextMax = 0;
    item.lParam = data;
    if (SendMessageW(m_hWnd, CBEM_INSERTITEMW, 0, (LPARAM)&item) < 0)
        return false;
    m_items.StringTable::put(data, text);
    return true;
}

bool ComboBox::setItemText(int data, const wchar_t* text)
{
    if (!LOBYTE(m_flags))
        return false;
    COMBOBOXEXITEMW item;
    item.iItem = 0;
    item.pszText = 0;
    item.cchTextMax = 0;
    item.iImage = 0;
    item.iSelectedImage = 0;
    item.iOverlay = 0;
    item.iIndent = 0;
    item.lParam = 0;
    item.mask = CBEIF_LPARAM;
    LRESULT count = SendMessageW(m_hWnd, CB_GETCOUNT, 0, 0);
    for (int i = 0; i < count; ++i)
    {
        item.iItem = i;
        if (!SendMessageW(m_hWnd, CBEM_GETITEMW, 0, (LPARAM)&item))
            break;
        if (item.lParam == -1)
        {
            item.pszText = (LPWSTR)text;
            item.mask = CBEIF_TEXT;
            return SendMessageW(m_hWnd, CBEM_SETITEMW, 0, (LPARAM)&item) >= 0;
        }
    }
    return false;
}

bool ComboBox::selectData(WPARAM data)
{
    if (!LOBYTE(m_flags))
        return SendMessageW(m_hWnd, CB_SETCURSEL, data, 0) != CB_ERR;
    LRESULT count = SendMessageW(m_hWnd, CB_GETCOUNT, 0, 0);
    WPARAM index = 0;
    if (count <= 0)
        return false;
    while ((WPARAM)SendMessageW(m_hWnd, CB_GETITEMDATA, index, 0) != data)
    {
        if ((int)++index >= count)
            return false;
    }
    SendMessageW(m_hWnd, CB_SETCURSEL, index, 0);
    return true;
}

LRESULT CALLBACK ComboBox::ownerDrawSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                 UINT_PTR idSubclass, DWORD_PTR refData)
{
    ComboBox* combo = (ComboBox*)refData;
    if (!combo)
        return DefSubclassProc(hWnd, msg, wParam, lParam);
    RECT client;
    SetRectEmpty(&client);
    GetClientRect(hWnd, &client);
    if (msg == WM_MEASUREITEM)
    {
        MEASUREITEMSTRUCT* measure = (MEASUREITEMSTRUCT*)lParam;
        if (measure->itemHeight < 0x16)
            measure->itemHeight = 22;
        return 1;
    }
    if (msg != WM_DRAWITEM)
        return DefSubclassProc(hWnd, msg, wParam, lParam);

    DRAWITEMSTRUCT* draw = (DRAWITEMSTRUCT*)lParam;
    if (draw->itemID == (UINT)-1)
        return 1;
    TEXTMETRICW tm;
    GetTextMetricsW(draw->hDC, &tm);
    int index = (int)draw->itemID;
    int y = (draw->rcItem.top + draw->rcItem.bottom - tm.tmHeight) / 2;
    const wchar_t* text;
    StringTable::Entry* entry;
    if (index < 0 || index >= combo->m_items.m_capacity || (entry = combo->m_items.m_items[index]) == 0)
        text = 0;
    else
        text = entry->text.m_buffer;
    UINT length = text ? (UINT)wcslen(text) : 0;

    if ((draw->itemState & ODS_COMBOBOXEDIT) != 0)
    {
        int oldMode = SetBkMode(draw->hDC, TRANSPARENT);
        ExtTextOutW(draw->hDC, 5, y, ETO_CLIPPED, &draw->rcItem, text, length, 0);
        SetBkMode(draw->hDC, oldMode);
        return 1;
    }
    int textColor = COLOR_WINDOWTEXT;
    if ((draw->itemState & ODS_SELECTED) != 0)
        textColor = COLOR_HIGHLIGHTTEXT;
    COLORREF oldText = SetTextColor(draw->hDC, GetSysColor(textColor));
    COLORREF oldBk = SetBkColor(draw->hDC, GetSysColor((8 * (draw->itemState & ODS_SELECTED)) | COLOR_WINDOW));
    ExtTextOutW(draw->hDC, 5, y, ETO_OPAQUE | ETO_CLIPPED, &draw->rcItem, text, length, 0);
    SetTextColor(draw->hDC, oldText);
    SetBkColor(draw->hDC, oldBk);
    return 1;
}
