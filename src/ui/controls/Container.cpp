#include "Container.h"
#include <cwchar>
#include "GDC.h"
#include "WindowDC.h"
#include "p10_vcall.h"
#include "gdi_cache.h"
#include "_imports_p10.h"

bool Container::createChildAt(HWND parent, HMENU id, int x, int y, int cx, int cy, DWORD style,
                              DWORD exStyle, LPCWSTR windowName)
{
    int width = cx;
    int height;
    RECT rc;
    RECT rcWindow;
    if (!cx || (height = cy) == 0)
    {
        SetRectEmpty(&rc);
        WNDPROC proc = (WNDPROC)GetClassLongW(parent, GCL_WNDPROC);
        LONG owner;
        if (proc && proc == &Window::wndProc &&
            ((owner = GetWindowLongW(parent, GWLP_USERDATA)) != 0 || (owner = GetWindowLongW(parent, 8)) != 0))
        {
            RECT tmp;
            CopyRect(&rc, reinterpret_cast<Window*>(owner)->getClientRect(&tmp));
        }
        else
        {
            GetClientRect(parent, &rc);
        }
        width = rc.right - rc.left;
        height = rc.bottom - rc.top;
    }
    SetRect(&rcWindow, x, y, x + width, y + height);
    return createChild(parent, id, &rcWindow, style, exStyle, windowName);
}

bool Container::createChild(HWND parent, HMENU id, const RECT* rect, DWORD style, DWORD exStyle,
                            LPCWSTR windowName)
{
    if (!parent)
        return false;
    DWORD st = style | (WS_CHILD | WS_CLIPCHILDREN);
    if ((int)style < 0)
        st = (style & 0x3DFFFFFF) | (WS_CHILD | WS_CLIPCHILDREN);
    if (exStyle & WS_EX_TRANSPARENT)
        m_transparent = true;
    if (exStyle & WS_EX_STATICEDGE)
        st &= ~WS_BORDER;
    return Window::create(parent, id, rect, st, exStyle | WS_EX_CONTROLPARENT, windowName);
}

HWND Container::destroy()
{
    HWND result = m_hWnd;
    m_focus = 0;
    if (result)
    {
        SendMessageW(result, WM_SETFONT, 0, 0);
        result = (HWND)(INT_PTR)DestroyWindow(m_hWnd);
        m_hWnd = 0;
    }
    return result;
}

void Container::onActivate(bool active)
{
    if (active)
    {
        if (m_focus)
            SetFocus(m_focus);
    }
    else
    {
        m_focus = GetFocus();
    }
}

LRESULT Container::onCtlColor(UINT msg, HDC hdc, HWND ctrl)
{
    COLORREF color = 0;
    for (int i = 0; i < m_highlighted.m_count; ++i)
    {
        if (m_highlighted.m_items[i] == ctrl)
        {
            color = m_highlightColor;
            break;
        }
    }
    int count = m_textColors.m_count;
    for (int i = 0; i < count; ++i)
    {
        if (m_textColors.m_table[i].key == ctrl)
        {
            int index = 0;
            while (m_textColors.m_table[index].key != ctrl)
            {
                if (++index >= count)
                {
                    index = -1;
                    break;
                }
            }
            color = (index == -1) ? 0 : m_textColors.m_table[index].value;
            break;
        }
    }
    SetTextColor(hdc, color);
    return Window::onCtlColor(msg, hdc, ctrl);
}

bool Container::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code)
        return false;
    if (ctrl)
    {
        SendMessageW(GetParent(m_hWnd), WM_COMMAND, id, (LPARAM)ctrl);
        return true;
    }
    HWND top = GetParent(m_hWnd);
    while (GetWindowLongW(top, GWL_STYLE) & WS_CHILD)
        top = GetParent(top);
    SendMessageW(top, WM_COMMAND, id, 0);
    return true;
}

bool Container::onKey(WPARAM vk, LPARAM flags, bool down)
{
    short key = (short)vk;
    if (key != VK_TAB)
    {
        if (key == VK_RETURN)
        {
            if (down)
            {
                PostMessageW(m_hWnd, WM_COMMAND, IDOK, (LPARAM)m_hWnd);
                return true;
            }
        }
        else if (key == VK_ESCAPE && down)
        {
            PostMessageW(m_hWnd, WM_COMMAND, IDCANCEL, (LPARAM)m_hWnd);
            return true;
        }
        return false;
    }
    if (!down)
        return false;
    HWND next = GetNextDlgTabItem(m_hWnd, m_focus, ((unsigned short)GetKeyState(VK_SHIFT) >> 15) & 1);
    if (next)
    {
        m_focus = next;
        SetFocus(next);
    }
    return true;
}

