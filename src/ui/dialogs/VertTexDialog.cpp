#include "VertTexDialog.h"
#include "Window.h"
#include "FileDialogs.h"
#include "mon_g1.h"
#include "_imports_p07.h"
#include <shlwapi.h>

VertTexDialog::VertTexDialog(const wchar_t* folder)
    : Modal(),
      m_folder(folder)
{
}

VertTexDialog::~VertTexDialog()
{
}

bool VertTexDialog::create(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 270, 175);
    if (!Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"MON Vertex-Texture Editor"))
        return false;
    m_owner = Modal_getTopLevelWindow(parent);
    Window_centerOn(m_hWnd, m_owner);
    show(true);
    return true;
}

bool VertTexDialog::onCreate()
{
    monCreateLabel(m_hWnd, L"Vertices file:", 15, 10, 200, 15, 4);
    monCreateEdit(m_vertEdit, m_hWnd, 0x65, L"", 15, 25, 200, 18, tabSubclassProc);
    monCreateButton(m_vertBrowse, m_hWnd, L"...", 0x66, 220, 25, 25, 18);
    monCreateLabel(m_hWnd, L"Texture file:", 15, 53, 200, 15, 4);
    monCreateEdit(m_texEdit, m_hWnd, 0x67, L"", 15, 68, 200, 18, tabSubclassProc);
    monCreateButton(m_texBrowse, m_hWnd, L"...", 0x68, 220, 68, 25, 18);
    return Modal::onCreate();
}

bool VertTexDialog::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code)
        return true;
    if (id == 0x66 || id == 0x68)
    {
        String path(m_folder);
        const wchar_t* filter;
        if (id == 0x66)
        {
            path << L"3DC\\";
            filter = L"3DC files\0*.3dc\0";
        }
        else
        {
            path << L"DDS\\";
            filter = L"Textures\0*.dds;*.tga\0";
        }
        if (FileDialogs_open(m_hWnd, &path, filter, 0, false))
        {
            if (path.startsWith(m_folder))
            {
                LPWSTR name = PathFindFileNameW(path.m_buffer);
                if (id == 0x66)
                    m_vertEdit.setText(name);
                else
                    m_texEdit.setText(name);
            }
            else
            {
                MessageBeep(MB_ICONHAND);
            }
        }
    }
    else if (id <= 11)
    {
        m_result = mapCommandToResult(id);
        if (m_result == 1 && m_modelessOwner)
            SendMessageW(m_modelessOwner->m_hWnd, 0x403, 1, 0);
        if (m_result != -1)
            PostMessageW(m_hWnd, WM_CLOSE, 0, 0);
    }
    return true;
}

int VertTexDialog::mapCommandToResult(int id)
{
    if (id != 1)
        return id;
    {
        String text;
        m_vertFile = *m_vertEdit.getText(&text);
    }
    {
        String text;
        m_texFile = *m_texEdit.getText(&text);
    }
    if (m_vertFile.m_length && m_texFile.m_length)
        return 1;
    return -1;
}
