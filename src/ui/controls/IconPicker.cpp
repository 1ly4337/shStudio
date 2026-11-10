#include <windows.h>
#include <commctrl.h>
#include <cmath>
#include <cstring>
#include "IconPicker.h"
#include "GDC.h"
#include "PaintDC.h"
#include "WindowDC.h"
#include "WinAppearance.h"
#include "_imports_p09.h"
#include "_imports_g6.h"

#ifndef DCX_USESTYLE
#define DCX_USESTYLE 0x00010000L
#endif

#pragma warning(disable : 4996)

IconPicker::IconPicker()
    : m_dropDown(&m_cells), m_columns(0), m_rows(0), m_cellWidth(0), m_cellHeight(0), m_selected(0),
      m_tracking(false)
{
    m_state = 0;
}

bool IconPicker::create(HWND parent, HMENU id, int x, int y, int cx, int cy, int iconSize, int unused)
{
    int count = m_cells.m_count;
    if (count <= 0)
        return false;
    int columns = m_columns;
    if (!columns)
    {
        int side = (int)std::sqrt((double)count);
        if (side < 5)
            columns = 6;
        else if (side <= 8)
            columns = 8;
        else if (side <= 16)
            columns = 16;
        else
            columns = 8 * (side > 24) + 24;
        m_columns = columns;
    }
    int rows = count / columns;
    m_rows = rows;
    if (rows * m_columns < count)
        m_rows = rows + 1;
    m_cellWidth = 32;
    m_cellHeight = 32;
    if (iconSize == 32)
    {
        m_icon.m_cx = 32;
        m_icon.m_cy = 32;
        m_icon.m_hList = ImageList_Create(32, 32, ILC_COLOR32, 64, 128);
        m_arrows.m_cx = 17;
        m_arrows.m_cy = 34;
        m_arrows.m_hList = ImageList_Create(17, 34, ILC_COLOR32, 64, 128);
        m_arrows.load(340, 0);
    }
    else if (iconSize == 64)
    {
        m_icon.m_cx = 64;
        m_icon.m_cy = 64;
        m_icon.m_hList = ImageList_Create(64, 64, ILC_COLOR32, 64, 128);
        m_arrows.m_cx = 17;
        m_arrows.m_cy = 60;
        m_arrows.m_hList = ImageList_Create(17, 60, ILC_COLOR32, 64, 128);
        m_arrows.load(341, 0);
    }
    RECT rc;
    SetRect(&rc, 20, y, cx + 40, y + cy + 2);
    return Window::create(parent, id, &rc, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, 0, 0);
}

int IconPicker::select(int id)
{
    int index = 0;
    int bound = m_bitmaps.m_ids.m_capacity;
    if (bound <= 0)
    {
        index = -1;
    }
    else
    {
        long* ids = m_bitmaps.m_ids.m_items;
        while (*ids != id)
        {
            ++index;
            ++ids;
            if (index >= bound)
            {
                index = -1;
                break;
            }
        }
    }
    m_selected = index;
    int result = showIcon(index);
    m_dropDown.m_selected = index;
    return result;
}

BOOL IconPicker::showIcon(int index)
{
    HBITMAP bitmap = m_bitmaps.getIcon(index);
    if (!bitmap)
        return 0;
    m_icon.removeAll();
    m_icon.addBitmap(bitmap);
    return DeleteObject(bitmap);
}

bool IconPicker::onCreate()
{
    int width = m_cellWidth * m_columns;
    HWND hWnd = m_hWnd;
    int height = m_rows * m_cellHeight;
    int dropDownId = (int)(INT_PTR)m_childId + 1000;
    HWND parent = GetParent(hWnd);
    m_dropDown.m_childId = (HMENU)(INT_PTR)dropDownId;
    m_dropDown.m_owner = hWnd;
    m_dropDown.m_cellW = m_cellWidth;
    m_dropDown.m_cellH = m_cellHeight;
    m_dropDown.m_columns = width / m_dropDown.m_cellW;
    int outerWidth = width + 2 * GetSystemMetrics(SM_CXBORDER);
    RECT rc;
    SetRect(&rc, 0, 0, outerWidth, height + 2 * GetSystemMetrics(SM_CYBORDER));
    m_dropDown.Window::create(parent, 0, &rc, WS_POPUP | WS_BORDER, 0, 0);
    m_dropDown.m_selected = m_selected;
    return true;
}

bool IconPicker::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code == 1)
    {
        int selection = m_dropDown.m_selected;
        m_selected = selection;
        showIcon(selection);
        invalidate(false);
        PostMessageW(GetParent(m_hWnd), WM_COMMAND, LOWORD(m_childId) | 0x10000, (LPARAM)m_hWnd);
    }
    return false;
}

bool IconPicker::onLButton(WPARAM keys, POINT* pt, bool down)
{
    if (down)
    {
        m_state |= 0xC00;
        invalidate(false);
        return true;
    }
    HWND dropDown = m_dropDown.m_hWnd;
    m_state = (m_state & 0xFFFFF3FF) | 0x400;
    if (IsWindowVisible(dropDown))
    {
        m_dropDown.show(false);
    }
    else
    {
        m_dropDown.m_selected = m_selected;
        m_dropDown.show(true);
    }
    invalidate(false);
    return true;
}

