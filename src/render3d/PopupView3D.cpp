#include "PopupView3D.h"
#include "View3DInfo.h"
#include "Drawable.h"
#include "fPoint.h"
#include "gdi_cache.h"
#include "WinAppearance.h"
#include "menus.h"
#include "ida_compat.h"
#include "_imports_p06.h"
#include <commctrl.h>
#include <GL/gl.h>

LRESULT CALLBACK pfnSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR ,
                             DWORD_PTR )
{
    if (uMsg == WM_COMMAND || uMsg == WM_NOTIFY)
        return SendMessageW(GetParent(hWnd), uMsg, wParam, lParam);
    if (uMsg - WM_CTLCOLORMSGBOX > 6)
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    SetBkMode((HDC)wParam, TRANSPARENT);
    if (isWindowsNT5())
        return (LRESULT)getSolidBrush(0xD8E9EC);
    if (Window_isClassicAppearance())
        return (LRESULT)getSolidBrush(GetSysColor(COLOR_BTNFACE));
    return (LRESULT)getSolidBrush(0xF0F0F0);
}

bool PopupView3D::vslot72(HWND parent, const RECT* client)
{
    DWORD style = getStyle();
    int menuId = vslot78();
    HMENU menu = menuId > 0 ? LoadMenuW(hModule, MAKEINTRESOURCEW((WORD)menuId)) : 0;
    RECT rect = *client;
    AdjustWindowRect(&rect, style, FALSE);
    if (menu)
        rect.bottom += GetSystemMetrics(SM_CYMENU);
    int bottom = GetSystemMetrics(SM_CYMENU) + rect.bottom;
    int top = client->top;
    int height = top - client->bottom;
    int shift = top - rect.top;
    int newBottom = shift + bottom;
    int overflow = newBottom + height - (shift + rect.top);
    rect.top += shift;
    rect.bottom = newBottom;
    if (overflow > 0) {
        rect.right -= overflow;
        rect.bottom = newBottom - overflow;
    }
    return Window::create(parent, menu, &rect, style, 0, L"3D View");
}

bool PopupView3D::onCreate()
{
    HWND status = CreateWindowExW(0, L"msctls_statusbar32", 0, 0x50000100, CW_USEDEFAULT, CW_USEDEFAULT,
                                  CW_USEDEFAULT, CW_USEDEFAULT, m_hWnd, (HMENU)0x3E9, hModule, 0);
    m_hStatusBar = status;
    if (status) {
        RECT rc;
        SetRectEmpty(&rc);
        GetClientRect(status, &rc);
        m_statusHeight = rc.bottom;
        SetWindowSubclass(m_hStatusBar, pfnSubclass, 'stat', (DWORD_PTR)this);
        vslot80(10);
    }
    RECT client;
    getClientRect(&client);
    client.bottom -= m_statusHeight;
    if (!m_viewer.createView(m_hWnd, (HMENU)0x65, &client, 0x50000000, 0))
        return false;
    m_viewer.createMouseControler(0);
    vslot79();
    return true;
}

