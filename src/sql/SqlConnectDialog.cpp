#include <windows.h>
#include <commctrl.h>
#include <cwchar>
#include "SqlConnectDialog.h"
#include "MsSQLserver.h"
#include "Preferences.h"
#include "RegKey.h"
#include "StringTable.h"
#include "CtrlHelpers.h"
#include "_imports_p09.h"
#include "_imports_g4.h"

#pragma comment(lib, "comctl32.lib")

SqlConnectDialog::SqlConnectDialog(MsSQLserver* server)
    : Modal(),
      m_server(server)
{
    m_serverName = g_preferences.getString(L"SQL Server", L"srvr", 0);
    m_user = g_preferences.getString(L"SQL Server", L"user", 0);
    m_password = g_preferences.getString(L"SQL Server", L"pass", 0);
    m_save = g_preferences.getInt(L"SQL Server", L"save", 0) == 1;
}

SqlConnectDialog::~SqlConnectDialog()
{
}

bool SqlConnectDialog::onCreate()
{
    HWND label = CreateWindowExW(0, L"Static", L"Drivers' Name:", 0x50000000, 20, 25, 70, 18, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);

    m_driverCombo.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"ComboBox", 0, m_driverCombo.m_dropLines & 3 | 0x50010000,
                                           100, 20, 220, 20, m_hWnd, (HMENU)0x6472, 0, 0);
    if (m_driverCombo.m_hWnd)
        m_driverCombo.setFontId(m_driverCombo.m_fontId);
    else
        GetLastError();

    RegKey drivers(HKEY_LOCAL_MACHINE);
    drivers.open((wchar_t*)L"SOFTWARE\\ODBC\\ODBCINST.INI", KEY_READ);
    if (drivers.m_open)
    {
        String name;
        DWORD index = 0;
        for (;;)
        {
            name.clear();
            if (!drivers.enumKey(&index, &name))
                break;
            const wchar_t* text = name.m_buffer;
            if (name.m_length >= 10 && !wcsncmp(text, L"SQL Server", 10)
                && SendMessageW(m_driverCombo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)text) != CB_ERR)
            {
                int count = (int)SendMessageW(m_driverCombo.m_hWnd, CB_GETCOUNT, 0, 0);
                if (count >= 0)
                    m_driverCombo.m_items.add(new StringTable::Entry(count, text));
            }
        }
        SendMessageW(m_driverCombo.m_hWnd, CB_SETCURSEL, 0, 0);
    }

    label = CreateWindowExW(0, L"Static", L"Name or IP of server:", 0x50000000, 20, 50, 130, 18, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    m_serverEdit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", m_serverName.m_buffer, 0x50010080,
                                          150, 48, 170, 18, m_hWnd, (HMENU)0x7372, 0, 0);
    if (m_serverEdit.m_hWnd)
    {
        m_serverEdit.setFontId(m_serverEdit.m_fontId);
        SetWindowSubclass(m_serverEdit.m_hWnd, tabSubclassProc, 0x65646974 , (DWORD_PTR)&m_serverEdit);
    }

    label = CreateWindowExW(0, L"Static", L"User's Name (SQL or NT):", 0x50000000, 20, 74, 130, 18, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    m_userEdit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", m_user.m_buffer, 0x50010080,
                                        150, 72, 170, 18, m_hWnd, (HMENU)0x7573, 0, 0);
    if (m_userEdit.m_hWnd)
    {
        m_userEdit.setFontId(m_userEdit.m_fontId);
        SetWindowSubclass(m_userEdit.m_hWnd, tabSubclassProc, 0x65646974 , (DWORD_PTR)&m_userEdit);
    }

    label = CreateWindowExW(0, L"Static", L"SQL User's Password:", 0x50000000, 20, 98, 130, 18, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    m_passwordEdit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", m_password.m_buffer, 0x50010080,
                                            150, 96, 150, 18, m_hWnd, (HMENU)0x7073, 0, 0);
    if (m_passwordEdit.m_hWnd)
    {
        m_passwordEdit.setFontId(m_passwordEdit.m_fontId);
        SetWindowSubclass(m_passwordEdit.m_hWnd, tabSubclassProc, 0x65646974 ,
                          (DWORD_PTR)&m_passwordEdit);
    }

    m_saveCheck.m_hWnd = CreateWindowExW(0, L"Button", L" Save info and auto-connect at startup", 0x50000002,
                                         20, 120, 220, 18, m_hWnd, (HMENU)0x6B70, 0, 0);
    if (m_saveCheck.m_hWnd)
        m_saveCheck.setFontId(m_saveCheck.m_fontId);
    else
        GetLastError();
    if (m_saveCheck.m_hWnd)
        SetWindowSubclass(m_saveCheck.m_hWnd, noEraseSubclassProc, 0x6368636B , (DWORD_PTR)&m_saveCheck);
    m_saveCheck.setCheck(m_save);

    return Modal::onCreate();
}

bool SqlConnectDialog::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code == 0)
    {
        if (id == 0x6B70)
        {
            m_save = !m_save;
            m_saveCheck.setCheck(m_save);
            return true;
        }
        if (id <= 0xB)
        {
            m_result = mapCommandToResult(id);
            if (m_result == 1 && m_modelessOwner)
                SendMessageW(m_modelessOwner->m_hWnd, 0x403, 1, 0);
            if (m_result != -1)
                PostMessageW(m_hWnd, WM_CLOSE, 0, 0);
        }
    }
    return true;
}

int SqlConnectDialog::mapCommandToResult(int id)
{
    if (id != 1)
        return id;
    {
        String text;
        m_driver = *m_driverCombo.getText(&text);
    }
    {
        String text;
        m_serverName = *m_serverEdit.getText(&text);
    }
    {
        String text;
        m_user = *m_userEdit.getText(&text);
    }
    {
        String text;
        m_password = *m_passwordEdit.getText(&text);
    }
    if (m_server->connect(m_driver.m_buffer, m_serverName.m_buffer, 0, m_user.m_buffer, m_password.m_buffer))
        return -1;
    g_preferences.writeString(L"SQL Server", L"drvr", m_driver.m_buffer);
    g_preferences.writeString(L"SQL Server", L"srvr", m_serverName.m_buffer);
    g_preferences.writeString(L"SQL Server", L"user", m_user.m_buffer);
    if (m_saveCheck.isChecked())
    {
        g_preferences.writeString(L"SQL Server", L"pass", m_password.m_buffer);
        g_preferences.writeInt(L"SQL Server", L"save", 1);
    }
    return 1;
}

void SqlConnectDialog::onShowDialog()
{
    SendMessageW(m_serverEdit.m_hWnd, EM_SETSEL, 0, -1);
    SetFocus(m_serverEdit.m_hWnd);
}
