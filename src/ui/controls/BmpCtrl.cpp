#include <windows.h>
#include <commctrl.h>
#include <cstring>
#include "BmpCtrl.h"
#include "GDC.h"
#include "PaintDC.h"
#include "ida_compat.h"
#include "_imports_p09.h"
#include "_imports_g5.h"

BmpCtrl::BmpCtrl(COLORREF bkColor, COLORREF fillColor)
{
    m_bitmapSize.cy = 0;
    m_bitmapSize.cx = 0;
    m_bkColor = bkColor;
    m_fillColor = fillColor;
    m_ownsBitmap = true;
    m_drawFlags = 0;
    m_bitmap = 0;
    m_imageIndex = -1;
    m_childId = 0;
}

BmpCtrl::~BmpCtrl()
{
    if (m_bitmap && m_ownsBitmap)
        DeleteObject(m_bitmap);
}

BOOL BmpCtrl::setBitmap(HBITMAP bitmap)
{
    if (m_bitmap && m_ownsBitmap)
        DeleteObject(m_bitmap);
    m_bitmap = bitmap;
    m_bitmapSize.cx = 0;
    m_bitmapSize.cy = 0;
    if (bitmap)
    {
        BITMAP bm;
        memset(&bm, 0, sizeof(bm));
        if (GetObjectW(bitmap, sizeof(BITMAP), &bm) > 0)
        {
            m_bitmapSize.cx = bm.bmWidth;
            m_bitmapSize.cy = bm.bmHeight;
        }
    }
    invalidate(true);
    return UpdateWindow(m_hWnd);
}

bool BmpCtrl::onLButton(WPARAM , POINT* , bool down)
{
    bool notify;
    if (down)
        notify = (m_style.m_bits & 2) != 0;
    else
        notify = (m_style.m_bits & 4) != 0;
    if (notify)
    {
        HWND hWnd = m_hWnd;
        PostMessageW(GetParent(hWnd), WM_COMMAND, (WPARAM)m_childId, (LPARAM)hWnd);
    }
    return true;
}

LRESULT BmpCtrl::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_ERASEBKGND)
    {
        HDC hDC = (HDC)wParam;
        if ((GetWindowLongW(m_hWnd, GWL_EXSTYLE) & WS_EX_TRANSPARENT) == 0)
        {
            GDC dc(hDC);
            COLORREF color = m_bkColor;
            RECT rc;
            const RECT* client = getClientRect(&rc);
            HBRUSH brush = CreateSolidBrush(color);
            UnrealizeObject(brush);
            FillRect(hDC, client, brush);
            DeleteObject(brush);
        }
        return 1;
    }
    if (msg == WM_PAINT)
    {
        PaintDC dc(m_hWnd);
        HDC hdc = FIELD(HDC, &dc, 0x04);
        RECT rc;
        getClientRect(&rc);
        if (m_bitmap)
        {
            if ((GetWindowLongW(m_hWnd, GWL_EXSTYLE) & WS_EX_TRANSPARENT) != 0)
            {
                POINT origin = { 0, 0 };
                drawBitmapTransparent(hdc, &origin, m_bitmap);
            }
            else
            {
                drawBitmapAligned(hdc, (int*)&rc, m_bitmap, (char)m_drawFlags);
            }
            return 0;
        }
        HIMAGELIST images = m_images.m_hList;
        if (ImageList_GetImageCount(images) <= 0)
        {
            if (m_fillColor)
            {
                dc.GDC::fillRect(&rc, m_fillColor);
                dc.GDC::frameRect(&rc, 0x958C89);
                return 0;
            }
        }
        else
        {
            int count = ImageList_GetImageCount(images);
            int index = m_imageIndex;
            if (index >= 0 && index < count)
            {
                ImageList_Draw(images, index, hdc, 0, 0, 0);
                return 0;
            }
        }
        dc.GDC::fillRect(&rc, m_bkColor);
        return 0;
    }
    if (msg != WM_SETCURSOR)
        return DefWindowProcW(m_hWnd, msg, wParam, lParam);
    LPCWSTR cursor = IDC_ARROW;
    if (m_style.m_bits)
        cursor = IDC_HAND;
    SetCursor(LoadCursorW(0, cursor));
    return 1;
}
