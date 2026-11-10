#include "DropDown.h"
#include "GDC.h"
#include "PaintDC.h"
#include "WindowDC.h"
#include "gdi_cache.h"
#include "_imports_p10.h"

void DropDown::onRegisterClass(WNDCLASSEXW* wcx)
{
    wcx->style = CS_VREDRAW | CS_HREDRAW | CS_SAVEBITS;
}

BOOL DropDown::show(bool show)
{
    if (show)
    {
        if (m_skipShow)
        {
            m_skipShow = false;
        }
        else
        {
            m_hot = m_selected;
            m_hit = false;
            RECT rc;
            SetRectEmpty(&rc);
            GetWindowRect(m_owner, &rc);
            SetWindowPos(m_hWnd, 0, rc.left, rc.bottom, 0, 0, SWP_NOSIZE);
            ShowWindow(m_hWnd, SW_SHOW);
            SendMessageW(GetParent(m_hWnd), WM_NCACTIVATE, TRUE, 0);
            if (!GetCapture())
                SetCapture(m_hWnd);
        }
    }
    else
    {
        if (GetCapture() == m_hWnd)
            ReleaseCapture();
        ShowWindow(m_hWnd, SW_HIDE);
    }
    return 0;
}

LRESULT DropDown::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg != WM_NCPAINT)
    {
        if (msg == WM_PAINT)
        {
            PaintDC dc(m_hWnd);
            RECT rc;
            CopyRect(&rc, &dc.m_ps.rcPaint);
            paint(&dc, &rc);
            return 0;
        }
        if (msg != WM_PRINT && msg != WM_PRINTCLIENT)
            return DefWindowProcW(m_hWnd, msg, wParam, lParam);
        GDC dc((HDC)wParam);
        print((HDC)wParam, (DWORD)lParam);
        return 0;
    }
    HDC hdc = GetDCEx(m_hWnd, (HRGN)wParam, DCX_WINDOW | DCX_INTERSECTRGN | 0x10000 );
    if (!hdc)
    {
        WindowDC dc(m_hWnd);
        drawFrame(&dc, true);
        return 0;
    }
    {
        GDC dc(hdc);
        drawFrame(&dc, true);
    }
    ReleaseDC(m_hWnd, hdc);
    return 0;
}

bool DropDown::onKey(WPARAM vk, LPARAM flags, bool down)
{
    if (!down || (short)vk != VK_ESCAPE)
        return false;
    show(false);
    return true;
}

bool DropDown::onMouseMove(WPARAM keys, POINT* pt)
{
    int index = hitTest(pt);
    if (index != -2 && m_hot != index)
    {
        RECT rc;
        SetRectEmpty(&rc);
        int hot = m_hot;
        if (hot != -1)
        {
            LONG left = hot % m_columns * m_cellW;
            int cellH = m_cellH;
            LONG top = hot / m_columns * cellH;
            rc.right = m_cellW + left;
            rc.top = top;
            rc.bottom = cellH + top;
            rc.left = left;
            invalidateRect(&rc, false);
        }
        m_hot = index;
        if (index != -1)
        {
            int row = index / m_columns;
            int col = index % m_columns;
            int cellH = m_cellH;
            LONG left = col * m_cellW;
            LONG top = row * cellH;
            rc.right = m_cellW + left;
            rc.bottom = cellH + row * cellH;
            rc.left = left;
            rc.top = top;
            invalidateRect(&rc, false);
        }
    }
    return false;
}

bool DropDown::onLButton(WPARAM keys, POINT* pt, bool down)
{
    int index = hitTest(pt);
    m_selected = index;
    if (down)
    {
        if (index < 0)
        {
            ClientToScreen(m_hWnd, pt);
            ScreenToClient(m_owner, pt);
            RECT rc;
            GetClientRect(m_owner, &rc);
            if (PtInRect(&rc, *pt))
                m_skipShow = true;
            show(false);
        }
    }
    else if (index >= 0)
    {
        show(false);
        PostMessageW(m_owner, WM_COMMAND, (WORD)(UINT_PTR)m_childId | 0x10000, (LPARAM)m_hWnd);
        return true;
    }
    return true;
}

bool DropDown::onRButton(WPARAM keys, POINT* pt, bool down)
{
    show(false);
    return false;
}

int DropDown::hitTest(POINT* pt)
{
    RECT cell;
    RECT client;
    SetRect(&cell, 0, 0, m_cellW, m_cellH);
    RECT* c = getClientRect(&client);
    int width = c->right - c->left;
    int index = 0;
    int count = getItemCount();
    if (count <= 0)
        return (m_hit != 0) - 2;
    while (!PtInRect(&cell, *pt))
    {
        LONG right = m_cellW + cell.right;
        LONG left = m_cellW + cell.left;
        cell.right = right;
        cell.left = left;
        if (right > width)
        {
            int cellH = m_cellH;
            cell.top += cellH;
            cell.bottom += cellH;
            cell.left = 0;
            cell.right = right - left;
        }
        if (++index >= count)
            return (m_hit != 0) - 2;
    }
    m_hit = true;
    return index;
}

BOOL DropDown::drawFrame(GDC* dc, bool excludeClient)
{
    RECT rc;
    getWindowRect(&rc);
    if (excludeClient)
    {
        POINT origin;
        origin.x = rc.left;
        origin.y = rc.top;
        ScreenToClient(m_hWnd, &origin);
        RECT client;
        getClientRect(&client);
        client.left -= origin.x;
        client.right -= origin.x;
        client.top -= origin.y;
        client.bottom -= origin.y;
        ExcludeClipRect(dc->m_hdc, client.left, client.top, client.right, client.bottom);
    }
    rc.right -= rc.left;
    rc.bottom -= rc.top;
    rc.left = 0;
    rc.top = 0;
    COLORREF color = 0xB99D7F;
    if (isClassicAppearance())
        color = 0;
    return dc->frameRect(&rc, color);
}

void DropDown::print(HDC hdc, DWORD flags)
{
    if ((flags & PRF_CHECKVISIBLE) == 0 || IsWindowVisible(m_hWnd))
    {
        if (flags & PRF_NONCLIENT)
        {
            GDC dc(hdc);
            RECT rc;
            getWindowRect(&rc);
            rc.right -= rc.left;
            rc.bottom -= rc.top;
            rc.left = 0;
            rc.top = 0;
            COLORREF color = 0xB99D7F;
            if (isClassicAppearance())
                color = 0;
            dc.frameRect(&rc, color);
        }
        if (flags & (PRF_CLIENT | PRF_CHILDREN | PRF_OWNED))
        {
            GDC dc(hdc);
            RECT rc;
            paint(&dc, getClientRect(&rc));
        }
    }
}

void DropDown::paint(GDC* dc, RECT* rc)
{
    FillRect(dc->m_hdc, rc, getSolidBrush(0));
    drawBackground(dc);
    RECT cell;
    SetRectEmpty(&cell);
    int count = getItemCount();
    for (int i = 0; i < count; ++i)
    {
        int row = i / m_columns;
        int col = i % m_columns;
        int cellW = m_cellW;
        cell.left = col * cellW;
        int cellH = m_cellH;
        cell.right = cellW + cell.left;
        cell.bottom = cellH + row * cellH;
        cell.top = row * cellH;
        RECT inter;
        if (IntersectRect(&inter, rc, &cell))
            drawItem(dc, i, &cell);
    }
}
