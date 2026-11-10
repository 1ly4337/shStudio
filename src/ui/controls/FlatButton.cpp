#include <windows.h>
#include <commctrl.h>
#include "FlatButton.h"
#include "GDC.h"
#include "PaintDC.h"
#include "_imports_p09.h"
#include "_imports_g6.h"

static const UINT_PTR kFlatSubclassId = 0x666C6174;

LRESULT CALLBACK FlatButton::subclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
                                          DWORD_PTR refData)
{
    FlatButton* button = (FlatButton*)refData;
    if (!button)
        return DefSubclassProc(hWnd, msg, wParam, lParam);

    if (msg == WM_PAINT)
    {
        PaintDC dc(hWnd);
        HDC hdc = dc.m_hdc;
        POINT origin;
        origin.x = 0;
        origin.y = 0;
        if (button->m_bmpHover && (button->m_state & 0x400) != 0)
            drawBitmapTransparent(hdc, &origin, button->m_bmpHover);
        else
            drawBitmapTransparent(hdc, &origin, button->m_bmpNormal);
        if ((button->m_state & 0x400) != 0)
            dc.frameRect(0, 0, button->m_width - 1, button->m_height - 1, 0xB99D7F);
        return 0;
    }

    switch (msg)
    {
    case WM_ERASEBKGND:
    {
        GDC dc((HDC)wParam);
        RECT client;
        RECT fill;
        GetClientRect(hWnd, &client);
        CopyRect(&fill, &client);
        HBRUSH brush = CreateSolidBrush(0xEDEDF1);
        UnrealizeObject(brush);
        FillRect((HDC)wParam, &fill, brush);
        DeleteObject(brush);
        return 0;
    }
    case WM_DRAWITEM:
    {
        POINT origin;
        origin.x = 0;
        origin.y = 0;
        drawBitmapTransparent(((DRAWITEMSTRUCT*)lParam)->hDC, &origin, button->m_bmpNormal);
        return 0;
    }
    case WM_MOUSEMOVE:
    {
        if (button->m_tracking)
            return DefSubclassProc(hWnd, msg, wParam, lParam);
        TRACKMOUSEEVENT track;
        track.cbSize = sizeof(TRACKMOUSEEVENT);
        track.dwFlags = TME_LEAVE;
        track.hwndTrack = hWnd;
        track.dwHoverTime = HOVER_DEFAULT;
        BOOL tracking = TrackMouseEvent(&track);
        button->m_state |= 0x400;
        button->m_tracking = tracking != 0;
        InvalidateRect(hWnd, 0, FALSE);
        SetCursor(LoadCursorW(0, IDC_HAND));
        break;
    }
    case WM_MOUSELEAVE:
        button->m_state &= ~0x400;
        button->m_tracking = false;
        InvalidateRect(hWnd, 0, TRUE);
        break;
    case WM_SETCURSOR:
        SetCursor(LoadCursorW(0, IDC_HAND));
        return 1;
    default:
        break;
    }
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

FlatButton::FlatButton()
    : m_bmpNormal(0), m_bmpHover(0), m_width(0), m_height(0), m_state(0), m_tracking(false)
{
}

FlatButton::~FlatButton()
{
    if (m_bmpNormal)
        DeleteObject(m_bmpNormal);
    if (m_bmpHover)
        DeleteObject(m_bmpHover);
}

bool FlatButton::create(HWND parent, HMENU id, int x, int y, int cx, int cy, HANDLE bitmap, int mode)
{
    if (!bitmap)
        return false;
    if (mode == 1)
    {
        m_bmpNormal = (HBITMAP)bitmap;
    }
    else if ((mode & 0x80) != 0)
    {
        m_bmpNormal = cropBitmap(bitmap, cx * (mode - 129), 0, cx, cy);
    }
    else
    {
        m_bmpNormal = cropBitmap(bitmap, 0, 0, cx, cy);
        BITMAP info;
        GetObjectW(bitmap, sizeof(BITMAP), &info);
        if (info.bmWidth >= 2 * cx)
            m_bmpHover = cropBitmap(bitmap, cx, 0, cx, cy);
        else if (info.bmHeight >= 2 * cy)
            m_bmpHover = cropBitmap(bitmap, 0, cy, cx, cy);
    }
    return createWindow(parent, id, x, y, cx, cy);
}

bool FlatButton::createWindow(HWND parent, HMENU id, int x, int y, int cx, int cy)
{
    m_width = cx;
    m_height = cy;
    m_hWnd = CreateWindowExW(0, L"Button", 0, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x, y, cx, cy, parent, id, 0, 0);
    if (m_hWnd)
        setFontId(m_fontId);
    else
        GetLastError();
    if (!m_hWnd)
        return false;
    SetWindowSubclass(m_hWnd, subclassProc, kFlatSubclassId, (DWORD_PTR)this);
    return true;
}
