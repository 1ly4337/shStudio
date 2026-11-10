#include "MapSpots.h"
#include "SpotDetails.h"
#include "TabCtrl.h"
#include "GDC.h"
#include "PaintDC.h"
#include "ScreenDC.h"
#include "Window.h"
#include "Exception.h"
#include "Spot.h"
#include "TacticsZone.h"
#include "Application.h"
#include "_imports_p06.h"

MapSpots::MapSpots(Document* document, void* arg3, TabCtrl<SpotDetails*>* tabs)
    : Container(),
      Scaler(document, tabs),
      m_74(arg3),
      m_picture(Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0, 0xFFFFFF),
      m_spotWnds()
{
    m_bkColor = Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0;
    m_height = 512;
    m_scale = 1.0f;
}

MapSpots::~MapSpots()
{
}

bool MapSpots::onCreate()
{
    SIZE size;
    getSize(&size);
    HWND parent = m_hWnd;
    m_picture.m_bitmapSize = size;
    RECT rc;
    SetRect(&rc, 0, 0, size.cx, size.cy);
    return m_picture.create(parent, (HMENU)0x6E, &rc, 0x50000000, 0, 0);
}

LRESULT MapSpots::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg != 0x440)
        return Container::defWindowProc(msg, wParam, lParam);
    SpotDetails* page = m_tabs->pageAt(-1);
    page->selectSpot((Spot*)lParam);
    return 1;
}

HBITMAP MapSpots::renderBitmap()
{
    int count = m_spotWnds.m_count;
    if (count < 1)
        return 0;
    HBITMAP bitmap = createSolidBitmap(m_height, m_height, 0xFFFFFF);
    if (bitmap)
    {
        ScreenDC dc;
        dc.m_prevBitmap = SelectObject(dc.m_hdc, bitmap);
        RECT rc;
        SetRectEmpty(&rc);
        RECT frame;
        getClientRect(&frame);
        dc.frameRect(&frame, 0);
        GetWindowRect(m_hWnd, &frame);
        for (int i = 0; i < count; ++i)
        {
            HWND wnd = (i < 0 || i >= m_spotWnds.m_count) ? 0 : m_spotWnds.m_items[i];
            GetWindowRect(wnd, &rc);
            rc.left -= frame.left;
            rc.right -= frame.left;
            rc.top -= frame.top;
            rc.bottom -= frame.top;
            dc.frameRect(&rc, 0xFF);
        }
    }
    return bitmap;
}

POINT* MapSpots::toPoint(POINT* out, float x, float z)
{
    float px = m_scale * x;
    int pz = (int)(float)(m_scale * z);
    out->x = (int)px;
    out->y = m_height - pz;
    return out;
}

RECT* MapSpots::getPointRect(RECT* out, float x, float z)
{
    int height = m_height;
    float px = m_scale * x;
    float pz = m_scale * z;
    out->left = (int)px;
    int top = height - (int)pz;
    out->right = (int)px + 10;
    out->top = top;
    out->bottom = top + 10;
    return out;
}

HWND MapSpots::findSpotWnd(Spot* spot)
{
    int count = m_spotWnds.m_count;
    if (count <= 0)
        return 0;
    for (int i = 0; ; )
    {
        HWND wnd = (i < 0 || i >= m_spotWnds.m_count) ? 0 : m_spotWnds.m_items[i];
        if (GetWindowLongW(wnd, GWL_USERDATA) == (LONG)spot)
            return wnd;
        if (++i >= count)
            return 0;
    }
}

void MapSpots::destroySpots()
{
    int count = m_spotWnds.m_count;
    for (int i = 0; i < count; ++i)
    {
        if (!DestroyWindow(*m_spotWnds.at(i)))
            GetLastError();
    }
    m_spotWnds.m_count = 0;
}

bool MapSpots::removeSpot(Spot* spot)
{
    HWND wnd = findSpotWnd(spot);
    if (wnd)
    {
        DestroyWindow(wnd);
        return m_spotWnds.remove(wnd);
    }
    return false;
}

BOOL MapSpots::redrawSpot(HWND hWnd, int unused)
{
    InvalidateRect(hWnd, 0, TRUE);
    HWND self = m_hWnd;
    RECT rc;
    RECT own;
    SetRectEmpty(&rc);
    SetRectEmpty(&own);
    GetWindowRect(hWnd, &rc);
    GetWindowRect(self, &own);
    rc.top = rc.top - 1 - own.top;
    rc.left = rc.left - own.left - 1;
    rc.right = rc.right - own.left + 1;
    rc.bottom = rc.bottom + 1 - own.top;
    return InvalidateRect(m_hWnd, &rc, TRUE);
}

LRESULT CALLBACK MapSpots::spotWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        Spot* spot = (Spot*)((CREATESTRUCTW*)lParam)->lpCreateParams;
        if (!spot)
            return -1;
        spot->m_hItem = hWnd;
        SetWindowLongW(hWnd, GWL_USERDATA, (LONG)spot);
        return 0;
    }
    case WM_DESTROY:
    {
        Spot* spot = (Spot*)GetWindowLongW(hWnd, GWL_USERDATA);
        if (spot)
            spot->m_hItem = 0;
        return 0;
    }
    case WM_PAINT:
    {
        RECT rc;
        SetRectEmpty(&rc);
        GetClientRect(hWnd, &rc);
        Spot* spot = (Spot*)GetWindowLongW(hWnd, GWL_USERDATA);
        TacticsZone* zone = dynamic_cast<TacticsZone*>(spot);
        PaintDC dc(hWnd);
        if (zone)
        {
            zone->draw(&dc);
        }
        else
        {
            int state = GetWindowLongW(hWnd, GWL_STYLE) & 0xF;
            COLORREF color;
            if (state == 2)
            {
                color = 0xFFFF80;
            }
            else
            {
                color = 0xFF;
                if (state == 1)
                {
                    dc.fillRect(&rc, 0xFF);
                    return 0;
                }
            }
            dc.frameRect(&rc, color);
        }
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_NCPAINT:
        return 0;
    }
    if (msg != WM_LBUTTONDOWN)
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    LPARAM spot = GetWindowLongW(hWnd, GWL_USERDATA);
    WPARAM id = GetWindowLongW(hWnd, GWL_ID);
    PostMessageW(GetParent(hWnd), 0x440, id, spot);
    return 0;
}

HWND MapSpots::createSpotWindow(HWND parent, HMENU id, const RECT* rc, void* spot)
{
    WNDCLASSEXW wcx;
    memset(&wcx, 0, sizeof(wcx));
    wcx.cbSize = sizeof(WNDCLASSEXW);
    if (!GetClassInfoExW(hModule, L"MapItem", &wcx))
    {
        wcx.lpszClassName = L"MapItem";
        wcx.lpfnWndProc = spotWndProc;
        wcx.hInstance = hModule;
        wcx.hCursor = LoadCursorW(0, IDC_HAND);
        wcx.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
        if (!RegisterClassExW(&wcx))
            return 0;
    }
    int x = rc->left;
    int y = rc->top;
    int bottom = rc->bottom;
    int width = rc->right - rc->left;
    int minSize = 5;
    if (width < 5)
        width = minSize;
    int height = bottom - y;
    if (height < 5)
        height = minSize;
    HWND wnd = CreateWindowExW(WS_EX_TRANSPARENT, L"MapItem", 0, 0x50000000, x, y, width, height, parent, id,
                               hModule, spot);
    if (wnd)
        SetWindowPos(wnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    return wnd;
}
