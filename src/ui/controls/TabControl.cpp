#include "TabControl.h"
#include "Window.h"
#include "dark_mode.h"
#include "_imports_p10.h"

TabControl::~TabControl()
{
    if (m_gradient)
        DeleteObject(m_gradient);
}

LPRECT TabControl::getTabsRect(LPRECT rc)
{
    HWND hWnd = m_hWnd;
    SetRectEmpty(rc);
    GetClientRect(hWnd, rc);
    rc->bottom = 20;
    if (SendMessageW(m_hWnd, TCM_GETROWCOUNT, 0, 0) > 1)
        rc->bottom += 18;
    return rc;
}

bool TabControl::addPage(LPCWSTR text, HWND page, int index)
{
    if (!m_hWnd || !page)
        return false;
    SendMessageW(page, WM_SETFONT, (WPARAM)getStdFont(0), 1);
    return insertPage(text, page, (LPARAM)page, index);
}

bool TabControl::insertPage(LPCWSTR text, HWND page, LPARAM data, int index)
{
    if (!m_hWnd || !page)
        return false;
    RECT rc;
    getClientRect(&rc);
    SetWindowPos(page, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_HIDEWINDOW);

    TCITEMW item;
    item.mask = TCIF_TEXT | TCIF_PARAM;
    item.dwState = 0;
    item.dwStateMask = 0;
    item.pszText = (LPWSTR)text;
    item.cchTextMax = 0;
    item.iImage = 0;
    item.lParam = data;
    WPARAM at = (WPARAM)index;
    if (index < 0)
        at = SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0);
    if (SendMessageW(m_hWnd, TCM_INSERTITEMW, at, (LPARAM)&item) == -1)
        return false;
    if (m_useGradient && !m_gradient)
    {
        RECT tab;
        SendMessageW(m_hWnd, TCM_GETITEMRECT, 0, (LPARAM)&tab);
        if (darkModeOn())
            m_gradient = createGradientBitmap(tab.bottom, dmBk(0xF0F0F0), dmBk(0xEEEEEE));
        else
            m_gradient = createGradientBitmap(tab.bottom, 0xF8F8F8, 0xEEEEEE);
    }
    return true;
}

HWND TabControl::getPage(int index)
{
    WPARAM at = (WPARAM)index;
    if (index == -1)
        at = SendMessageW(m_hWnd, TCM_GETCURSEL, 0, 0);
    TCITEMW item;
    item.dwState = 0;
    item.dwStateMask = 0;
    item.pszText = NULL;
    item.cchTextMax = 0;
    item.iImage = 0;
    item.lParam = 0;
    item.mask = TCIF_PARAM;
    LRESULT ok = SendMessageW(m_hWnd, TCM_GETITEMW, at, (LPARAM)&item);
    if (ok != 1)
        return NULL;
    return (HWND)item.lParam;
}

HWND TabControl::removePage(int index)
{
    HWND page = getPage(index);
    if (page)
    {
        SendMessageW(m_hWnd, TCM_DELETEITEM, (WPARAM)index, 0);
        return page;
    }
    return page;
}

bool TabControl::destroyPage(int index)
{
    HWND page = removePage(index);
    if (!page)
        return false;
    DestroyWindow(page);
    return true;
}

bool TabControl::select(int index)
{
    if (!m_hWnd || SendMessageW(m_hWnd, TCM_SETCURSEL, (WPARAM)index, 0) == -1)
        return false;
    if (m_currentPage)
        ShowWindow(m_currentPage, SW_HIDE);
    m_currentPage = getPage(index);
    if (m_currentPage)
        ShowWindow(m_currentPage, SW_SHOW);
    return true;
}

bool TabControl::onCreate()
{
    setFont(getStdFont(0));
    RECT rc;
    if (!createPages(getClientRect(&rc)))
        return false;
    onSize(0, 0, 0);
    select(0);
    return true;
}

void TabControl::onFocus(bool focused)
{
    HWND page = getPage(-1);
    if (page)
        SendMessageW(page, (focused == false) + WM_SETFOCUS, 0, 0);
}

bool TabControl::onRButton(int keys, POINT* pt, bool down)
{
    if (!down)
        return false;
    int cur = (int)SendMessageW(m_hWnd, TCM_GETCURSEL, 0, 0);
    RECT tab;
    SendMessageW(m_hWnd, TCM_GETITEMRECT, (WPARAM)cur, (LPARAM)&tab);
    if (!PtInRect(&tab, *pt))
        return false;

    HWND target = getPage(cur);
    LRESULT menuId = SendMessageW(target, 0x407, (WPARAM)GetDlgCtrlID(target), (LPARAM)target);
    if (menuId <= 0)
    {
        target = m_hWnd;
        menuId = SendMessageW(target, 0x407, (WPARAM)GetDlgCtrlID(target), (LPARAM)target);
        if (menuId <= 0)
            return false;
    }
    HMENU menu = LoadMenuW(NULL, MAKEINTRESOURCEW((WORD)menuId));
    if (!menu)
        return false;
    HMENU popup = GetSubMenu(menu, 0);
    if (popup)
    {
        SetMenuDefaultItem(popup, (UINT)-1, 1);
        SendMessageW(target, 0x408, (WPARAM)popup, (LPARAM)target);
        ClientToScreen(m_hWnd, pt);
        TrackPopupMenu(popup, 0, pt->x, pt->y, 0, target, NULL);
    }
    DestroyMenu(menu);
    return true;
}

