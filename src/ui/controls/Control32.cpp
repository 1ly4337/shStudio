#include "p05_common.h"
#include "Control32.h"

BOOL Control32::enable(bool on)
{
    return EnableWindow(m_hWnd, on);
}

unsigned int Control32::isDisabled()
{
    return ((unsigned int)GetWindowLongW(m_hWnd, GWL_STYLE) >> 27) & 1;
}

BOOL Control32::isEnabled()
{
    return IsWindowEnabled(m_hWnd);
}

BOOL Control32::invalidate()
{
    return InvalidateRect(m_hWnd, NULL, TRUE);
}

BOOL Control32::move(int x, int y)
{
    return SetWindowPos(m_hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

BOOL Control32::resize(int cx, int cy)
{
    return SetWindowPos(m_hWnd, NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
}

BOOL Control32::show(bool on)
{
    int cmd = 0;
    if (on)
        cmd = SW_SHOW;
    return ShowWindow(m_hWnd, cmd);
}

LRESULT Control32::setText(const WCHAR* text)
{
    const WCHAR* s = L"";
    if (text)
        s = text;
    return SendMessageW(m_hWnd, WM_SETTEXT, 0, (LPARAM)s);
}

LPRECT Control32::getClientRect(LPRECT rc)
{
    rc->left = 0;
    rc->top = 0;
    rc->right = 0;
    rc->bottom = 0;
    GetClientRect(m_hWnd, rc);
    return rc;
}

LRESULT Control32::getTextLength()
{
    return SendMessageW(m_hWnd, WM_GETTEXTLENGTH, 0, 0);
}
