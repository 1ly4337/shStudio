#include "TabControl.h"
#include "_imports_p06.h"

LPRECT TabControl::getClientRect(LPRECT rc)
{
    HWND hwnd = m_hWnd;
    SetRectEmpty(rc);
    GetClientRect(hwnd, rc);
    ++rc->left;
    rc->right -= 3;
    rc->top += 21;
    if ((GetWindowLongW(m_hWnd, GWL_STYLE) & TCS_MULTILINE) != 0)
        rc->top += 18;
    rc->bottom -= 2;
    return rc;
}

LPCWSTR TabControl::getClassName()
{
    return L"SysTabControl32";
}

DWORD TabControl::getStyle()
{
    return getTabStyle() | 0x14020000;
}
