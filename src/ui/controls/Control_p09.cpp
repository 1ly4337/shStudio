#include <windows.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <cwchar>
#include <cstring>
#include "Control.hpp"
#include "String.hpp"
#include "_imports_p09.h"
#include "_imports_g6.h"

wchar_t g_dropFileName[512];

static const UINT_PTR kControlSubclassId = 0x6374726C;

Control::~Control()
{
}

bool Control::create(HWND parent, HMENU id, int x, int y, int cx, int cy, DWORD style, DWORD exStyle,
                     int unused)
{
    DWORD ctrlStyle = getCtrlStyle();
    DWORD fullStyle = getStyle() | ctrlStyle | WS_CHILD | style;
    DWORD fullExStyle = getWindowExStyle() | exStyle;
    m_acceptFiles = m_acceptFiles || (fullExStyle & WS_EX_ACCEPTFILES) != 0;
    DWORD createStyle = fullStyle;
    if ((fullStyle & WS_BORDER) != 0)
    {
        fullExStyle |= WS_EX_CLIENTEDGE;
        createStyle = fullStyle & ~WS_BORDER;
    }
    m_id = (int)(INT_PTR)id;
    HWND hWnd = CreateWindowExW(fullExStyle, getClassName(), 0, createStyle, x, y, cx, cy, parent, id, 0, 0);
    m_hWnd = hWnd;
    if (!hWnd)
    {
        GetLastError();
        return false;
    }
    bool ok = SetWindowSubclass(hWnd, subclassProc, kControlSubclassId, (DWORD_PTR)this) != 0;
    if (!ok || !(ok = onCreate()))
    {
        DestroyWindow(m_hWnd);
        m_hWnd = 0;
    }
    return ok;
}

bool Control::onCreate()
{
    setFont(getStdFont(0));
    return true;
}

bool Control::onKey(short vk, int flags, bool down)
{
    if (GetKeyState(VK_CONTROL) >= 0 || vk != 'W')
        return false;
    HWND top;
    for (top = GetParent(m_hWnd); (GetWindowLongW(top, GWL_STYLE) & WS_CHILD) != 0; top = GetParent(top))
        ;
    SendMessageW(top, WM_COMMAND, 0xE102, 0);
    return true;
}

void Control::onDropFiles(HDROP drop, POINT* pt)
{
    int count = (int)DragQueryFileW(drop, 0xFFFFFFFF, 0, 0);
    for (int i = 0; i < count; ++i)
    {
        memset(g_dropFileName, 0, sizeof(g_dropFileName));
        if (!DragQueryFileW(drop, i, g_dropFileName, 512))
            continue;
        const wchar_t* ext = PathFindExtensionW(g_dropFileName);
        if (ext && wcscmp(ext, L".lnk") == 0)
            resolveShortcut(g_dropFileName, g_dropFileName);
        if (!onDropFile(i, pt, g_dropFileName))
            break;
    }
    DragFinish(drop);
}

Control* Control::fromHandle(HWND hWnd)
{
    if (!hWnd)
        return 0;
    DWORD_PTR refData = 0;
    if (!GetWindowSubclass(hWnd, subclassProc, kControlSubclassId, &refData))
        return 0;
    return (Control*)refData;
}

bool Control::setText(const wchar_t* text)
{
    if (!m_hWnd)
        return false;
    const wchar_t* value = L"";
    if (text)
        value = text;
    return SendMessageW(m_hWnd, WM_SETTEXT, 0, (LPARAM)value) == 1;
}

bool Control::getText(String& out)
{
    out.clear();
    if (!m_hWnd)
        return false;
    LRESULT length = SendMessageW(m_hWnd, WM_GETTEXTLENGTH, 0, 0);
    if (length < 0)
        return false;
    if (length)
    {
        WPARAM size = (WPARAM)(length + 1);
        wchar_t* buffer = new wchar_t[size];
        memset(buffer, 0, 2 * length + 2);
        LRESULT copied = SendMessageW(m_hWnd, WM_GETTEXT, size, (LPARAM)buffer);
        out.assign(buffer, (unsigned)copied);
        delete[] buffer;
    }
    return true;
}

int Control::modifyStyle(DWORD add, DWORD remove)
{
    if (!m_hWnd)
        return 0;
    LONG style = GetWindowLongW(m_hWnd, GWL_STYLE);
    int result = SetWindowLongW(m_hWnd, GWL_STYLE, add | (~remove & style));
    if (((remove | add) & 0xFFFF0000) != 0)
        return SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
    return result;
}