bool PopupView3D::vslot80(int x)
{
    HANDLE bitmap = LoadImageW(hModule, MAKEINTRESOURCEW(0xA9), IMAGE_BITMAP, 0, 0,
                               LR_CREATEDIBSECTION | LR_LOADTRANSPARENT);
    int id = 200;
    for (int i = 0; i < 3; ++i) {
        if (bitmap) {
            m_rotReset[i].create(m_hStatusBar, (HMENU)(INT_PTR)id, x, 3, 18, 18, bitmap, id - 71);
            x += 20;
        }
        m_rotEdit[i].createNumber(m_hStatusBar, (HMENU)(INT_PTR)(id + 10), x, 3, 50, 17, 0, 0, 0);
        HWND buddy = m_rotEdit[i].m_hWnd;
        x += 55;
        m_rotSpin[i].m_hWnd = CreateWindowExW(0, L"msctls_updown32", 0, 0x50000067, x, 3, 20, 17, m_hStatusBar,
                                              (HMENU)(INT_PTR)(id + 20), 0, 0);
        if (m_rotSpin[i].m_hWnd) {
            m_rotSpin[i].setFontId(m_rotSpin[i].m_fontId);
            SendMessageW(m_rotSpin[i].m_hWnd, UDM_SETBUDDY, (WPARAM)buddy, 0);
        } else {
            GetLastError();
        }
        SendMessageW(m_rotSpin[i].m_hWnd, UDM_SETRANGE32, 0, 360);
        ++id;
    }
    if (bitmap) {
        id = 230;
        for (int i = 0; i < 4; ++i) {
            m_posReset[i].create(m_hStatusBar, (HMENU)(INT_PTR)id, x, 3, 18, 18, bitmap, id - 98);
            x += 20;
            ++id;
        }
    }
    m_alpha.m_hWnd = CreateWindowExW(0, L"Button", L" Alpha Blending", 0x50000002, x + 5, 3, 90, 18,
                                     m_hStatusBar, (HMENU)0xF0, 0, 0);
    if (m_alpha.m_hWnd)
        m_alpha.setFontId(m_alpha.m_fontId);
    else
        GetLastError();
    if (m_alpha.m_hWnd)
        SetWindowSubclass(m_alpha.m_hWnd, noEraseSubclassProc, 'chck', (DWORD_PTR)&m_alpha);
    DeleteObject(bitmap);
    return true;
}

void PopupView3D::onActivate(bool active)
{
    if (active) {
        glViewer* viewer = &m_viewer;
        if (g_activeGlView != viewer) {
            g_activeGlView = viewer;
            viewer->activate(true);
        }
    }
}

bool PopupView3D::onCommand(UINT code, HWND ctrl, UINT id)
{
    LPARAM lParam = (LPARAM)ctrl;
    if (code == EN_UPDATE && id - 210 <= 3) {
        if (m_updating)
            return true;
        fPoint rotation(m_viewer.m_rotation.x, m_viewer.m_rotation.y, m_viewer.m_rotation.z);
        float value = (&m_rotEdit[0] + (id - 210))->getFloat();
        rotation.at(id - 210) = value;
        m_viewer.vfunc_11C((int)(INT_PTR)&rotation);
        return true;
    }
    if ((code | 1) == 1) {
        if (!ctrl) {
            if (id == 0xE11E) {
                if (m_viewer.m_drawable) {
                    View3DInfo info(m_viewer.m_drawable);
                    if (info.vslot78(m_hWnd))
                        info.doModal();
                }
                return true;
            }
            if (id == 0xE102) {
                show(false);
                return true;
            }
            goto forward;
        }
    } else if (!ctrl) {
        goto forward;
    }
    if (code)
        goto forward;
    if (id - 200 <= 2) {
        m_rotEdit[id - 200].setInt(code);
        fPoint rotation(m_viewer.m_rotation.x, m_viewer.m_rotation.y, m_viewer.m_rotation.z);
        rotation.at(id - 200) = (float)0;
        m_viewer.vfunc_11C((int)(INT_PTR)&rotation);
        return true;
    }
    switch (id) {
    case 0xE6: {
        fPoint position(0.0f, 0.0f, -2.5f);
        m_viewer.setPosition(&position);
        return true;
    }
    case 0xE7: {
        fPoint position(0.0f, m_viewer.m_position.y, m_viewer.m_position.z);
        m_viewer.setPosition(&position);
        return true;
    }
    case 0xE8: {
        fPoint position(m_viewer.m_position.x, 0.0f, m_viewer.m_position.z);
        m_viewer.setPosition(&position);
        return true;
    }
    case 0xE9: {
        fPoint position(m_viewer.m_position.x, m_viewer.m_position.y, -2.5f);
        m_viewer.setPosition(&position);
        return true;
    }
    case 0xF0:
        vslot75(!m_alpha.isChecked());
        return true;
    default:
        return Popup::onCommand(0, ctrl, id);
    }
forward:
    if (id < 0xE100)
        return false;
    {
        HWND child = GetDlgItem(m_hWnd, 101);
        if (!child)
            return false;
        SendMessageW(child, WM_COMMAND, (WPARAM)((WORD)id | ((WORD)code << 16)), lParam);
    }
    return true;
}

