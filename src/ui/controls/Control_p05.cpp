#include "p05_common.h"
#include "Control.hpp"

BOOL Control::enable(bool on)
{
    return EnableWindow(m_hWnd, on);
}

BOOL Control::isEnabled()
{
    return IsWindowEnabled(m_hWnd);
}

BOOL Control::move(int x, int y)
{
    return SetWindowPos(m_hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

BOOL Control::resize(int cx, int cy)
{
    return SetWindowPos(m_hWnd, NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
}

LRESULT Control::setRedraw(bool on)
{
    return SendMessageW(m_hWnd, WM_SETREDRAW, on, 0);
}

BOOL Control::show(bool on)
{
    int cmd = 0;
    if (on)
        cmd = SW_SHOW;
    return ShowWindow(m_hWnd, cmd);
}

BOOL Control::isVisible()
{
    return IsWindowVisible(m_hWnd);
}

LRESULT Control::setFont(HFONT font)
{
    return SendMessageW(m_hWnd, WM_SETFONT, (WPARAM)font, TRUE);
}

LRESULT Control::setWindowTheme(LPCWSTR theme)
{
    return SendMessageW(m_hWnd, CCM_SETWINDOWTHEME, 0, (LPARAM)theme);
}

POINT* Control::getPosition(POINT* out)
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

SIZE* Control::getSize(SIZE* out)
{
    HWND hWnd = m_hWnd;
    RECT rc;
    SetRectEmpty(&rc);
    GetWindowRect(hWnd, &rc);
    out->cx = rc.right - rc.left;
    out->cy = rc.bottom - rc.top;
    return out;
}

LPRECT Control::getClientRect(LPRECT rc)
{
    HWND hWnd = m_hWnd;
    SetRectEmpty(rc);
    GetClientRect(hWnd, rc);
    return rc;
}

LPRECT Control::getWindowRect(LPRECT rc)
{
    HWND hWnd = m_hWnd;
    SetRectEmpty(rc);
    GetWindowRect(hWnd, rc);
    return rc;
}

LPRECT Control::getRectInParent(LPRECT rc)
{
    HWND parent = GetParent(m_hWnd);
    HWND hWnd = m_hWnd;
    RECT parentRc;
    SetRectEmpty(rc);
    SetRectEmpty(&parentRc);
    GetWindowRect(hWnd, rc);
    GetWindowRect(parent, &parentRc);
    rc->left -= parentRc.left;
    rc->right -= parentRc.left;
    rc->top -= parentRc.top;
    rc->bottom -= parentRc.top;
    return rc;
}

DWORD Control::getStyle()
{
    return WS_VISIBLE;
}

int Control::vslot50(int )
{
    return E_FAIL;
}