int Control::modifyStyleEx(DWORD add, DWORD remove)
{
    if (!m_hWnd)
        return 0;
    LONG exStyle = GetWindowLongW(m_hWnd, GWL_EXSTYLE);
    DWORD newExStyle = add | (~remove & exStyle);
    SetWindowLongW(m_hWnd, GWL_EXSTYLE, newExStyle);
    UpdateWindow(m_hWnd);
    if (m_acceptFiles || (newExStyle & WS_EX_ACCEPTFILES) != 0)
    {
        m_acceptFiles = true;
        return 1;
    }
    m_acceptFiles = false;
    return 0;
}

BOOL Control::modifyFlags(int add, int remove)
{
    m_flags = ~remove & (add | m_flags);
    return invalidate(true);
}

BOOL Control::setFlags(int flags)
{
    m_flags = flags;
    return invalidate(true);
}

bool Control::openTheme(LPCWSTR classList, DWORD flags)
{
    if (m_theme)
        CloseThemeData(m_theme);
    m_theme = OpenThemeDataEx(m_hWnd, classList, flags);
    return m_theme != 0;
}

HRESULT Control::closeTheme()
{
    HRESULT result = (HRESULT)(INT_PTR)m_theme;
    if (m_theme)
    {
        result = CloseThemeData(m_theme);
        m_theme = 0;
    }
    return result;
}

LRESULT Control::windowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    return DefSubclassProc(m_hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Control::subclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
                                       DWORD_PTR refData)
{
    Control* control = (Control*)refData;
    if (!control)
        return DefSubclassProc(hWnd, msg, wParam, lParam);

    POINT pt;
    switch (msg)
    {
    case WM_DESTROY:
        control->closeTheme();
        control->onDestroy();
        RemoveWindowSubclass(hWnd, subclassProc, kControlSubclassId);
        control->m_hWnd = 0;
        break;
    case WM_SIZE:
        control->onSize(wParam, LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_ACTIVATE:
    {
        LRESULT result = DefSubclassProc(hWnd, WM_ACTIVATE, wParam, lParam);
        control->onActivate(wParam != 0);
        return result;
    }
    case WM_SETFOCUS:
        control->m_flags |= 0x10;
        control->onFocus(true);
        break;
    case WM_KILLFOCUS:
        control->m_flags &= ~0x10;
        control->onFocus(false);
        break;
    case WM_NOTIFY:
        if (lParam)
        {
            LRESULT result = 0;
            if (control->onNotify((NMHDR*)lParam, &result))
                return result;
        }
        break;
    case WM_KEYDOWN:
    case WM_KEYUP:
        if (control->onKey((short)wParam, (int)lParam, msg == WM_KEYDOWN))
            return 0;
        break;
    case WM_CHAR:
        if (control->onChar(wParam, lParam))
            return 0;
        break;
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
        if (control->onSysKey(wParam, lParam, msg == WM_SYSKEYDOWN))
            return 0;
        break;
    case WM_COMMAND:
        if (control->onCommand(HIWORD(wParam), lParam, LOWORD(wParam)))
            return 0;
        break;
    case WM_SYSCOMMAND:
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (control->onSysCommand(wParam, &pt))
            return 0;
        break;
    case WM_INITMENU:
        if (control->onInitMenuPopup((HMENU)wParam))
            return 0;
        break;
    case WM_MOUSEMOVE:
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (control->onMouseMove(wParam, &pt))
            return 0;
        break;
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (control->onLButton(wParam, &pt, msg == WM_LBUTTONDOWN))
            return 0;
        break;
    case WM_LBUTTONDBLCLK:
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (control->onDoubleClick(wParam, &pt, 0))
            return 0;
        break;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (control->onRButton(wParam, &pt, msg == WM_RBUTTONDOWN))
            return 0;
        break;
    case WM_RBUTTONDBLCLK:
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (control->onDoubleClick(wParam, &pt, 2))
            return 0;
        break;
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (control->onMButton(wParam, &pt, msg == WM_MBUTTONDOWN))
            return 0;
        break;
    case WM_MBUTTONDBLCLK:
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (control->onDoubleClick(wParam, &pt, 1))
            return 0;
        break;
    case WM_MOUSEWHEEL:
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (control->onMouseWheel((short)HIWORD(wParam), &pt, LOWORD(wParam)))
            return 0;
        break;
    case WM_DROPFILES:
        if (control->m_acceptFiles)
        {
            POINT cursor;
            cursor.y = 0;
            cursor.x = 0;
            GetCursorPos(&cursor);
            ScreenToClient(hWnd, &cursor);
            control->onDropFiles((HDROP)wParam, &cursor);
            return 0;
        }
        break;
    case WM_MOUSELEAVE:
        if (control->onMouseLeave())
            return 0;
        break;
    case WM_THEMECHANGED:
        control->closeTheme();
        return 0;
    }
    return control->windowProc(msg, wParam, lParam);
}