LRESULT PopupView3D::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_SHOWWINDOW) {
        if (wParam)
            clearModel();
        else
            g_activeGlView = 0;
    }
    return DefWindowProcW(m_hWnd, msg, wParam, lParam);
}

LRESULT PopupView3D::onCtlColor(UINT msg, HDC hdc, HWND )
{
    switch (msg) {
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
        Window_isClassicAppearance();
        return (LRESULT)getSolidBrush(GetSysColor(COLOR_WINDOW));
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
        if (Window_isClassicAppearance())
            return (LRESULT)getSolidBrush(GetSysColor(COLOR_BTNFACE));
        return (LRESULT)getSolidBrush(0xF0F0F0);
    case WM_CTLCOLORSCROLLBAR:
        Window_isClassicAppearance();
        return (LRESULT)getSolidBrush(GetSysColor(COLOR_SCROLLBAR));
    case WM_CTLCOLORSTATIC:
        SetBkMode(hdc, TRANSPARENT);
        Window_isClassicAppearance();
        if (m_bkColor == GetSysColor(COLOR_WINDOW)) {
            Window_isClassicAppearance();
            return (LRESULT)getSolidBrush(GetSysColor(COLOR_WINDOW));
        }
        if (Window_isClassicAppearance())
            return (LRESULT)getSolidBrush(GetSysColor(COLOR_BTNFACE));
        return (LRESULT)getSolidBrush(0xF0F0F0);
    default:
        return 0;
    }
}

void PopupView3D::vslot73(const fPoint* rotation)
{
    if (m_rotEdit[0].m_hWnd)
        m_rotEdit[0].setInt((int)rotation->x);
    if (m_rotEdit[1].m_hWnd)
        m_rotEdit[1].setInt((int)rotation->y);
    if (m_rotEdit[2].m_hWnd)
        m_rotEdit[2].setInt((int)rotation->z);
    m_viewer.vfunc_11C((int)(INT_PTR)rotation);
}

void PopupView3D::vslot74(const fPoint* position)
{
    m_viewer.setPosition(position);
}

void PopupView3D::vslot75(bool on)
{
    m_alpha.setCheck(on);
    if (m_viewer.m_drawable)
        m_viewer.m_drawable->setFlag(1, on);
    m_viewer.m_drawFlags = on != 0;
}

void PopupView3D::vslot79()
{
    HMENU menu = GetMenu(m_hWnd);
    if (!menu)
        return;
    HMENU scripts = findScriptsMenu(GetSubMenu(menu, 2));
    if (!scripts)
        return;
    clearMenu(scripts);
    fillScriptMenu(scripts);
    if (!GetMenuItemCount(scripts)) {
        MENUITEMINFOW mi;
        memset(&mi, 0, sizeof(mi));
        mi.cbSize = sizeof(mi);
        mi.fMask = MIIM_STATE | MIIM_ID | MIIM_STRING;
        mi.fType = 0;
        mi.wID = 1;
        mi.fState = MF_GRAYED;
        mi.dwTypeData = (LPWSTR)L"no script";
        InsertMenuItemW(scripts, (UINT)-1, FALSE, &mi);
    }
    DrawMenuBar(m_hWnd);
}

void PopupView3D::vslot1(Drawable* )
{
    glRotatef(m_angles.x, 1.0f, 0.0f, 0.0f);
    glRotatef(m_angles.y, 0.0f, 1.0f, 0.0f);
    glRotatef(m_angles.z, 0.0f, 0.0f, 1.0f);
}

void PopupView3D::vslot2(const fPoint* rotation)
{
    m_updating = true;
    if (m_rotEdit[0].m_hWnd)
        m_rotEdit[0].setInt((int)rotation->x);
    if (m_rotEdit[1].m_hWnd)
        m_rotEdit[1].setInt((int)rotation->y);
    if (m_rotEdit[2].m_hWnd)
        m_rotEdit[2].setInt((int)rotation->z);
    m_updating = false;
}

void PopupView3D::vslot3(const fPoint* )
{
    m_updating = false;
}
