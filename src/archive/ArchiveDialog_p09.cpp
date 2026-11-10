#include "ArchiveDialog.h"
#include "Application.h"
#include "gdi_cache.h"
#include "ida_compat.h"
#include "_imports_p09.h"
#include "_imports_g3.h"

bool ArchiveDialog::onCreate()
{
    WPARAM font = (WPARAM)getStdFont(0);
    HWND parent = m_hWnd;
    HWND fileLabel;
    int y;
    if (m_packing)
    {
        FIELD(HWND, this, 0xB4) = CreateWindowExW(0, L"SysAnimate32", 0, 0x5000000E, 10, 5, 272, 60, parent,
                                                  (HMENU)0x65, hModule, 0);
        m_status = CreateWindowExW(0, L"Static", L"", 0x50000000, 20, 70, 260, 16, m_hWnd, (HMENU)0x66, hModule, 0);
        fileLabel = CreateWindowExW(0, L"Static", L"", 0x50000000, 20, 85, 260, 16, m_hWnd, (HMENU)0x67, hModule, 0);
        y = 110;
    }
    else
    {
        FIELD(HWND, this, 0xB4) = CreateWindowExW(0, L"SysAnimate32", 0, 0x5000000E, 10, 5, 48, 45, parent,
                                                  (HMENU)0x65, hModule, 0);
        m_status = CreateWindowExW(0, L"Static", L"", 0x50000000, 70, 15, 200, 16, m_hWnd, (HMENU)0x66, hModule, 0);
        fileLabel = CreateWindowExW(0, L"Static", L"", 0x50000000, 70, 35, 200, 16, m_hWnd, (HMENU)0x67, hModule, 0);
        y = 60;
    }
    m_file = fileLabel;
    SendMessageW(FIELD(HWND, this, 0xB4), ACM_OPENW, 0, (m_packing != 0) + 128);
    SendMessageW(m_status, WM_SETFONT, font, 1);
    SendMessageW(m_file, WM_SETFONT, font, 1);
    m_progress = CreateWindowExW(0, L"msctls_progress32", 0, 0x50000001, 20, y + 3, 180, 18, m_hWnd, (HMENU)0x68,
                                 hModule, 0);
    SendMessageW(m_progress, PBM_SETRANGE32, 0, (LPARAM)getWorkSize());
    SendMessageW(m_progress, PBM_SETPOS, 0, 0);
    HWND button = CreateWindowExW(0, L"Button", getButtonText(2), 0x50000000, 215, y, 60, 24, m_hWnd, (HMENU)2, 0, 0);
    if (button)
        SendMessageW(button, WM_SETFONT, (WPARAM)getStdFont(0), 0);
    m_btnCancel = button;
    m_result = E_FAIL;
    HANDLE thread = CreateThread(0, 0x2000, (LPTHREAD_START_ROUTINE)getWorkerProc(), this, 0x20, 0);
    FIELD(HANDLE, this, 0xC4) = thread;
    return thread != 0;
}

int ArchiveDialog::mapCommandToResult(int id)
{
    if (id == 2 && FIELD(HANDLE, this, 0xC4))
    {
        EnableWindow(m_btnCancel, FALSE);
        m_continue = false;
        return -1;
    }
    return id;
}