bool TabControl::onCommand(int code, LPARAM ctrl, int id)
{
    HWND page = getPage(-1);
    if (!page)
    {
        page = m_currentPage;
        if (!page)
            return false;
    }
    if ((code | 1) != 1)
        return false;
    SendMessageW(page, WM_COMMAND, id | ((WORD)code << 16), ctrl);
    return true;
}

bool TabControl::onInitMenu(WPARAM menu)
{
    HWND page = getPage(-1);
    if (page)
        SendMessageW(page, WM_INITMENU, menu, 0);
    return true;
}

bool TabControl::onNotify(NMHDR* hdr, LRESULT* result)
{
    UINT code = hdr->code;
    if (code == (UINT)-121)
    {
        *result = 1;
        return true;
    }
    if (code == (UINT)TCN_SELCHANGE)
    {
        int cur = (int)SendMessageW(m_hWnd, TCM_GETCURSEL, 0, 0);
        m_currentPage = getPage(cur);
        if (m_currentPage)
        {
            ShowWindow(m_currentPage, SW_SHOW);
            SetFocus(m_currentPage);
        }
        onPageChanged(cur);
        return true;
    }
    if (code == (UINT)TCN_SELCHANGING)
    {
        int cur = (int)SendMessageW(m_hWnd, TCM_GETCURSEL, 0, 0);
        if (canLeavePage(cur))
        {
            if (m_currentPage)
                ShowWindow(m_currentPage, SW_HIDE);
            *result = 0;
            return true;
        }
        *result = 1;
        return true;
    }
    HWND from;
    Control* ctrl;
    if (hdr->idFrom && (from = hdr->hwndFrom) != NULL && from != m_hWnd && code != (UINT)-155
        && (ctrl = Control::fromHandle(from)) != NULL)
    {
        return ctrl->onNotify(hdr, result);
    }
    return false;
}

void TabControl::onSize(int type, int cx, int cy)
{
    RECT rc;
    getClientRect(&rc);
    int count = (int)SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0);
    for (int i = 0; i < count; ++i)
    {
        HWND page = getPage(i);
        if (page)
            SetWindowPos(page, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
    }
}

LRESULT TabControl::subclassProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_PAINT)
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hWnd, &ps);
        paint(hdc);
        EndPaint(m_hWnd, &ps);
        return 0;
    }
    if (msg == WM_ERASEBKGND)
        return 1;
    return DefSubclassProc(m_hWnd, msg, wParam, lParam);
}

BOOL TabControl::paint(HDC hdc)
{
    DefSubclassProc(m_hWnd, WM_PRINTCLIENT, (WPARAM)hdc, PRF_CLIENT);
    HRGN tabs = CreateRectRgn(0, 0, 0, 0);
    int count = (int)SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0);
    int cur = (int)SendMessageW(m_hWnd, TCM_GETCURSEL, 0, 0);
    for (int i = 0; i < count; ++i)
    {
        RECT r;
        SendMessageW(m_hWnd, TCM_GETITEMRECT, (WPARAM)i, (LPARAM)&r);
        if (i == cur)
        {
            r.left -= 2;
            r.right += 2;
            r.top -= 2;
        }
        else
        {
            r.right -= 1;
            if (i == count - 1)
                r.right -= 1;
        }
        HRGN item = CreateRectRgn(r.left, r.top, r.right, r.bottom);
        CombineRgn(tabs, tabs, item, RGN_OR);
        DeleteObject(item);
    }

    RECT tmp;
    RECT rc = *getTabsRect(&tmp);
    HRGN clip = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
    CombineRgn(clip, clip, tabs, RGN_DIFF);
    SelectClipRgn(hdc, clip);
    if (m_useGradient && m_gradient)
    {
        HDC mem = CreateCompatibleDC(hdc);
        HGDIOBJ old = SelectObject(mem, m_gradient);
        StretchBlt(hdc, 0, 0, rc.right, rc.bottom, mem, 0, 0, 8, rc.bottom, SRCCOPY);
        SelectObject(mem, old);
        DeleteDC(mem);
        DeleteObject(clip);
    }
    else
    {
        HWND parent = GetParent(m_hWnd);
        DWORD proc = GetClassLongW(parent, GCL_WNDPROC);
        LONG wnd;
        COLORREF color;
        if (proc && proc == (DWORD)(DWORD_PTR)&Window::wndProc
            && ((wnd = GetWindowLongW(parent, GWL_USERDATA)) != 0 || (wnd = GetWindowLongW(parent, 8)) != 0))
            color = ((Window*)wnd)->m_bkColor;
        else
            color = 0xFFFFFF;
        HBRUSH brush = CreateSolidBrush(color);
        UnrealizeObject(brush);
        FillRgn(hdc, clip, brush);
        DeleteObject(brush);
        DeleteObject(clip);
    }
    return DeleteObject(tabs);
}
