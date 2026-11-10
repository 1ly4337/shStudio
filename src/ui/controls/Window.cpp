#include "Window.h"
#include <shellapi.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <cwchar>
#include "GDC.h"
#include "Control.hpp"
#include "gdi_cache.h"
#include "dark_mode.h"
#include "_imports_p10.h"

int g_classicAppearance = -1;

bool Window_isClassicAppearance()
{
    if (g_classicAppearance < 0)
        g_classicAppearance = isClassicAppearance() != 0;
    return g_classicAppearance > 0;
}

static inline unsigned char& Window_flagBits(Window* w)
{
    return w->m_flags.m_bits;
}

Window::Window()
{
    m_sizeMovePos.y = 0;
    m_sizeMovePos.x = 0;
    m_sizeMoveSize.cy = 0;
    m_sizeMoveSize.cx = 0;
    Window_isClassicAppearance();
    m_bkColor = GetSysColor(COLOR_WINDOW);
    m_hWnd = 0;
    m_hNotify = 0;
    m_hTheme = 0;
    m_childId = 0;
    m_state = 0;
    Window_flagBits(this) |= 1u;
}

bool Window::openTheme(LPCWSTR classList, DWORD flags)
{
    if (m_hTheme)
    {
        CloseThemeData(m_hTheme);
        m_hTheme = 0;
    }
    if (classList)
        m_hTheme = OpenThemeDataEx(m_hWnd, classList, flags);
    return m_hTheme != 0;
}

HRESULT Window::closeTheme()
{
    HRESULT result = (HRESULT)(INT_PTR)m_hTheme;
    if (m_hTheme)
    {
        result = CloseThemeData(m_hTheme);
        m_hTheme = 0;
    }
    return result;
}

