#include "SettingsDial.h"
#include "ComboBox.h"
#include "OptionPanel.h"
#include "ptrArray.h"
#include "ida_compat.h"
#include <commctrl.h>
#include "_imports_p10.h"

bool SettingsDial::onCreate()
{
    HWND combo = CreateWindowExW(WS_EX_CLIENTEDGE, L"ComboBox", NULL, (m_pages.m_dropLines & 3) | 0x50010000,
                                 20, 20, 265, 100, m_hWnd, (HMENU)0x65, NULL, NULL);
    m_pages.m_hWnd = combo;
    if (combo)
        m_pages.setFontId(m_pages.m_fontId);
    else
        GetLastError();
    m_hUpDown = CreateWindowExW(0, L"msctls_updown32", NULL, 0x50000041, 290, 20, 40, 21, m_hWnd, (HMENU)0x66,
                                hModule, NULL);
    bool ok = createPanels();
    if (ok)
    {
        int count = m_panels.size();
        for (int i = 0; i < count; ++i)
            m_panels.m_items[i]->load((IniFile*)m_settings);
        short last = (short)m_panels.size();
        SendMessageW(m_hUpDown, UDM_SETRANGE, 0, (unsigned short)(last - 1));
        SendMessageW(m_hUpDown, UDM_SETPOS, 0, 0);
        selectPanel(0, false);
        SendMessageW(m_pages.m_hWnd, CB_SETCURSEL, 0, 0);
        return Modal::onCreate();
    }
    return ok;
}

bool SettingsDial::addPanel(OptionPanel* panel, LPCWSTR name)
{
    if (!panel || !name)
        return false;
    m_pages.addString(name);
    m_panels.add(panel);
    return true;
}

bool SettingsDial::selectPanel(int key, bool byId)
{
    int index;
    if (byId)
    {
        index = 0;
        int count = m_panels.size();
        if (count <= 0)
            return false;
        while ((int)(INT_PTR)m_panels.m_items[index]->m_childId != key)
        {
            if (++index >= count)
                return false;
        }
    }
    else
    {
        index = key;
        if (key < 0 || key >= m_panels.size())
            return false;
    }
    if (!m_panels.m_items[index])
        return false;
    if (m_current >= 0)
        m_panels.m_items[m_current]->show(false);
    m_current = index;
    m_panels.m_items[index]->show(true);
    return true;
}

bool SettingsDial::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code != CBN_SELCHANGE)
        return Modal::onCommand(code, ctrl, id);
    LRESULT sel = SendMessageW(m_pages.m_hWnd, CB_GETCURSEL, 0, 0);
    SendMessageW(m_hUpDown, UDM_SETPOS, 0, (unsigned short)sel);
    selectPanel((int)sel, false);
    return true;
}

bool SettingsDial::onNotify(NMHDR* hdr, LRESULT* result)
{
    if (hdr->code != (UINT)UDN_DELTAPOS)
        return false;
    NMUPDOWN* ud = (NMUPDOWN*)hdr;
    int pos = ud->iPos + ud->iDelta;
    int sel = (int)SendMessageW(m_pages.m_hWnd, CB_GETCOUNT, 0, 0) - 1;
    if (pos >= 0)
    {
        if (pos > sel)
            pos = 0;
        sel = pos;
    }
    SendMessageW(m_pages.m_hWnd, CB_SETCURSEL, (WPARAM)sel, 0);
    selectPanel(sel, false);
    return true;
}

int SettingsDial::mapCommandToResult(int id)
{
    if (id == IDOK)
    {
        int count = m_panels.size();
        for (int i = 0; i < count; ++i)
        {
            if (!m_panels.m_items[i]->validate())
                return -1;
        }
        for (int i = 0; i < count; ++i)
            m_panels.m_items[i]->save((IniFile*)m_settings);
        return IDOK;
    }
    return id;
}
