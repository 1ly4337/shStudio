#include "StringList.h"
#include "Strings.h"
#include "String.hpp"
#include <cstdio>
#include "_imports_p06.h"

LRESULT StringList::fill()
{
    LRESULT result = SendMessageW(m_hWnd, LVM_DELETEALLITEMS, 0, 0);
    if (m_strings)
    {
        int count = m_strings->size();
        result = count;
        wchar_t buffer[16] = { 0 };
        for (int i = 0; i < count; ++i)
        {
            swprintf_s(buffer, L"%d", i);
            result = insertTextItem(buffer, -1, -1);
            int item = (int)result;
            if (item < 0)
                break;
            String* text = (i >= 0 && i < m_strings->m_count) ? m_strings->m_items[i] : 0;
            LVITEMW lvi;
            memset(&lvi.iItem, 0, sizeof(lvi) - sizeof(lvi.mask));
            lvi.mask = LVIF_TEXT;
            lvi.iItem = item;
            lvi.iSubItem = 1;
            lvi.pszText = text ? text->m_buffer : 0;
            result = SendMessageW(m_hWnd, LVM_SETITEMTEXTW, item, (LPARAM)&lvi);
        }
    }
    return result;
}

int StringList::getColumnAtCursor(int index, int* range)
{
    range[0] = (int)SendMessageW(m_hWnd, LVM_GETCOLUMNWIDTH, 0, 0);
    range[1] = range[0] + (int)SendMessageW(m_hWnd, LVM_GETCOLUMNWIDTH, 1, 0);
    return 1;
}

bool StringList::setCellText(int index, int subItem, const wchar_t* text)
{
    if (!m_strings || index < 0 || index >= m_strings->size() || subItem != 1)
        return false;
    ListCtrl::setCellText(index, 1, text);
    *m_strings->at(index) = text;
    return true;
}

bool StringList::insertNew(int index)
{
    if (!m_strings)
        return false;
    int count = m_strings->size();
    wchar_t buffer[16] = { 0 };
    swprintf_s(buffer, L"%d", count);
    if (insertTextItem(buffer, -1, -1) < 0)
        return false;
    m_strings->add(L"");
    selectItem(count);
    PostMessageW(m_hWnd, LVM_EDITLABELW, count, 0);
    return true;
}

bool StringList::removeItem(int index)
{
    Strings* strings = m_strings;
    bool result = strings->remove(index);
    if (!result)
        return result;
    strings->m_ids.removeAt(index);
    return true;
}