void Window::onThemeChanged()
{
    closeTheme();
    if (openTheme(getThemeClassList(0), 0))
        RedrawWindow(m_hWnd, 0, 0, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
}

static POINT* Window_getPosInParent(POINT* out, HWND hWnd)
{
    RECT rc;
    RECT parentRect;
    SetRectEmpty(&rc);
    SetRectEmpty(&parentRect);
    GetWindowRect(hWnd, &rc);
    HWND parent = GetParent(hWnd);
    if (parent)
        GetWindowRect(parent, &parentRect);
    out->x = rc.left - parentRect.left;
    out->y = rc.top - parentRect.top;
    return out;
}

static bool Window_positionChanged(HWND hWnd, POINT* pos)
{
    RECT rc;
    RECT parentRect;
    SetRectEmpty(&rc);
    SetRectEmpty(&parentRect);
    GetWindowRect(hWnd, &rc);
    HWND parent = GetParent(hWnd);
    if (parent)
        GetWindowRect(parent, &parentRect);
    int y = rc.top - parentRect.top;
    if (rc.left - parentRect.left == pos->x && y == pos->y)
        return false;
    pos->x = rc.left - parentRect.left;
    pos->y = y;
    return true;
}

static SIZE* Window_getSize(SIZE* out, HWND hWnd)
{
    RECT rc;
    SetRectEmpty(&rc);
    GetWindowRect(hWnd, &rc);
    out->cx = rc.right - rc.left;
    out->cy = rc.bottom - rc.top;
    return out;
}

static bool Window_sizeChanged(HWND hWnd, SIZE* size)
{
    RECT rc;
    SetRectEmpty(&rc);
    GetWindowRect(hWnd, &rc);
    int cy = rc.bottom - rc.top;
    if (size->cx == rc.right - rc.left && size->cy == cy)
        return false;
    size->cx = rc.right - rc.left;
    size->cy = cy;
    return true;
}

HWND Window::createStatic(int x, int y, int width, int height, LPCWSTR text, DWORD style, unsigned int fontId)
{
    HWND result = CreateWindowExW(0, L"Static", text, style | WS_CHILD | WS_VISIBLE, x, y, width, height,
                                  m_hWnd, 0, 0, 0);
    if (result)
    {
        WPARAM font = (WPARAM)getStdFont((int)fontId);
        SendMessageW(result, WM_SETFONT, font, 0);
    }
    return result;
}

HWND Window::createButton(HMENU id, int x, int y, int width, int height, LPCWSTR text, int unused1, int unused2)
{
    HWND result = CreateWindowExW(0, L"Button", text, WS_CHILD | WS_VISIBLE, x, y, width, 24,
                                  m_hWnd, id, 0, 0);
    if (result)
    {
        WPARAM font = (WPARAM)getStdFont(0);
        SendMessageW(result, WM_SETFONT, font, 0);
    }
    return result;
}

BOOL Window_centerOnScreen(HWND hWnd)
{
    RECT rc;
    SetRectEmpty(&rc);
    BOOL result = GetWindowRect(hWnd, &rc);
    if (result)
    {
        int width = rc.right - rc.left;
        int x = GetSystemMetrics(SM_CXSCREEN) - width;
        int height = rc.bottom - rc.top;
        int screenY = GetSystemMetrics(SM_CYSCREEN);
        return SetWindowPos(hWnd, 0, x / 2, (screenY - height) / 2, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
    }
    return result;
}

BOOL Window_centerOn(HWND hWnd, HWND reference)
{
    if (!reference)
        return Window_centerOnScreen(hWnd);
    RECT refRect;
    RECT rc;
    SetRectEmpty(&rc);
    SetRectEmpty(&refRect);
    BOOL result = GetWindowRect(hWnd, &rc);
    if (result)
    {
        result = GetWindowRect(reference, &refRect);
        if (result)
        {
            if ((GetWindowLongW(hWnd, GWL_STYLE) & WS_CHILD) != 0 && (HWND)(LONG_PTR)GetWindowLongW(hWnd, GWL_HWNDPARENT) == reference)
            {
                return SetWindowPos(hWnd, 0,
                                    (rc.left + refRect.right - rc.right - refRect.left) / 2,
                                    (refRect.bottom + rc.top - rc.bottom - refRect.top) / 2,
                                    0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
            }
            int x = refRect.left + (refRect.right - refRect.left) / 2 - (rc.right - rc.left) / 2;
            int y = refRect.top + (refRect.bottom - refRect.top) / 2 - (rc.bottom - rc.top) / 2;
            if (refRect.top + 10 > y)
                y = refRect.top + 10;
            if (x < 100)
                x = 100;
            return SetWindowPos(hWnd, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
        }
    }
    return result;
}

bool Window::create(HWND parent, HMENU menuOrId, const RECT* rect, DWORD style, DWORD exStyle, LPCWSTR windowName)
{
    LPCWSTR className = getClassName();
    WNDCLASSEXW wcx;
    memset(&wcx, 0, sizeof(wcx));
    wcx.cbSize = sizeof(WNDCLASSEXW);
    if (!GetClassInfoExW(hModule, className, &wcx))
    {
        wcx.lpszClassName = className;
        wcx.lpfnWndProc = Window::wndProc;
        wcx.hInstance = hModule;
        wcx.hCursor = LoadCursorW(0, IDC_ARROW);
        wcx.hbrBackground = CreateSolidBrush(dmBk(m_bkColor));
        onRegisterClass(&wcx);
        if (!RegisterClassExW(&wcx))
            return false;
    }
    DWORD fullStyle = getStyle() | style;
    DWORD fullExStyle = getExStyle() | exStyle;
    int width = rect->right;
    if (rect->left != (LONG)0x80000000)
        width -= rect->left;
    int height = rect->bottom;
    if (rect->top != (LONG)0x80000000)
        height -= rect->top;
    HMENU menu = menuOrId;
    DWORD isChild = fullStyle & WS_CHILD;
    if (isChild)
    {
        m_childId = menuOrId;
        menu = 0;
    }
    if (!CreateWindowExW(fullExStyle, className, windowName, fullStyle, rect->left, rect->top, width, height,
                         parent, menu, hModule, this))
    {
        GetLastError();
        return false;
    }
    HWND hWnd = m_hWnd;
    if (!hWnd)
        throw "window class error";
    if (isChild)
        SetWindowLongW(hWnd, GWL_ID, (LONG)(LONG_PTR)m_childId);
    m_hNotify = parent;
    return true;
}

HWND Window::destroy()
{
    HWND result = m_hWnd;
    if (result)
    {
        SendMessageW(result, WM_SETFONT, 0, 0);
        result = (HWND)(INT_PTR)DestroyWindow(m_hWnd);
        m_hWnd = 0;
    }
    return result;
}

Window* Window::fromHwnd(HWND hWnd, bool searchParents)
{
    HWND wnd = hWnd;
    WNDPROC proc = (WNDPROC)(LONG_PTR)GetClassLongW(hWnd, GCL_WNDPROC);
    if (!proc)
        return 0;
    if (proc != Window::wndProc)
    {
        if (!searchParents)
            return 0;
        wnd = GetParent(wnd);
        if (!wnd)
            return 0;
        while ((WNDPROC)(LONG_PTR)GetClassLongW(wnd, GCL_WNDPROC) != Window::wndProc)
        {
            wnd = GetParent(wnd);
            if (!wnd)
                return 0;
        }
    }
    Window* result = (Window*)(LONG_PTR)GetWindowLongW(wnd, GWL_USERDATA);
    if (!result)
        return (Window*)(LONG_PTR)GetWindowLongW(wnd, DWLP_USER);
    return result;
}

LRESULT CALLBACK Window::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static WCHAR s_dropPath[512];

    if (msg == WM_NCCREATE)
    {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
        Window* created = (Window*)cs->lpCreateParams;
        if (created)
        {
            created->m_hWnd = hWnd;
            SetWindowLongW(hWnd, GWL_USERDATA, (LONG)(LONG_PTR)created);
            if (created->onNcCreate(cs))
            {
                if (cs->hwndParent)
                {
                    if (SendMessageW(cs->hwndParent, WM_NOTIFYFORMAT, (WPARAM)hWnd, NF_QUERY) == NFR_UNICODE)
                        Window_flagBits(created) |= 1u;
                    else
                        Window_flagBits(created) &= ~1u;
                }
                return DefWindowProcW(hWnd, WM_NCCREATE, wParam, lParam);
            }
        }
        return 0;
    }

    LRESULT result = 0;
    Window* self = (Window*)(LONG_PTR)GetWindowLongW(hWnd, GWL_USERDATA);
    if (!self)
        return DefWindowProcW(hWnd, msg, wParam, lParam);

    if (msg <= WM_KEYUP)
    {
        if (msg >= WM_KEYDOWN)
        {
            if (self->onKey(wParam, lParam, msg == WM_KEYDOWN))
                return 0;
            return self->defWindowProc(msg, wParam, lParam);
        }
        switch (msg)
        {
        case WM_CREATE:
            return (self->onCreate() != 0) - 1;
        case WM_DESTROY:
            self->onDestroy();
            self->m_hWnd = 0;
            return 0;
        case WM_MOVE:
            self->onMove(LOWORD(lParam), HIWORD(lParam));
            return 0;
        case WM_SIZE:
            self->onSize(wParam, LOWORD(lParam), HIWORD(lParam));
            return 0;
        case WM_ACTIVATE:
            result = DefWindowProcW(hWnd, msg, wParam, lParam);
            self->onActivate(wParam != 0);
            return result;
        case WM_SETFOCUS:
            self->m_state |= 0x10u;
            self->onFocus(true);
            return self->defWindowProc(msg, wParam, lParam);
        case WM_KILLFOCUS:
            self->m_state &= ~0x10u;
            self->onFocus(false);
            return self->defWindowProc(msg, wParam, lParam);
        case WM_ENABLE:
            self->onEnable(wParam == 1);
            return self->defWindowProc(msg, wParam, lParam);
        case WM_CLOSE:
            if ((GetWindowLongW(self->m_hWnd, GWL_STYLE) & WS_CHILD) != 0 || !self->onClose())
                return 0;
            DestroyWindow(self->m_hWnd);
            return 0;
        case WM_ERASEBKGND:
        {
            GDC dc((HDC)wParam);
            RECT rc;
            COLORREF color = dmBk(self->m_bkColor);
            dc.fillRect(self->getClientRect(&rc), color);
            return 1;
        }
        case WM_GETMINMAXINFO:
            if (!self->onGetMinMaxInfo((MINMAXINFO*)lParam))
                return self->defWindowProc(msg, wParam, lParam);
            return 0;
        case WM_SETFONT:
            self->onSetFont((HFONT)wParam, (WORD)lParam == 1);
            return 0;
        case WM_NOTIFY:
        {
            if (!lParam)
                return self->defWindowProc(msg, wParam, lParam);
            Control* control = Control::fromHandle(((NMHDR*)lParam)->hwndFrom);
            if (control && control->onNotify((NMHDR*)lParam, &result))
                return result;
            if (!self->onNotify((NMHDR*)lParam, &result))
                return self->defWindowProc(msg, wParam, lParam);
            return result;
        }
        case WM_NOTIFYFORMAT:
            if (lParam == NF_REQUERY)
            {
                LRESULT format = SendMessageW(self->m_hNotify, WM_NOTIFYFORMAT, (WPARAM)hWnd, NF_QUERY);
                self->m_flags.set(format == NFR_UNICODE, format == NFR_UNICODE);
                return format;
            }
            result = 0;
            if (lParam == NF_QUERY)
                return NFR_UNICODE;
            return result;
        case WM_NCDESTROY:
            self->onNcDestroy();
            return 0;
        default:
            return self->defWindowProc(msg, wParam, lParam);
        }
    }

    if (msg <= WM_COMMAND)
    {
        if (msg == WM_COMMAND)
        {
            if (self->onCommand(HIWORD(wParam), (HWND)lParam, LOWORD(wParam)))
                return 0;
        }
        else if (msg - WM_SYSKEYDOWN <= 1
                 && self->onSysKey(wParam, lParam, msg == WM_SYSKEYDOWN))
        {
            return 0;
        }
        return self->defWindowProc(msg, wParam, lParam);
    }

    if (msg > WM_ENTERSIZEMOVE)
    {
        if (msg > WM_THEMECHANGED)
        {
            switch (msg)
            {
            case CCM_SETUNICODEFORMAT:
                return self->m_flags.set(0, wParam != 0) != 0;
            case CCM_GETUNICODEFORMAT:
                return Window_flagBits(self) & 1;
            case CCM_SETNOTIFYWINDOW:
                if (wParam)
                {
                    result = (LRESULT)self->m_hNotify;
                    self->m_hNotify = (HWND)wParam;
                    return result;
                }
                return 0;
            }
            return self->defWindowProc(msg, wParam, lParam);
        }
        if (msg == WM_THEMECHANGED)
        {
            self->onThemeChanged();
            return 0;
        }
        if (msg == WM_EXITSIZEMOVE)
        {
            if (Window_positionChanged(self->m_hWnd, &self->m_sizeMovePos))
                self->onMoved();
            if (Window_sizeChanged(self->m_hWnd, &self->m_sizeMoveSize))
            {
                self->onResized();
                return 0;
            }
            return 0;
        }
        if (msg != WM_DROPFILES)
        {
            if (msg == WM_MOUSELEAVE && self->onMouseLeave())
                return 0;
            return self->defWindowProc(msg, wParam, lParam);
        }
        if (!wParam || lParam)
            return 0;
        int index = (int)lParam;
        POINT pt = { 0, 0 };
        GetCursorPos(&pt);
        HWND target = WindowFromPoint(pt);
        int targetId = GetDlgCtrlID(target);
        int count = (int)DragQueryFileW((HDROP)wParam, 0xFFFFFFFF, 0, 0);
        if (count > 0)
        {
            do
            {
                memset(s_dropPath, 0, sizeof(s_dropPath));
                if (DragQueryFileW((HDROP)wParam, index, s_dropPath, 512))
                {
                    LPCWSTR ext = PathFindExtensionW(s_dropPath);
                    if (ext)
                    {
                        int cmp = wcscmp(ext, L".lnk");
                        if (cmp)
                            cmp = cmp < 0 ? -1 : 1;
                        if (!cmp)
                            resolveShortcut(s_dropPath, s_dropPath);
                    }
                    if (!self->onDropFile(s_dropPath, target, targetId))
                        break;
                }
                ++index;
            } while (index < count);
        }
        DragFinish((HDROP)wParam);
        return 0;
    }

    if (msg == WM_ENTERSIZEMOVE)
    {
        POINT pos;
        Window_getPosInParent(&pos, self->m_hWnd);
        self->m_sizeMovePos = pos;
        SIZE size;
        Window_getSize(&size, self->m_hWnd);
        self->m_sizeMoveSize = size;
        self->onEnterSizeMove();
        return 0;
    }

    POINT pt;
    switch (msg)
    {
    case WM_SYSCOMMAND:
        if (wParam == SC_MAXIMIZE && self->onMaximize())
            return 0;
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (!self->onSysCommand(wParam, &pt))
            return self->defWindowProc(msg, wParam, lParam);
        result = 0;
        break;
    case WM_TIMER:
        KillTimer(hWnd, wParam);
        if (!self->onTimer(wParam))
            return self->defWindowProc(msg, wParam, lParam);
        return 0;
    case WM_HSCROLL:
    case WM_VSCROLL:
        if (!self->onScroll(msg == WM_HSCROLL, LOWORD(wParam), HIWORD(wParam)))
            return self->defWindowProc(msg, wParam, lParam);
        return 0;
    case WM_INITMENU:
        if (!self->onInitMenu((HMENU)wParam))
            return self->defWindowProc(msg, wParam, lParam);
        return 0;
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
        result = self->onCtlColor(msg, (HDC)wParam, (HWND)lParam);
        if (!result)
            return self->defWindowProc(msg, wParam, lParam);
        return result;
    case WM_MOUSEMOVE:
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (!self->onMouseMove(wParam, &pt))
            return self->defWindowProc(msg, wParam, lParam);
        return 0;
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (!self->onLButton(wParam, &pt, msg == WM_LBUTTONDOWN))
            return self->defWindowProc(msg, wParam, lParam);
        return 0;
    case WM_LBUTTONDBLCLK:
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (!self->onDblClick(wParam, &pt, 0))
            return self->defWindowProc(msg, wParam, lParam);
        return 0;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (!self->onRButton(wParam, &pt, msg == WM_RBUTTONDOWN))
            return self->defWindowProc(msg, wParam, lParam);
        return 0;
    case WM_RBUTTONDBLCLK:
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (!self->onDblClick(wParam, &pt, 2))
            return self->defWindowProc(msg, wParam, lParam);
        return 0;
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (!self->onMButton(wParam, &pt, msg == WM_MBUTTONDOWN))
            return self->defWindowProc(msg, wParam, lParam);
        return 0;
    case WM_MBUTTONDBLCLK:
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (!self->onDblClick(wParam, &pt, 1))
            return self->defWindowProc(msg, wParam, lParam);
        return 0;
    case WM_MOUSEWHEEL:
        pt.x = (short)LOWORD(lParam);
        pt.y = (short)HIWORD(lParam);
        if (!self->onMouseWheel((short)HIWORD(wParam), &pt, LOWORD(wParam)))
            return self->defWindowProc(msg, wParam, lParam);
        return 0;
    default:
        return self->defWindowProc(msg, wParam, lParam);
    }
    return result;
}

LRESULT Window::onCtlColor(UINT msg, HDC hdc, HWND ctrl)
{
    switch (msg)
    {
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLOREDIT:
        Window_isClassicAppearance();
        return (LRESULT)getSolidBrush(GetSysColor(COLOR_WINDOW));
    case WM_CTLCOLORLISTBOX:
        Window_isClassicAppearance();
        return (LRESULT)getSolidBrush(GetSysColor(COLOR_WINDOW));
    case WM_CTLCOLORBTN:
        if ((GetWindowLongW(ctrl, GWL_STYLE) & 2) != 0)
        {
            SetBkMode(hdc, TRANSPARENT);
            COLORREF face = Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0;
            if (m_bkColor != face)
            {
                Window_isClassicAppearance();
                return (LRESULT)getSolidBrush(GetSysColor(COLOR_WINDOW));
            }
            if (!Window_isClassicAppearance())
                return (LRESULT)getSolidBrush(0xF0F0F0);
        }
        else if (!Window_isClassicAppearance())
        {
            return (LRESULT)getSolidBrush(0xF0F0F0);
        }
        return (LRESULT)getSolidBrush(GetSysColor(COLOR_BTNFACE));
    case WM_CTLCOLORDLG:
        if (!Window_isClassicAppearance())
            return (LRESULT)getSolidBrush(0xF0F0F0);
        return (LRESULT)getSolidBrush(GetSysColor(COLOR_BTNFACE));
    case WM_CTLCOLORSCROLLBAR:
        Window_isClassicAppearance();
        return (LRESULT)getSolidBrush(GetSysColor(COLOR_SCROLLBAR));
    case WM_CTLCOLORSTATIC:
    {
        SetBkMode(hdc, TRANSPARENT);
        COLORREF face = Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0;
        if (m_bkColor == face)
        {
            if (!Window_isClassicAppearance())
                return (LRESULT)getSolidBrush(0xF0F0F0);
            return (LRESULT)getSolidBrush(GetSysColor(COLOR_BTNFACE));
        }
        Window_isClassicAppearance();
        return (LRESULT)getSolidBrush(GetSysColor(COLOR_WINDOW));
    }
    default:
        return 0;
    }
}

LRESULT Window::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProcW(m_hWnd, msg, wParam, lParam);
}

HWND Window_fromPoint(const POINT* pt)
{
    HWND result = WindowFromPoint(*pt);
    HWND wnd = result;
    if (!result)
        return result;
    while (true)
    {
        WNDPROC proc = (WNDPROC)(LONG_PTR)GetClassLongW(wnd, GCL_WNDPROC);
        if (proc && proc == Window::wndProc
            && (GetWindowLongW(wnd, GWL_USERDATA) || GetWindowLongW(wnd, DWLP_USER)))
            break;
        if (wnd)
        {
            DWORD_PTR refData = 0;
            if (GetWindowSubclass(wnd, (SUBCLASSPROC)Control::subclassProc, 0x6374726Cu , &refData) && refData)
                break;
        }
        if ((GetWindowLongW(wnd, GWL_STYLE) & WS_CHILD) == 0)
            break;
        wnd = GetParent(wnd);
    }
    return wnd;
}
