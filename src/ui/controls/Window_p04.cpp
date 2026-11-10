#include <windows.h>
#include <commctrl.h>
#include "Window.h"

void Window::activate(bool active)
{
    onActivate(active);
}

BOOL Window::enable(bool enable)
{
    return EnableWindow(m_hWnd, enable != 0);
}

BOOL Window::invalidateRect(const RECT* rect, bool erase)
{
    return InvalidateRect(m_hWnd, rect, erase != 0);
}

BOOL Window::invalidate(bool erase)
{
    return InvalidateRect(m_hWnd, NULL, erase != 0);
}

BOOL Window::move(int x, int y)
{
    return SetWindowPos(m_hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

LRESULT Window::setRedraw(bool redraw)
{
    return SendMessageW(m_hWnd, WM_SETREDRAW, redraw != 0, 0);
}

BOOL Window::resize(int cx, int cy)
{
    return SetWindowPos(m_hWnd, NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
}

BOOL Window::show(bool show)
{
    int cmd = SW_HIDE;
    if (show)
        cmd = SW_SHOW;
    return ShowWindow(m_hWnd, cmd);
}

BOOL Window::isVisible()
{
    return IsWindowVisible(m_hWnd);
}

LRESULT Window::setText(LPCWSTR text)
{
    return SendMessageW(m_hWnd, WM_SETTEXT, 0, (LPARAM)text);
}

LRESULT Window::setFont(HFONT font)
{
    return SendMessageW(m_hWnd, WM_SETFONT, (WPARAM)font, TRUE);
}

void Window::modifyStyle(DWORD add, DWORD remove)
{
    HWND hWnd = m_hWnd;
    if (hWnd)
    {
        LONG style = GetWindowLongW(hWnd, GWL_STYLE);
        SetWindowLongW(hWnd, GWL_STYLE, add | (~remove & style));
        if (((remove | add) & 0xFFFF0000) != 0)
            SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
                         SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
}

void Window::modifyExStyle(DWORD add, DWORD remove)
{
    HWND hWnd = m_hWnd;
    if (hWnd)
    {
        LONG exStyle = GetWindowLongW(hWnd, GWL_EXSTYLE);
        SetWindowLongW(hWnd, GWL_EXSTYLE, add | (~remove & exStyle));
        UpdateWindow(hWnd);
    }
}

LRESULT Window::setWindowTheme(LPCWSTR theme)
{
    return SendMessageW(m_hWnd, CCM_SETWINDOWTHEME, 0, (LPARAM)theme);
}

POINT* Window::getPosition(POINT* out)
{
    HWND hWnd = m_hWnd;
    RECT rc;
    RECT parentRc;
    SetRectEmpty(&rc);
    SetRectEmpty(&parentRc);
    GetWindowRect(hWnd, &rc);
    HWND parent = GetParent(hWnd);
    if (parent)
        GetWindowRect(parent, &parentRc);
    out->x = rc.left - parentRc.left;
    out->y = rc.top - parentRc.top;
    return out;
}

SIZE* Window::getSize(SIZE* out)
{
    HWND hWnd = m_hWnd;
    RECT rc;
    SetRectEmpty(&rc);
    GetWindowRect(hWnd, &rc);
    out->cx = rc.right - rc.left;
    out->cy = rc.bottom - rc.top;
    return out;
}

RECT* Window::getClientRect(RECT* out)
{
    HWND hWnd = m_hWnd;
    SetRectEmpty(out);
    GetClientRect(hWnd, out);
    return out;
}

RECT* Window::getRectInParent(RECT* out)
{
    HWND parent = GetParent(m_hWnd);
    HWND hWnd = m_hWnd;
    RECT parentRc;
    SetRectEmpty(out);
    SetRectEmpty(&parentRc);
    GetWindowRect(hWnd, out);
    GetWindowRect(parent, &parentRc);
    out->left -= parentRc.left;
    out->right += -parentRc.left;
    out->top += -parentRc.top;
    out->bottom += -parentRc.top;
    return out;
}

RECT* Window::getWindowRect(RECT* out)
{
    HWND hWnd = m_hWnd;
    SetRectEmpty(out);
    GetWindowRect(hWnd, out);
    return out;
}
