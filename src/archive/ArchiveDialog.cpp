#include "p05_common.h"
#include "ArchiveDialog.h"

ArchiveDialog::ArchiveDialog(FolderRoot* root, bool packing)
    : Modal(), StreamNotifier()
{
    m_root = root;
    m_packing = packing;
    m_worker = 0;
    m_pos = 0;
    m_continue = true;
    m_result = 0;
    m_progress = 0;
    m_animation = 0;
    m_file = 0;
    m_status = 0;
}

bool ArchiveDialog::create(HWND parent)
{
    int height = 130;
    if (m_packing)
        height = 180;
    LPCWSTR title = getTitle();
    m_buttons = 7;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 300, height);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, title);
    if (result) {
        HWND owner = (HWND)Modal_getTopLevelWindow(parent);
        HWND hWnd = m_hWnd;
        m_owner = owner;
        Window_centerOn(hWnd, owner);
        show(true);
        return true;
    }
    return result;
}

bool ArchiveDialog::setStatus(LPCWSTR text)
{
    SetWindowTextW(m_status, text);
    return m_continue;
}

bool ArchiveDialog::progress(LPCWSTR text, int stepLow, int )
{
    HWND file = m_file;
    m_pos += stepLow;
    SetWindowTextW(file, text);
    SendMessageW(m_progress, PBM_SETPOS, m_pos, 0);
    return m_continue;
}

LRESULT ArchiveDialog::setRange(LPARAM max)
{
    return SendMessageW(m_progress, PBM_SETRANGE32, 0, max);
}
