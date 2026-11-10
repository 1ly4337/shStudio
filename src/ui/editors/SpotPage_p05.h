#pragma once
#include "p05_common.h"
#include "MapSpots.h"
#include "EditText.h"
#include "Button.h"
#include "CtrlHelpers.h"

#define SPOT_WND(s) FIELD(HWND, (s), 0x0C)
#define SPOT_X(s)   FIELD(float, (s), 0x14)
#define SPOT_Y(s)   FIELD(float, (s), 0x18)
#define SPOT_Z(s)   FIELD(float, (s), 0x1C)

inline RECT* spotPointRect(RECT* rc, MapSpots* spots, float x, float z)
{
    float scale = spots->m_scale;
    int height = spots->m_height;
    rc->left = (int)(x * scale);
    rc->top = height - (int)(z * scale);
    rc->right = rc->left + 10;
    rc->bottom = rc->top + 10;
    return rc;
}

inline HWND createLabel(HWND parent, const wchar_t* text, int x, int y, int cx, int cy)
{
    HWND h = CreateWindowExW(0, L"Static", text, WS_CHILD | WS_VISIBLE, x, y, cx, cy, parent, 0, 0, 0);
    if (h)
        SendMessageW(h, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    return h;
}

inline void createFloatEdit(EditText& edit, HWND parent, int x, int y, int cx, int cy, UINT id)
{
    HWND h = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, x, y, cx, cy, parent,
                             (HMENU)(UINT_PTR)id, 0, 0);
    edit.m_hWnd = h;
    if (h) {
        edit.setFontId(edit.m_fontId);
        SetWindowSubclass(edit.m_hWnd, decimalEditSubclassProc, 0x65646974 , (DWORD_PTR)&edit);
        edit.setFloat(0.0f);
    }
}

inline void createButton(Button& button, HWND parent, const wchar_t* text, DWORD style, int x, int y, int cx,
                         int cy, UINT id)
{
    HWND h = CreateWindowExW(0, L"Button", text, style, x, y, cx, cy, parent, (HMENU)(UINT_PTR)id, 0, 0);
    button.m_hWnd = h;
    if (h)
        button.setFontId(button.m_fontId);
    else
        GetLastError();
}

inline void destroySpotWindow(MapSpots* spots, Spot* spot)
{
    HWND wnd = spots->findSpotWnd(spot);
    if (wnd) {
        DestroyWindow(wnd);
        spots->m_spotWnds.remove(wnd);
    }
}

inline void addPointSpotWindow(MapSpots* spots, Spot* spot, HMENU id)
{
    RECT rc;
    spotPointRect(&rc, spots, SPOT_X(spot), SPOT_Z(spot));
    HWND wnd = MapSpots::createSpotWindow(spots->m_hWnd, id, &rc, spot);
    spots->m_spotWnds.add(wnd);
}
