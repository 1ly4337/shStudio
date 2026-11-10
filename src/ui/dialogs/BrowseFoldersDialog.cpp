#include "BrowseFoldersDialog.h"
#include "FolderRoot.h"
#include "Application.h"
#include "gdi_cache.h"
#include "_imports_p09.h"
#include "_imports_g3.h"

BrowseFoldersDialog::~BrowseFoldersDialog()
{
}

bool BrowseFoldersDialog::onCreate()
{
    m_animate = CreateWindowExW(0, L"SysAnimate32", 0, 0x5000000E, 10, 5, 48, 45, m_hWnd, (HMENU)0x65, hModule, 0);
    m_folderText = CreateWindowExW(0, L"Static", L"", 0x50000000, 70, 15, 200, 18, m_hWnd, (HMENU)0x66, hModule, 0);
    m_fileText = CreateWindowExW(0, L"Static", L"", 0x50000000, 70, 35, 200, 18, m_hWnd, (HMENU)0x67, hModule, 0);
    SendMessageW(m_animate, ACM_OPENW, 0, 128);
    SendMessageW(m_folderText, WM_SETFONT, (WPARAM)getStdFont(0), 1);
    SendMessageW(m_fileText, WM_SETFONT, (WPARAM)getStdFont(0), 1);
    m_threadResult = E_FAIL;
    m_thread = CreateThread(0, 0x2000, threadProc, this, 0x20, 0);
    return Modal::onCreate();
}

int BrowseFoldersDialog::mapCommandToResult(int id)
{
    if (!m_thread)
        return id;
    EnableWindow(m_btnCancel, FALSE);
    m_continue = false;
    return -1;
}

DWORD WINAPI BrowseFoldersDialog::threadProc(LPVOID param)
{
    BrowseFoldersDialog* dialog = (BrowseFoldersDialog*)param;
    if (!dialog)
        return E_POINTER;
    int result = dialog->m_root->rescan(static_cast<StreamNotifier*>(dialog));
    dialog->m_threadResult = result;
    dialog->m_thread = 0;
    PostMessageW(dialog->m_hWnd, WM_COMMAND, 2 * (result != 0) + 1, 0);
    return 0;
}

bool BrowseFoldersDialog::setStatus(LPCWSTR text)
{
    SetWindowTextW(m_folderText, text);
    return m_continue;
}

bool BrowseFoldersDialog::progress(LPCWSTR text, int stepLow, int stepHigh)
{
    SetWindowTextW(m_fileText, text);
    return m_continue;
}