bool IconPicker::onMouseMove(WPARAM keys, POINT* pt)
{
    RECT rc;
    getClientRect(&rc);
    if (PtInRect(&rc, *pt))
    {
        if (!m_tracking)
        {
            TRACKMOUSEEVENT track;
            track.hwndTrack = m_hWnd;
            track.cbSize = sizeof(TRACKMOUSEEVENT);
            track.dwFlags = TME_LEAVE;
            track.dwHoverTime = HOVER_DEFAULT;
            BOOL tracking = TrackMouseEvent(&track);
            m_state |= 0x400;
            m_tracking = tracking != 0;
            invalidate(false);
            return true;
        }
    }
    else if (m_tracking)
    {
        onMouseLeave();
    }
    return true;
}

bool IconPicker::onMouseLeave()
{
    m_state &= 0xFFFFF3FF;
    m_tracking = false;
    invalidate(false);
    return true;
}

bool IconPicker::onKey(WPARAM vk, LPARAM flags, bool down)
{
    short key = (short)vk;
    if (down || (key != VK_F4 && key != VK_DOWN && key != VK_NEXT))
        return false;
    m_dropDown.m_selected = m_selected;
    m_dropDown.show(true);
    return true;
}

LRESULT IconPicker::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        PaintDC dc(m_hWnd);
        RECT rc;
        CopyRect(&rc, &dc.m_ps.rcPaint);
        paint(&dc, &rc);
        return 0;
    }
    case WM_NCPAINT:
    {
        HDC hdc = GetDCEx(m_hWnd, (HRGN)wParam, DCX_WINDOW | DCX_INTERSECTRGN | DCX_USESTYLE);
        if (hdc)
        {
            {
                GDC dc(hdc);
                drawBorder(&dc, true);
            }
            ReleaseDC(m_hWnd, hdc);
        }
        else
        {
            WindowDC dc(m_hWnd);
            drawBorder(&dc, true);
        }
        return 0;
    }
    case WM_PRINT:
    case WM_PRINTCLIENT:
    {
        GDC dc((HDC)wParam);
        print((HDC)wParam, (UINT)lParam);
        return 0;
    }
    default:
        return DefWindowProcW(m_hWnd, msg, wParam, lParam);
    }
}

BOOL IconPicker::drawBorder(GDC* dc, bool excludeClient)
{
    RECT client;
    RECT window;
    getClientRect(&client);
    getWindowRect(&window);
    if (excludeClient)
    {
        POINT origin;
        origin.x = window.left;
        origin.y = window.top;
        ScreenToClient(m_hWnd, &origin);
        client.left -= origin.x;
        client.right -= origin.x;
        client.top -= origin.y;
        client.bottom -= origin.y;
        ExcludeClipRect(dc->m_hdc, client.left, client.top, client.right, client.bottom);
    }
    window.right -= window.left;
    window.bottom -= window.top;
    window.left = 0;
    window.top = 0;
    COLORREF color;
    if (Window_isClassicAppearance())
        color = GetSysColor(COLOR_INACTIVECAPTION);
    else
        color = 0xB99D7F;
    return dc->frameRect(&window, color);
}

void IconPicker::print(HDC hdc, UINT flags)
{
    if ((flags & PRF_CHECKVISIBLE) != 0 && !IsWindowVisible(m_hWnd))
        return;
    if ((flags & PRF_NONCLIENT) != 0)
    {
        GDC dc(hdc);
        drawBorder(&dc, false);
    }
    if ((flags & (PRF_CLIENT | PRF_CHILDREN | PRF_OWNED)) != 0)
    {
        GDC dc(hdc);
        RECT rc;
        paint(&dc, getClientRect(&rc));
    }
}

BOOL IconPicker::paint(GDC* dc, RECT* rc)
{
    HDC hdc = dc->m_hdc;
    Window_isClassicAppearance();
    FillRect(hdc, rc, getSolidBrush(GetSysColor(COLOR_WINDOW)));
    SetBkColor(hdc, 0xFFFFFF);
    UINT iconStyle = 0;
    if ((m_state & 0x410) != 0)
        iconStyle = ILD_FOCUS;
    if ((m_state & 4) != 0)
        iconStyle = ILD_SELECTED;
    OSVERSIONINFOW version;
    memset(&version.dwMajorVersion, 0, 0x110);
    version.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
    GetVersionExW(&version);
    if (version.dwMajorVersion == 5)
        iconStyle = 0;
    ImageList_Draw(m_icon.m_hList, 0, hdc, 0, 0, iconStyle);

    RECT arrow;
    getClientRect(&arrow);
    arrow.left = arrow.right - 17;
    int arrowImage = 0;
    UINT frameState = DFCS_FLAT;
    if ((m_state & 0x410) != 0)
    {
        frameState = DFCS_FLAT | DFCS_HOT;
        arrowImage = 1;
    }
    if ((m_state & 0x800) != 0)
    {
        frameState |= DFCS_PUSHED;
        arrowImage = 2;
    }
    memset(&version.dwMajorVersion, 0, 0x110);
    version.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
    GetVersionExW(&version);
    if (version.dwMajorVersion == 5 || isClassicAppearance())
        return DrawFrameControl(hdc, &arrow, DFC_SCROLL, frameState | DFCS_SCROLLDOWN);
    return ImageList_Draw(m_arrows.m_hList, arrowImage, hdc, arrow.left, 0, 0);
}
