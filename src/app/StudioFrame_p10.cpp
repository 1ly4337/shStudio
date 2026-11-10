#include "StudioFrame.h"
#include "Application.h"
#include "String.hpp"
#include "Clipboard.h"
#include "Preferences.h"
#include <gdiplus.h>
#include "_imports_p10.h"

bool StudioFrame::open(int unused1, int unused2, int unused3)
{
    if (!g_gdiplusToken)
    {
        Gdiplus::GdiplusStartupInput input;
        input.GdiplusVersion = 1;
        input.DebugEventCallback = 0;
        input.SuppressBackgroundThread = FALSE;
        input.SuppressExternalCodecs = FALSE;
        if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &input, 0))
            return false;
    }
    g_appName = L"shStudio 10";
    m_iconId = 105;
    SIZE size;
    size.cx = g_preferences.m_width;
    size.cy = g_preferences.m_height;
    adjustInitialSize(&size);
    RECT rc;
    SetRect(&rc, g_preferences.m_left, g_preferences.m_top, g_preferences.m_left + size.cx, g_preferences.m_top + size.cy);
    HMENU menu = LoadMenuW(hModule, MAKEINTRESOURCEW(0x80));
    m_hMenu = menu;
    if (!Window::create(0, menu, &rc, 0, 0, L"shStudio 10"))
        return false;
    m_hAccel = LoadAcceleratorsW(hModule, MAKEINTRESOURCEW(0x80));
    g_clipboard.m_hOwner = m_hWnd;
    g_mainFrame = this;
    g_hMainWnd = g_clipboard.m_hOwner;
    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);
    SetTimer(m_hWnd, 0x80, 50, 0);
    return true;
}
