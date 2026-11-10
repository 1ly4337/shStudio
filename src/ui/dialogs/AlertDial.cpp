#include "AlertDial.h"
#include "String.hpp"
#include "Window.h"
#include "CtrlHelpers.h"
#include <commctrl.h>
#include "_imports_p10.h"

static unsigned short AlertDial_iconResource(int iconFlags)
{
    switch (iconFlags)
    {
    case MB_ICONHAND:        return 16129;
    case MB_ICONQUESTION:    return 16130;
    case MB_ICONASTERISK:    return 16132;
    default:                 return 16131;
    }
}

AlertDial::AlertDial()
{
    SetRectEmpty(&m_textRect);
    m_bkColor = GetSysColor(COLOR_WINDOW);
    m_btnMarginBottom = 34;
    m_text = NULL;
    m_details = NULL;
    m_gradient = NULL;
    m_icon = NULL;
}

bool AlertDial::createSimpleAlert(HWND parent, int flags, LPCWSTR text, LPCWSTR title)
{
    return createAlert(parent, flags, text, NULL, title);
}

bool AlertDial::createAlert(HWND parent, int flags, LPCWSTR text, LPCWSTR details, LPCWSTR title)
{
    HWND owner = parent;
    m_details = details;
    m_text = text;
    int icon = flags & 0xF0;
    if ((flags & 0xF0) == 0)
        icon = MB_ICONEXCLAMATION;
    m_icon = (HICON)LoadImageW(hModule, MAKEINTRESOURCEW(AlertDial_iconResource(icon)), IMAGE_ICON, 32, 32, 0);
    int buttons = flags & 0xF;
    if ((unsigned)(flags & 0xF) > MB_ABORTRETRYIGNORE)
        m_gradient = createGradientBitmap(40, 0xFFFFFF, 0xE0E0E0);

    HDC display = CreateDCW(L"DISPLAY", NULL, NULL, NULL);
    HDC hdc = CreateCompatibleDC(display);
    SetRect(&m_textRect, 0, 0, 320, 0);
    HGDIOBJ oldFont = SelectObject(hdc, getStdFont(0));
    if (text)
        DrawTextW(hdc, text, -1, &m_textRect, DT_CALCRECT | DT_WORDBREAK);
    else
        SetRectEmpty(&m_textRect);
    if (m_textRect.bottom < 32)
        m_textRect.bottom = 32;
    if (m_textRect.right < 245)
        m_textRect.right = 245;
    int cx = m_textRect.right + 80;
    int caption = GetSystemMetrics(SM_CYCAPTION);
    int textBottom = m_textRect.bottom;
    m_textRect.left += 60;
    m_textRect.right += 60;
    m_textRect.top += 24;
    m_textRect.bottom += 24;
    int cy = textBottom + 82 + caption;
    if (details)
        cy += 152;
    if (parent)
    {
        do
        {
            if ((GetWindowLongW(owner, GWL_STYLE) & WS_CHILD) == 0)
                break;
            owner = GetParent(owner);
        } while (owner);
    }
    LPCWSTR caption2 = g_appName.m_buffer;
    if (title)
        caption2 = title;
    bool ok = Modal::createDialog(owner, cx, cy, buttons, caption2);
    if (ok && details)
    {
        int width = m_textRect.right - m_textRect.left + 35;
        int top = m_textRect.bottom - m_textRect.top + 36;
        HWND edit = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", details, 0x50211044, 20, top, width, 140,
                                    m_hWnd, (HMENU)0x84, NULL, NULL);
        m_detailsEdit.m_hWnd = edit;
        if (edit)
        {
            m_detailsEdit.setFontId(m_detailsEdit.m_fontId);
            SetWindowSubclass(m_detailsEdit.m_hWnd, tabSubclassProc, 0x65646974 , (DWORD_PTR)&m_detailsEdit);
        }
    }
    if (oldFont)
        SelectObject(hdc, oldFont);
    DeleteDC(hdc);
    DeleteDC(display);
    return ok;
}

int AlertDial::mapCommandToResult(int id)
{
    if (id == IDOK)
    {
        switch (m_buttons)
        {
        case MB_OK:               return IDOK;
        case MB_ABORTRETRYIGNORE:
        case MB_RETRYCANCEL:      return IDRETRY;
        case MB_YESNOCANCEL:
        case MB_YESNO:            return IDYES;
        case MB_CANCELTRYCONTINUE:return IDTRYAGAIN;
        default:                  return -1;
        }
    }
    if (id == IDCANCEL)
    {
        switch (m_buttons)
        {
        case MB_OKCANCEL:
        case MB_YESNOCANCEL:
        case MB_RETRYCANCEL:
        case MB_CANCELTRYCONTINUE:return IDCANCEL;
        case MB_ABORTRETRYIGNORE: return IDABORT;
        default:                  return -1;
        }
    }
    return id;
}

LRESULT AlertDial::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg != WM_PAINT)
        return Container::defWindowProc(msg, wParam, lParam);

    HWND hWnd = m_hWnd;
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    RECT rc;
    getClientRect(&rc);
    DrawIcon(hdc, 20, 20, m_icon);
    if (m_gradient)
    {
        HDC mem = CreateCompatibleDC(hdc);
        HGDIOBJ old = SelectObject(mem, m_gradient);
        StretchBlt(hdc, 0, rc.bottom - 40, rc.right, 40, mem, 0, 0, 8, 40, SRCCOPY);
        SelectObject(mem, old);
        DeleteDC(mem);
    }
    HGDIOBJ oldFont = SelectObject(hdc, getStdFont(0));
    SetTextColor(hdc, 0);
    SetBkColor(hdc, 0xFFFFFF);
    if (m_text)
        DrawTextW(hdc, m_text, -1, &m_textRect, DT_WORDBREAK);
    if (oldFont)
        SelectObject(hdc, oldFont);
    EndPaint(hWnd, &ps);
    return 0;
}

AlertDial::~AlertDial()
{
    if (m_gradient)
        DeleteObject(m_gradient);
    if (m_icon)
        DestroyIcon(m_icon);
}

int AlertDial::alert(HWND parent, LPCWSTR text, LPCWSTR title, int flags)
{
    AlertDial dlg;
    int result;
    if (dlg.createAlert(parent, flags, text, NULL, title))
        result = dlg.doModal();
    else
        result = -1;
    return result;
}

int AlertDial::productListError(HWND parent, LPCWSTR details)
{
    AlertDial dlg;
    int result;
    if (dlg.AlertDial::createAlert(parent, MB_ICONHAND,
                                   L"An error has occured while updating PS_GameDefs.dbo.ProductList.", details, NULL))
        result = dlg.Modal::doModal();
    else
        result = -1;
    return result;
}

bool AlertDial::confirm(HWND parent, LPCWSTR text)
{
    return alert(parent, text, NULL, MB_ICONQUESTION | MB_YESNO) == IDYES;
}
