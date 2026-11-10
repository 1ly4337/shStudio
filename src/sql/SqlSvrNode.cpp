#include <windows.h>
#include <commctrl.h>
#include <cstring>
#include "SqlSvrNode.h"
#include "SqlConnectDialog.h"
#include "TableNode.h"
#include "MsSQLserver.h"
#include "TreeList.h"
#include "ToggleNode.h"
#include "AutoPtrCell.h"
#include "Preferences.h"
#include "StudioFrame.h"
#include "String.hpp"
#include "ida_compat.h"
#include "_imports_p09.h"
#include "_imports_g4.h"

#pragma warning(disable : 4066 4498)

SqlSvrNode::SqlSvrNode(void* owner)
    : WorkspaceNode(owner, 17, 134)
{
    m_connecting = false;
    m_connected = false;
    m_dbItem = 0;
    m_server = new MsSQLserver();
    if (m_server->init())
    {
        delete m_server;
        m_server = 0;
    }
}

SqlSvrNode::~SqlSvrNode()
{
    if (m_connecting)
    {
        do
            Sleep(500);
        while (m_connecting);
        Sleep(250);
    }
    if (m_server)
    {
        if (m_connected)
            m_server->disconnect();
        delete m_server;
    }
}

const wchar_t* SqlSvrNode::getString(DWORD tag) const
{
    if (tag == 'name')
        return L"Databases";
    if (tag != 'stat')
        return 0;
    if (m_connecting)
        return L"Connecting...";
    if (!m_connected)
        return L"Offline";
    return L"Connected";
}

static BOOL setMenuItemState(HMENU menu, UINT id, UINT state)
{
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, 0x2C);
    mii.cbSize = 48;
    mii.fMask = MIIM_STATE;
    mii.fState = state;
    return SetMenuItemInfoW(menu, id, FALSE, &mii);
}

void SqlSvrNode::onInitItemMenu(HMENU hMenu, HWND, HTREEITEM)
{
    if (m_connected)
    {
        setMenuItemState(hMenu, 0xE160, MFS_DISABLED);
        setMenuItemState(hMenu, 0xE161, 0);
    }
    else
    {
        setMenuItemState(hMenu, 0xE160, 0);
        setMenuItemState(hMenu, 0xE161, MFS_DISABLED);
    }
}

void SqlSvrNode::onCommand(int cmd, int arg)
{
    if (cmd == 0xE160)
    {
        if (connect((TreeList*)arg, true) == 32)
            (g_studioFrame)->setStatusText(L"Another connection attempt is running.", 0);
    }
    else if (cmd == 0xE161)
    {
        TreeList* tree = (TreeList*)arg;
        tree->deleteChildren((HTREEITEM)(INT_PTR)m_data);
        tree->invalidate(true);
        if (m_connected)
            m_server->disconnect();
        m_connected = false;
        m_dbItem = 0;
    }
}

int SqlSvrNode::connect(TreeList* tree, bool showDialog)
{
    if (!m_server)
        return E_POINTER;
    if (m_connecting)
        return 32;
    m_connecting = true;
    if (m_connected)
    {
        tree->deleteChildren((HTREEITEM)(INT_PTR)m_data);
        tree->invalidate(true);
        m_dbItem = 0;
        m_connected = false;
    }

    int hr;
    if (showDialog)
    {
        SqlConnectDialog dialog(m_server);
        if (dialog.createDialog((HWND)m_owner, 340, 220, 1, L"SQL Server Connection")
            && dialog.doModal() == 1)
        {
            g_preferences.writeString(L"SQL Server", L"srvr", dialog.m_serverName.m_buffer);
            g_preferences.writeString(L"SQL Server", L"user", dialog.m_user.m_buffer);
            hr = 0;
        }
        else
        {
            hr = E_FAIL;
        }
    }
    else if (g_preferences.getInt(L"SQL Server", L"save", 0) == 1)
    {
        String driver = g_preferences.getString(L"SQL Server", L"drvr", 0);
        String server = g_preferences.getString(L"SQL Server", L"srvr", 0);
        String user = g_preferences.getString(L"SQL Server", L"user", 0);
        String password = g_preferences.getString(L"SQL Server", L"pass", 0);
        hr = m_server->connect(driver.m_buffer, server.m_buffer, -1, user.m_buffer, password.m_buffer);
    }
    else
    {
        hr = E_FAIL;
    }

    m_connecting = false;
    if (hr)
    {
        disconnect(tree);
        return hr;
    }
    m_connected = true;
    int result = m_server->useDatabase(L"PS_GameDefs");
    if (!result)
    {
        m_dbItem = (LPARAM)tree->TreeList::insertNode((HTREEITEM)(INT_PTR)m_data, new ToggleNode(L"PS_GameDefs"), 18, 1, TVI_LAST);
        int count = m_server->m_tables.m_count;
        for (int i = 0; i < count; ++i)
        {
            Table* table = m_server->getTable(i);
            tree->TreeList::insertCell(new AutoPtrCell(new TableNode(table)), (HTREEITEM)m_dbItem, 19, 0, TVI_LAST);
        }
        SendMessageW(FIELD(HWND, tree, 0x38), TVM_EXPAND, TVE_EXPAND, m_data);
    }
    return result;
}

void SqlSvrNode::disconnect(TreeList* tree)
{
    tree->deleteChildren((HTREEITEM)(INT_PTR)m_data);
    tree->invalidate(true);
    if (m_connected)
        m_server->disconnect();
    m_dbItem = 0;
    m_connected = false;
}
