#include "SearchPanel.h"
#include "TextSearch.h"
#include "String.hpp"
#include "gdi_cache.h"
#include "_imports_p10.h"
#include <commctrl.h>
#include <cctype>

#define SP_BUTTON(p) FIELD(HWND, (p), 0x34)
#define SP_EDIT(p)   FIELD(HWND, (p), 0x38)
#define SP_STATUS(p) FIELD(HWND, (p), 0x3C)

LRESULT SearchPanel::setStatus(const wchar_t* text)
{
    const wchar_t* s = L"";
    if (text)
        s = text;
    return SendMessageW(SP_STATUS(this), WM_SETTEXT, 0, (LPARAM)s);
}

const wchar_t* findNoCase(const wchar_t* text, const wchar_t* pattern)
{
    if (!text || !pattern)
        return text;
    wchar_t c = *text;
    if (!c)
        return 0;
    for (;;)
    {
        if (toupper(c) == toupper(*pattern))
        {
            const wchar_t* p = pattern;
            while (*p)
            {
                if (toupper(text[p - pattern]) != toupper(*p))
                    break;
                ++p;
                if (!text[p - pattern])
                    break;
            }
            if (!*p)
                return text;
        }
        c = text[1];
        ++text;
        if (!c)
            return 0;
    }
}

String SearchPanel::getText()
{
    String result;
    LRESULT length = SendMessageW(SP_EDIT(this), WM_GETTEXTLENGTH, 0, 0);
    if (length > 0)
    {
        WPARAM size = length + 16;
        result.reserve((unsigned)size);
        unsigned n = (unsigned)SendMessageW(SP_EDIT(this), WM_GETTEXT, size, (LPARAM)result.m_buffer);
        if (n < result.m_capacity)
        {
            result.m_length = n;
            result.m_buffer[n] = 0;
        }
    }
    return result;
}

BOOL SearchPanel::show(bool visible)
{
    if (visible != (isVisible() != 0))
        ShowWindow(m_hWnd, visible ? SW_SHOW : SW_HIDE);
    if (visible)
    {
        SetForegroundWindow(SP_EDIT(this));
        SetActiveWindow(SP_EDIT(this));
        SetFocus(SP_EDIT(this));
        return (BOOL)SendMessageW(SP_EDIT(this), EM_SETSEL, 0, -1);
    }
    RECT rect;
    getRectInParent(&rect);
    return InvalidateRect(GetParent(m_hWnd), &rect, TRUE);
}

LRESULT CALLBACK SearchPanel::editSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                               UINT_PTR idSubclass, DWORD_PTR refData)
{
    if (msg == WM_KEYDOWN)
    {
        UINT post;
        if (wParam == VK_RETURN)
        {
            post = WM_USER + 1;
            goto send;
        }
        if (wParam == VK_ESCAPE)
        {
            post = WM_USER + 2;
        send:
            PostMessageW((HWND)refData, post, (WPARAM)GetWindowLongW(hWnd, GWL_ID), (LPARAM)hWnd);
            return 0;
        }
    }
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

bool SearchPanel::onCreate()
{
    SP_BUTTON(this) = CreateWindowExW(0, L"Button", L"X", WS_CHILD | WS_VISIBLE, 10, 3, 20, 20, m_hWnd, (HMENU)101, 0, 0);
    SP_EDIT(this) = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080u, 40, 4, 250, 18, m_hWnd, (HMENU)102, 0, 0);
    SP_STATUS(this) = CreateWindowExW(0, L"Static", L"", WS_CHILD | WS_VISIBLE, 300, 6, 150, 16, m_hWnd, (HMENU)103, 0, 0);
    WPARAM font = (WPARAM)getStdFont(0);
    SendMessageW(SP_BUTTON(this), WM_SETFONT, font, 1);
    SendMessageW(SP_STATUS(this), WM_SETFONT, font, 1);
    SendMessageW(SP_EDIT(this), WM_SETFONT, (WPARAM)getStdFont(6), 1);
    SetWindowSubclass(SP_EDIT(this), editSubclassProc, 0x65646974u , (DWORD_PTR)m_hWnd);
    return true;
}

bool SearchPanel::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code || ctrl != SP_BUTTON(this) || id != 101)
        return false;
    WPARAM wId = (WPARAM)GetWindowLongW(m_hWnd, GWL_ID);
    PostMessageW(GetParent(m_hWnd), WM_USER + 2, wId, (LPARAM)this);
    return true;
}

LRESULT SearchPanel::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg != WM_USER + 1 && msg != WM_USER + 2)
        return DefWindowProcW(m_hWnd, msg, wParam, lParam);
    WPARAM wId = (WPARAM)GetWindowLongW(m_hWnd, GWL_ID);
    PostMessageW(GetParent(m_hWnd), msg, wId, (LPARAM)this);
    return 0;
}
