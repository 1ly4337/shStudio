#include "FolderOption.h"
#include "IniFile.h"
#include "Preferences.h"
#include "StudioFrame.h"
#include "String.hpp"
#include "g2_inline.h"
#include "_imports_p07.h"
#include "FileDialogs.h"

bool __stdcall FolderOption::isClientFolder(const wchar_t* path)
{
    String dataPath(path);
    dataPath << L"\\Bin\\Data\\";
    DWORD attributes = GetFileAttributesW(dataPath.m_buffer);
    bool ok = attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    return ok;
}

FolderOption::FolderOption()
{
}

FolderOption::~FolderOption()
{
}

void FolderOption::load(IniFile* prefs)
{
    {
        String location = prefs->getString(L"Server Data", L"location", 0);
        m_picker.m_path = location.m_buffer;
        m_picker.invalidate(false);
    }
    m_autoConnect.setCheck(prefs->getInt(L"Server Data", L"connect", 0) == 1);
}

void FolderOption::save(IniFile* prefs)
{
    prefs->writeString(L"Server Data", L"location", m_picker.m_path.m_length ? m_picker.m_path.m_buffer : 0);
    bool connect = (BYTE)m_autoConnect.isChecked() != 0;
    prefs->writeInt(L"Server Data", L"connect", connect);
}

bool FolderOption::onCreate()
{
    g2_createStatic(m_hWnd, L"Location of server data", 15, 10, 250, 16, 1);
    HWND parent = m_hWnd;
    m_picker.m_browse = false;
    RECT rc;
    SetRect(&rc, 15, 27, 295, 47);
    m_picker.create(parent, (HMENU)0x65, &rc, 0x50010000, 0, 0);
    g2_createStatic(m_hWnd,
                    L"Some files are common to client and server; server copies of these files or SQL scripts are "
                    L"saved there when the client files are edited.",
                    15, 51, 280, 48, 4);
    g2_createCheckBox(m_autoConnect, m_hWnd, L"Auto-connect at startup", 15, 99, 150, 18, 0x66);
    m_autoConnect.setCheck(g_preferences.getInt(L"Server Data", L"Connect", 0) == 1);
    return true;
}

bool FolderOption::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code)
        return Container::onCommand(code, ctrl, id);
    if (id != 101)
    {
        if (id == 102)
        {
            bool checked = (BYTE)m_autoConnect.isChecked() != 0;
            m_autoConnect.setCheck(!checked);
            return true;
        }
        return Container::onCommand(code, ctrl, id);
    }

    String path(m_picker.m_path.m_length ? m_picker.m_path.m_buffer : 0);
    if (FileDialogs_selectFolder(GetParent(m_hWnd), &path, L"Locate your PSM_Client directory", 0, true, isClientFolder))
    {
        m_picker.m_path = path.m_buffer;
        m_picker.invalidate(false);
        m_autoConnect.setCheck(true);
        g_studioFrame->connectServer(&path);
    }
    return true;
}
