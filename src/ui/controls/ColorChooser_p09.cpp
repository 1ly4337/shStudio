#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <cstring>
#include "ColorChooser.h"
#include "GDC.h"
#include "PaintDC.h"
#include "_imports_p09.h"
#include "_imports_g6.h"

COLORREF g_customColors[16];

static const UINT_PTR kColorSubclassId = 0x636C7263;

LRESULT CALLBACK ColorChooser::subclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
                                            DWORD_PTR refData)
{
    ColorChooser* chooser = (ColorChooser*)refData;
    if (!chooser)
        return DefSubclassProc(hWnd, msg, wParam, lParam);

    if (msg == WM_PAINT)
    {
        PaintDC dc(hWnd);
        HDC hdc = dc.m_hdc;
        HGDIOBJ& savedPen = dc.m_savedPen;
        COLORREF color = chooser->m_color & 0xFFFFFF;
        if (!IsWindowEnabled(hWnd))
            color = 0xC0C0C0;
        RECT rc;
        SetRectEmpty(&rc);
        GetClientRect(hWnd, &rc);
        HPEN pen = CreatePen(PS_SOLID, 1, 0xB99D7F);
        HGDIOBJ previous = SelectObject(hdc, pen);
        if (savedPen)
            DeleteObject(previous);
        else
            savedPen = previous;
        dc.frameRect(&rc);
        ++rc.top;
        --rc.bottom;
        ++rc.left;
        --rc.right;
        HBRUSH brush = CreateSolidBrush(color);
        UnrealizeObject(brush);
        FillRect(hdc, &rc, brush);
        DeleteObject(brush);
        return 0;
    }
    if (msg == WM_SETCURSOR)
    {
        SetCursor(LoadCursorW(0, IDC_HAND));
        return 1;
    }
    if (msg != WM_LBUTTONDOWN)
        return DefSubclassProc(hWnd, msg, wParam, lParam);

    CHOOSECOLORW cc;
    memset(&cc, 0, sizeof(cc));
    for (int i = 0; i < 16; ++i)
        g_customColors[i] = 0xFFFFFF;
    cc.lStructSize = sizeof(CHOOSECOLORW);
    cc.hwndOwner = GetParent(hWnd);
    cc.rgbResult = chooser->m_color;
    cc.lpCustColors = g_customColors;
    cc.Flags = CC_RGBINIT | CC_FULLOPEN | CC_SOLIDCOLOR;
    if (ChooseColorW(&cc))
        chooser->m_color = cc.rgbResult | 0xFF000000;
    return 1;
}

bool ColorChooser::create(HWND parent, HMENU id, int x, int y, int cx, int cy, COLORREF color, int unused1,
                          int unused2)
{
    m_color = color;
    m_hWnd = CreateWindowExW(0, L"Button", 0, WS_CHILD | WS_VISIBLE, 80, 170, 50, 18, parent, (HMENU)0x636C6F72, 0, 0);
    if (!m_hWnd)
    {
        GetLastError();
        return false;
    }
    setFontId(m_fontId);
    SetWindowSubclass(m_hWnd, subclassProc, kColorSubclassId, (DWORD_PTR)this);
    return true;
}
