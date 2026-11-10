#include "Scroller.h"
#include "WindowDC.h"
#include "_imports_p10.h"

Scroller::Scroller()
{
    m_contentHeight = 0;
    m_contentWidth = 0;
    m_scrollY = 0;
    m_scrollX = 0;
    m_field48 = 0;
    m_field44 = 0;
    m_bkColor = Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0;
    m_contentHeight = 0;
    m_contentWidth = 0;
    m_scrollY = 0;
    m_scrollX = 0;
    m_updating = false;
}

LONG Scroller::updateScrollBars()
{
    LONG style = GetWindowLongW(m_hWnd, GWL_STYLE);
    LONG result = style;
    if ((style & (WS_HSCROLL | WS_VSCROLL)) == 0)
        return result;
    int oldX = m_scrollX;
    int oldY = m_scrollY;
    m_updating = true;
    RECT client;
    getClientRect(&client);
    int width = client.right;
    int height = client.bottom;
    SCROLLINFO si;
    memset(&si.nMin, 0, 20);
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_ALL;
    if ((style & WS_VSCROLL) != 0)
    {
        int range = m_contentHeight;
        int page = height;
        if (range < height)
            page = m_contentHeight;
        if (range < 0)
            range = 0;
        int pos = range - page;
        if (m_scrollY < range - page)
            pos = m_scrollY;
        si.nMax = range;
        if (page < range)
            range = page;
        m_scrollY = pos;
        si.nPos = pos;
        si.nPage = range;
        SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
    }
    if ((style & WS_HSCROLL) != 0)
    {
        int range = m_contentWidth;
        int page = width;
        if (range < width)
            page = m_contentWidth;
        if (range < 0)
            range = 0;
        int pos = range - page;
        int pageValue = range + 1;
        if (m_scrollX < range - page)
            pos = m_scrollX;
        if (page < pageValue)
            pageValue = page;
        si.nPage = pageValue;
        m_scrollX = pos;
        si.nMax = range;
        si.nPos = pos;
        SetScrollInfo(m_hWnd, SB_HORZ, &si, TRUE);
    }
    result = oldX;
    if (oldX > m_scrollX || oldY > m_scrollY)
        result = ScrollWindowEx(m_hWnd, oldX - m_scrollX, oldY - m_scrollY, 0, 0, 0, 0, SW_INVALIDATE);
    m_updating = false;
    return result;
}

LRESULT Scroller::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg != WM_NCPAINT)
        return DefWindowProcW(m_hWnd, msg, wParam, lParam);
    LRESULT result = DefWindowProcW(m_hWnd, WM_NCPAINT, wParam, lParam);
    if ((GetWindowLongW(m_hWnd, GWL_STYLE) & WS_BORDER) != 0)
    {
        WindowDC dc(m_hWnd);
        SIZE size;
        getSize(&size);
        int right = size.cx;
        int bottom = GetSystemMetrics(SM_CYVSCROLL) + size.cy;
        HPEN pen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
        dc.m_savedPen = SelectObject(dc.m_hdc, pen);
        --right;
        --bottom;
        MoveToEx(dc.m_hdc, 0, 0, 0);
        LineTo(dc.m_hdc, right, 0);
        LineTo(dc.m_hdc, right, bottom);
        LineTo(dc.m_hdc, 0, bottom);
        LineTo(dc.m_hdc, 0, 0);
    }
    return result;
}

bool Scroller::onMouseWheel(int delta, POINT* pt, UINT keys)
{
    int amount = delta;
    int code = SB_LINEUP;
    if (delta < 0)
    {
        code = SB_LINEDOWN;
        amount = -delta;
    }
    return onScroll(false, code, amount);
}

bool Scroller::onScroll(bool horizontal, int code, int pos)
{
    RECT client;
    getClientRect(&client);
    int width = client.right;
    int height = client.bottom;
    int content;
    int current;
    int range;
    int page;
    if (horizontal)
    {
        content = m_contentWidth;
        current = m_scrollX;
        range = content < 0 ? 0 : content;
        page = width;
    }
    else
    {
        content = m_contentHeight;
        current = m_scrollY;
        range = content < 0 ? 0 : content;
        page = height;
    }
    if (content < page)
        page = content;
    int step = getLineStep();
    int delta = 0;
    switch (code)
    {
    case SB_LINEUP:
        delta = -getLineStep();
        break;
    case SB_LINEDOWN:
        delta = getLineStep();
        break;
    case SB_PAGEUP:
        delta = -page;
        break;
    case SB_PAGEDOWN:
        delta = page;
        break;
    case SB_THUMBPOSITION:
        return true;
    case SB_THUMBTRACK:
        delta = MulDiv(step * ((step + pos - 1) / step) - current, range, range + page);
        break;
    case SB_TOP:
        delta = -current;
        break;
    case SB_BOTTOM:
        delta = range - current;
        break;
    case SB_ENDSCROLL:
        if (horizontal)
            return true;
        if (m_scrollY >= 16)
            return true;
        delta = -m_scrollY;
        break;
    default:
        break;
    }
    if (delta + current < 0)
        delta = -current;
    if (delta + current > range - page)
        delta = range - page - current;
    if (horizontal)
        ScrollWindow(m_hWnd, -delta, 0, 0, 0);
    else
        ScrollWindow(m_hWnd, 0, -delta, 0, 0);
    int newPos = delta + current;
    SetScrollPos(m_hWnd, horizontal ? SB_HORZ : SB_VERT, newPos, TRUE);
    if (horizontal)
        m_scrollX = newPos;
    else
        m_scrollY = newPos;
    return true;
}

BOOL Scroller::resetScroll()
{
    ScrollWindow(m_hWnd, m_scrollX, m_scrollY, 0, 0);
    return InvalidateRect(m_hWnd, 0, TRUE);
}

void Scroller::onSize(WPARAM type, int cx, int cy)
{
    if (!m_updating)
        updateScrollBars();
}
