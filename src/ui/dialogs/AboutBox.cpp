#include "p05_common.h"
#include "AboutBox.h"
#include "String.hpp"
#include "VersionInfo.h"

bool AboutBox::create(HWND parent)
{
    m_buttons = 0;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 310, 120);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"About...");
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

bool AboutBox::onCreate()
{
    HWND hWnd = m_hWnd;
    HANDLE icon = LoadImageW(hModule, (LPCWSTR)0x69, IMAGE_ICON, 48, 48, LR_DEFAULTSIZE);
    HWND iconWnd = CreateWindowExW(0, L"Static", 0, 0x50000003, 15, 5, 48, 48, hWnd, 0, 0, 0);
    if (iconWnd)
        SendMessageW(iconWnd, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    SendMessageW(iconWnd, STM_SETICON, (WPARAM)icon, 0);
    WPARAM boldFont = (WPARAM)getStdFont(0);

    VersionInfo version(hModule);
    String title;
    title.append((const wchar_t*)&g_productName, 16);
    {
        String v = version.fileVersion();
        title.append(v.m_buffer, v.m_length);
    }
    HWND nameWnd = CreateWindowExW(0, L"Static", title.m_buffer, WS_CHILD | WS_VISIBLE, 70, 15, 200, 18,
                                   m_hWnd, 0, 0, 0);
    if (nameWnd)
        SendMessageW(nameWnd, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    SendMessageW(nameWnd, WM_SETFONT, boldFont, 0);
    HWND copyWnd = CreateWindowExW(0, L"Static", g_caption, WS_CHILD | WS_VISIBLE, 70, 32, 200, 18,
                                   m_hWnd, 0, 0, 0);
    if (copyWnd)
        SendMessageW(copyWnd, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    SendMessageW(copyWnd, WM_SETFONT, boldFont, 0);
    return Modal::onCreate();
}
