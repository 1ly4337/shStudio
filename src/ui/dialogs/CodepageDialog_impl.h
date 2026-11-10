#pragma once
#include <windows.h>
#include <cstdlib>
#include "String.hpp"
#include "CtrlHelpers.h"
#include "_imports_p10.h"

namespace CodepageDialogImpl
{

template <class L>
int findCodepage(L* list, int codepage)
{
    int row = 0;
    int count = list->getCount(0);
    if (count <= 0)
        return -1;
    while (true)
    {
        String text = list->getCellText(row, 0);
        int cp = _wtoi(text.m_buffer);
        if (cp != (int)0x80000000 && cp == codepage)
            return row;
        if (++row >= count)
            return -1;
    }
}

template <class L>
bool listOnCreate(L* list)
{
    bool ok = list->ListCtrl::onCreate();
    if (ok)
    {
        list->setFont(getStdFont(0));
        list->addColumn(L"CP", 40, 0, -1);
        list->addColumn(L"Name", 180, 0, -1);
        list->addColumn(L"AGE Clients", 100, 0, -1);
        return true;
    }
    return ok;
}

template <class L>
void listOnItemChanged(L* list, int item, UINT state)
{
    if (state & LVIS_SELECTED)
    {
        String text = list->getCellText(item, 0);
        int cp = _wtoi(text.m_buffer);
        if (cp != (int)0x80000000)
            list->m_owner->m_cpEdit.EditText::setInt(cp);
    }
}

template <class D>
void onShowDialog(D* dlg)
{
    dlg->m_cpEdit.EditText::setInt(dlg->m_codepage);
    int row = dlg->m_list.findCodepage(dlg->m_codepage);
    if (row >= 0)
        dlg->m_list.selectItem(row);
    SetFocus(dlg->m_cpEdit.m_hWnd);
    SendMessageW(dlg->m_cpEdit.m_hWnd, EM_SETSEL, 0, -1);
}

template <class D>
bool onCommand(D* dlg, UINT code, HWND ctrl, UINT id)
{
    if (code == EN_CHANGE)
    {
        if (id == 103)
        {
            String text;
            dlg->m_cpEdit.getText(&text);
            if (text.m_length >= 3)
            try
            {
                UINT cp = dlg->m_cpEdit.EditText::getInt(dlg->m_codepage);
                dlg->m_codepage = cp;
                String name;
                CPINFOEXW info;
                memset(&info, 0, sizeof(info));
                if (GetCPInfoExW(cp, 0, &info))
                {
                    name << info.CodePageName;
                    if (name.String::startsWith(text))
                    {
                        name = name.right(name.m_length - (unsigned)wcslen(text.m_buffer));
                        name = name.trimLeft(L' ');
                        name = name.trimLeft(L'(');
                        name = name.trimRight(L')');
                    }
                }
                else
                {
                    name << (unsigned long)dlg->m_codepage << L" (Invalid)";
                }
                const wchar_t* label = L"";
                if (name.m_buffer)
                    label = name.m_buffer;
                SendMessageW(dlg->m_cpName, WM_SETTEXT, 0, (LPARAM)label);
            }
            catch (...)
            {
            }
        }
        return true;
    }
    return dlg->Modal::onCommand(code, ctrl, id);
}

template <class D>
bool createCodepageControls(D* dlg, int y)
{
    int lineY = y;
    RegKey key((HKEY)NULL);
    key.m_open = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"System\\CurrentControlSet\\Control\\NLS\\CodePage", 0,
                               KEY_READ, &key.m_key) == ERROR_SUCCESS;
    key.m_path = _wcsdup(L"System\\CurrentControlSet\\Control\\NLS\\CodePage");
    if (key.m_open)
    {
        String acp;
        int value;
        if (key.getString(L"ACP", &acp) && (value = acp.toInt()) != (int)0x80000000)
        {
            String line;
            line.append(L"System ACP (ANSI code page): ");
            line << (long)value;
            try
            {
                WCHAR locale[128];
                memset(locale, 0, sizeof(locale));
                GetUserDefaultLocaleName(locale, 128);
                line << " (";
                line << (const wchar_t*)locale;
                line << ')';
            }
            catch (...)
            {
            }
            HWND st = CreateWindowExW(0, L"Static", line.m_buffer, 0x50000000, 10, lineY, 340, 16, dlg->m_hWnd,
                                      NULL, NULL, NULL);
            if (st)
                SendMessageW(st, WM_SETFONT, (WPARAM)getStdFont(4), 0);
            y = lineY + 24;
        }
    }
    int labelY = y + 2;
    HWND label = CreateWindowExW(0, L"Static", L"Code page:", 0x50000000, 10, labelY, 60, 40, dlg->m_hWnd,
                                 NULL, NULL, NULL);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    HWND edit = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50012080, 70, y, 60, 18, dlg->m_hWnd,
                                (HMENU)0x67, NULL, NULL);
    dlg->m_cpEdit.m_hWnd = edit;
    if (edit)
    {
        dlg->m_cpEdit.setFontId(dlg->m_cpEdit.m_fontId);
        SetWindowSubclass(dlg->m_cpEdit.m_hWnd, tabSubclassProc, 0x65646974 , (DWORD_PTR)&dlg->m_cpEdit);
    }
    SendMessageW(dlg->m_cpEdit.m_hWnd, EM_LIMITTEXT, 6, 0);
    HWND name = CreateWindowExW(0, L"Static", L"--", 0x50000000, 140, labelY, 170, 24, dlg->m_hWnd,
                                (HMENU)0x68, NULL, NULL);
    if (name)
        SendMessageW(name, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    dlg->m_cpName = name;
    bool ok = dlg->Modal::onCreate();
    return ok;
}

}