bool Container::drawNcBorder(GDC* dc)
{
    if ((GetWindowLongW(m_hWnd, GWL_STYLE) & (WS_CHILD | WS_BORDER)) != (WS_CHILD | WS_BORDER))
        return false;
    SIZE size;
    SIZE* s = getSize(&size);
    int cx = s->cx;
    int cy = s->cy;
    Window_isClassicAppearance();
    HGDIOBJ old = SelectObject(dc->m_hdc, CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW)));
    if (dc->m_savedPen)
        DeleteObject(old);
    else
        dc->m_savedPen = old;
    int right = cx - 1;
    int bottom = cy - 1;
    MoveToEx(dc->m_hdc, 0, 0, 0);
    LineTo(dc->m_hdc, right, 0);
    LineTo(dc->m_hdc, right, bottom);
    LineTo(dc->m_hdc, 0, bottom);
    LineTo(dc->m_hdc, 0, 0);
    return true;
}

LRESULT Container::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg > WM_NCPAINT)
    {
        if (msg > WM_VSCROLL)
        {
            if (msg != 0x430)
                return DefWindowProcW(m_hWnd, msg, wParam, lParam);
            SetForegroundWindow((HWND)wParam);
            SetActiveWindow((HWND)wParam);
            SetFocus((HWND)wParam);
            return 0;
        }
        if (msg < WM_HSCROLL)
        {
            if (msg == WM_GETDLGCODE)
                return DefWindowProcW(m_hWnd, WM_GETDLGCODE, wParam, lParam) | DLGC_WANTTAB;
            return DefWindowProcW(m_hWnd, msg, wParam, lParam);
        }
        if (!onScrollBar(msg - WM_HSCROLL, LOWORD(wParam), HIWORD(wParam)))
            return DefWindowProcW(m_hWnd, msg, wParam, lParam);
        return 0;
    }
    if (msg == WM_NCPAINT)
    {
        bool handled;
        {
            WindowDC dc(m_hWnd);
            handled = drawNcBorder(&dc);
        }
        if (!handled)
            return DefWindowProcW(m_hWnd, WM_NCPAINT, wParam, lParam);
        return 0;
    }
    switch (msg)
    {
    case WM_ENABLE:
        onEnable(wParam == 1);
        return 0;
    case WM_ERASEBKGND:
        if (!m_transparent)
        {
            GDC dc((HDC)wParam);
            RECT rc;
            COLORREF color = m_bkColor;
            dc.fillRect(getClientRect(&rc), color);
        }
        return 1;
    case WM_DRAWITEM:
        if (!wParam || !lParam)
            return 0;
        if (!onDrawItem((UINT)wParam, (DRAWITEMSTRUCT*)lParam))
            return DefWindowProcW(m_hWnd, msg, wParam, lParam);
        return 1;
    case WM_MEASUREITEM:
    {
        LRESULT result = 0;
        if (wParam)
        {
            MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
            result = mis->itemID != (UINT)-1;
            mis->itemHeight = 4 * (UINT)result + 12;
            return 1;
        }
        return result;
    }
    default:
        return DefWindowProcW(m_hWnd, msg, wParam, lParam);
    }
}

bool Container::onDrawItem(UINT id, DRAWITEMSTRUCT* dis)
{
    if (id == (UINT)-1)
        return false;
    bool selected = (dis->itemState & ODS_SELECTED) != 0;
    HDC hdc = dis->hDC;
    GDC dc(hdc);
    wchar_t text[256 + 6];
    SetBkColor(hdc, getStdColor(8 * (selected ? 1 : 0) + COLOR_WINDOW));
    COLORREF textColor;
    if (selected)
    {
        Window_isClassicAppearance();
        textColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
    }
    else
    {
        Window_isClassicAppearance();
        textColor = GetSysColor(COLOR_WINDOWTEXT);
        int count = m_textColors.m_count;
        int index = 0;
        if (count > 0)
        {
            HTable<HWND, unsigned long>::Entry* e = m_textColors.m_table;
            while (e->key != dis->hwndItem)
            {
                ++index;
                e += 1;
                if (index >= count)
                    goto colorDone;
            }
            if (index != -1)
                textColor = m_textColors.m_table[index].value;
        }
    }
colorDone:
    SetTextColor(hdc, textColor);
    TEXTMETRICW tm;
    memset(&tm, 0, sizeof(tm));
    GetTextMetricsW(dis->hDC, &tm);
    int y = (dis->rcItem.top + dis->rcItem.bottom - tm.tmHeight) / 2;
    int x = (unsigned short)GetDialogBaseUnits() >> 1;
    bool drawn;
    if (dis->CtlType != ODT_COMBOBOX ||
        (memset(text, 0, 0x200), SendMessageW(dis->hwndItem, CB_GETLBTEXT, dis->itemID, (LPARAM)text) == CB_ERR))
    {
        drawn = false;
    }
    else
    {
        ExtTextOutW(dis->hDC, x, y, ETO_CLIPPED | ETO_OPAQUE, &dis->rcItem, text, (UINT)wcslen(text), 0);
        if (dis->itemState & ODS_FOCUS)
            DrawFocusRect(dis->hDC, &dis->rcItem);
        drawn = true;
    }
    return drawn;
}

bool Container_toggleCheck(HWND button)
{
    bool checked = SendMessageW(button, BM_GETCHECK, 0, 0) != BST_CHECKED;
    SendMessageW(button, BM_SETCHECK, checked, 0);
    return checked;
}
