#include <windows.h>
#include <cstring>
#include "Preferences.h"
#include "_imports_p09.h"
#include "_imports_g5.h"

Preferences g_preferences;

Preferences::Preferences()
{
    m_left = getInt(m_appName.m_buffer, L"left", 50);
    m_top = getInt(m_appName.m_buffer, L"top", 50);
    m_width = getInt(m_appName.m_buffer, L"width", 480);
    m_height = getInt(m_appName.m_buffer, L"height", 300);
    m_display = getInt(m_appName.m_buffer, L"display", 1);
}

Preferences::~Preferences()
{
    writeInt(m_appName.m_buffer, L"left", m_left);
    writeInt(m_appName.m_buffer, L"top", m_top);
    writeInt(m_appName.m_buffer, L"width", m_width);
    writeInt(m_appName.m_buffer, L"height", m_height);
    writeInt(m_appName.m_buffer, L"display", m_display);
}

void Preferences::saveWindowPlacement(HWND hWnd)
{
    if (!IsWindow(hWnd))
        return;
    WINDOWINFO info;
    memset(&info, 0, sizeof(info));
    info.cbSize = 60;
    GetWindowInfo(hWnd, &info);
    g_preferences.m_display = (info.dwStyle & WS_MAXIMIZE | 0x800000) >> 23;
    if (info.rcWindow.top > 0 && info.rcWindow.left > 0)
    {
        g_preferences.m_width = info.rcWindow.right - info.rcWindow.left;
        g_preferences.m_left = info.rcWindow.left;
        g_preferences.m_top = info.rcWindow.top;
        g_preferences.m_height = info.rcWindow.bottom - info.rcWindow.top;
    }
}
