#include "Popup.h"
#include "Application.h"
#include <commctrl.h>
#include <cstdio>
#include "menus.h"
#include "_imports_p10.h"

Popup* Popup::findDocPopup(HWND hWnd)
{
    Window* window = Window::fromHwnd(hWnd, true);
    Popup* popup = dynamic_cast<Popup*>(window);
    if (popup && dynamic_cast<Application*>(popup))
        return 0;
    return popup;
}

bool Popup::createFrame(const RECT* rect, WORD menuId, LPCWSTR name)
{
    HMENU menu = LoadMenuW(hModule, MAKEINTRESOURCEW(menuId));
    m_hMenu = menu;
    return Window::create(0, menu, rect, 0, 0, name);
}

RECT* Popup::getClientRect(RECT* out)
{
    RECT rc;
    ::GetClientRect(m_hWnd, &rc);
    rc.bottom -= m_statusHeight;
    CopyRect(out, &rc);
    return out;
}

bool Popup::onCreate()
{
    LONG style = GetWindowLongW(m_hWnd, GWL_STYLE);
    HWND status = CreateWindowExW(0, L"msctls_statusbar32", L"Ready",
                                  ((DWORD)style >> 10) & SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE,
                                  (int)0x80000000, (int)0x80000000, (int)0x80000000, (int)0x80000000,
                                  m_hWnd, (HMENU)1001, hModule, 0);
    m_hStatusBar = status;
    if (status)
    {
        SIZE size;
        getSize(&size);
        HWND progress = CreateWindowExW(0, L"msctls_progress32", 0, WS_CHILD | PBS_SMOOTH, 250, 6,
                                        size.cx - 274, 12, m_hStatusBar, 0, hModule, 0);
        HWND bar = m_hStatusBar;
        m_hProgress = progress;
        RECT rc;
        SetRectEmpty(&rc);
        ::GetClientRect(bar, &rc);
        m_statusHeight = rc.bottom;
    }
    if (m_hMenu)
        updateMenu(m_hMenu);
    return true;
}

bool Popup::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id < 0xE100)
        return false;
    HWND child = GetDlgItem(m_hWnd, 101);
    if (!child)
        return false;
    SendMessageW(child, WM_COMMAND, (WORD)id | ((WORD)code << 16), (LPARAM)ctrl);
    return true;
}

bool Popup::onInitMenu(HMENU menu)
{
    setStatusText(0, 0);
    HWND child = GetDlgItem(m_hWnd, 101);
    if (child)
        SendMessageW(child, WM_INITMENU, (WPARAM)menu, 0);
    return true;
}

void Popup::onSize(WPARAM type, int cx, int cy)
{
    int height = cy;
    if (m_hStatusBar)
    {
        int statusHeight = m_statusHeight;
        height = cy - statusHeight;
        SetWindowPos(m_hStatusBar, 0, 0, cy - statusHeight, cx, statusHeight, SWP_NOZORDER);
        SetWindowPos(m_hProgress, 0, 250, 6, cx - 274, 12, SWP_NOZORDER);
    }
    HWND child = GetDlgItem(m_hWnd, 101);
    if (child)
        SetWindowPos(child, 0, 0, 0, cx, height, SWP_NOMOVE | SWP_NOZORDER);
}

bool Popup::onTimer(UINT_PTR id)
{
    if (id != 130)
        return false;
    SetForegroundWindow(m_hWnd);
    UpdateWindow(m_hWnd);
    return true;
}

void Popup::setStatusText(LPCWSTR fmt, INT_PTR arg)
{
    if (!fmt)
    {
        if (IsWindowVisible(m_hProgress))
            ShowWindow(m_hProgress, SW_HIDE);
        SetWindowTextW(m_hStatusBar, L"Ready");
        return;
    }
    if (!arg)
    {
        SetWindowTextW(m_hStatusBar, fmt);
        InvalidateRect(m_hStatusBar, 0, TRUE);
        UpdateWindow(m_hStatusBar);
        return;
    }
    wchar_t* buffer = new wchar_t[0x400];
    if (buffer)
    {
        memset(buffer, 0, 0x400);
        swprintf_s(buffer, 0x400, fmt, arg);
        SetWindowTextW(m_hStatusBar, buffer);
        delete[] buffer;
    }
}

LRESULT Popup::setProgress(WPARAM pos, LPARAM range)
{
    if (range <= 0)
    {
        setStatusText(0, 0);
        return 0;
    }
    if (!IsWindowVisible(m_hProgress))
    {
        ShowWindow(m_hProgress, SW_SHOW);
        SendMessageW(m_hProgress, PBM_SETRANGE32, 0, range);
    }
    return SendMessageW(m_hProgress, PBM_SETPOS, pos, 0);
}

void Popup::updateMenu(HMENU menu)
{
    EnableMenuItem(menu, 0xE102, MF_ENABLED);
    EnableMenuItem(menu, 0xE103, MF_ENABLED);
    updateScriptMenu(menu);
}

HMENU Popup::updateScriptMenu(HMENU menu)
{
    HMENU sub = GetSubMenu(menu, 2);
    HMENU scripts = findScriptsMenu(sub);
    if (!scripts)
        return scripts;
    clearMenu(scripts);
    fillScriptMenu(scripts);
    if (!GetMenuItemCount(scripts))
    {
        MENUITEMINFOW mi;
        memset(&mi, 0, sizeof(mi));
        mi.cbSize = sizeof(MENUITEMINFOW);
        mi.fMask = MIIM_STATE | MIIM_ID | MIIM_STRING;
        mi.fType = 0;
        mi.wID = 1;
        mi.fState = MF_GRAYED;
        mi.dwTypeData = (LPWSTR)L"no script";
        InsertMenuItemW(scripts, 0xFFFFFFFF, FALSE, &mi);
    }
    return (HMENU)(INT_PTR)DrawMenuBar(m_hWnd);
}
