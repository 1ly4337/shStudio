#include <windows.h>
#include <commctrl.h>
#include "ToggleButton.h"
#include "GDC.h"
#include "PaintDC.h"
#include "_imports_p09.h"
#include "_imports_g6.h"

static const UINT_PTR kToggleSubclassId = 0x7467676C;

LRESULT ToggleButton::setCheck(bool checked)
{
    HWND hWnd = m_hWnd;
    m_checked = checked;
    return InvalidateRect(hWnd, 0, FALSE);
}

BOOL ToggleButton::isChecked()
{
    return m_checked;
}

LRESULT CALLBACK ToggleButton::subclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
                                            DWORD_PTR refData)
{
    ToggleButton* button = (ToggleButton*)refData;
    if (!button)
        return DefSubclassProc(hWnd, msg, wParam, lParam);
    if (msg == WM_ERASEBKGND)
        return 0;
    if (msg == WM_PAINT)
    {
        PaintDC dc(hWnd);
        POINT origin;
        origin.x = 0;
        origin.y = 0;
        HBITMAP bitmap = button->m_checked ? button->m_bmpOn : button->m_bmpOff;
        drawBitmapTransparent(dc.m_hdc, &origin, bitmap);
        return 0;
    }
    if (msg != WM_DRAWITEM)
        return DefSubclassProc(hWnd, msg, wParam, lParam);
    button->draw(((DRAWITEMSTRUCT*)lParam)->hDC);
    return 0;
}

ToggleButton::~ToggleButton()
{
    if (m_bmpOn)
        DeleteObject(m_bmpOn);
    if (m_bmpOff)
        DeleteObject(m_bmpOff);
}

bool ToggleButton::create(HWND parent, HMENU id, int x, int y, int unused)
{
    HANDLE image = LoadImageW(GetModuleHandleW(0), MAKEINTRESOURCEW(0x15F), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    if (!image)
        return false;
    BITMAP info;
    GetObjectW(image, sizeof(BITMAP), &info);
    int height = info.bmHeight;
    int width = info.bmWidth / 2;
    m_bmpOff = cropBitmap(image, 0, 0, info.bmWidth / 2, info.bmHeight);
    m_bmpOn = cropBitmap(image, width, 0, width, height);
    DeleteObject(image);
    return createWindow(parent, id, x, y, width, height);
}

bool ToggleButton::createWindow(HWND parent, HMENU id, int x, int y, int cx, int cy)
{
    if (!m_bmpOff || !m_bmpOn)
        return false;
    m_hWnd = CreateWindowExW(0, L"Button", 0, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 310, y, cx, cy, parent,
                             (HMENU)0xC9, 0, 0);
    if (m_hWnd)
        setFontId(m_fontId);
    else
        GetLastError();
    if (!m_hWnd)
        return false;
    SetWindowSubclass(m_hWnd, subclassProc, kToggleSubclassId, (DWORD_PTR)this);
    return true;
}

HDC ToggleButton::draw(HDC hdc)
{
    POINT origin;
    origin.x = 0;
    origin.y = 0;
    HBITMAP bitmap = m_checked ? m_bmpOn : m_bmpOff;
    return (HDC)(INT_PTR)drawBitmapTransparent(hdc, &origin, bitmap);
}
