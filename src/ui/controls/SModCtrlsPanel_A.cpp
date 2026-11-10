#include "SModCtrlsPanel.h"
#include "glViewer.h"
#include "Drawable.h"
#include "fPoint.h"
#include "Preferences.h"
#include "gdi_cache.h"
#include "Container.h"
#include "_imports_p06.h"

bool SModCtrlsPanel::onCreate()
{
    bool result = CtrlsPanel::onCreate();
    if (!result)
        return result;

    int y = m_bottom + 12;
    m_btn1.m_hWnd = CreateWindowExW(0, L"Button", L" Draw bounds", 0x50010002, 10, y, 100, 18, m_hWnd,
                                    (HMENU)0x12E, 0, 0);
    if (m_btn1.m_hWnd)
        m_btn1.setFontId(m_btn1.m_fontId);
    else
        GetLastError();
    y += 24;
    m_btn2.m_hWnd = CreateWindowExW(0, L"Button", L" Alpha Blending", 0x50010002, 10, y, 100, 18, m_hWnd,
                                    (HMENU)0x12F, 0, 0);
    if (m_btn2.m_hWnd)
        m_btn2.setFontId(m_btn2.m_fontId);
    else
        GetLastError();
    m_btn2.setCheck(g_preferences.getInt(L"3D View", L"Transparency", 0) == 1);

    HWND label = CreateWindowExW(0, L"Static", L"Draw Mode:", 0x50000000, 10, y + 24, 100, 16, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    m_combo1.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"ComboBox", 0, (m_combo1.m_dropLines & 3) | 0x50010000, 10,
                                      y + 40, 100, 60, m_hWnd, (HMENU)0x12C, 0, 0);
    if (m_combo1.m_hWnd)
        m_combo1.setFontId(m_combo1.m_fontId);
    else
        GetLastError();
    if (SendMessageW(m_combo1.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Wireframe") != CB_ERR)
        m_combo1.m_items.StringTable::put((int)SendMessageW(m_combo1.m_hWnd, CB_GETCOUNT, 0, 0), L"Wireframe");
    if (SendMessageW(m_combo1.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Texture") != CB_ERR)
        m_combo1.m_items.StringTable::put((int)SendMessageW(m_combo1.m_hWnd, CB_GETCOUNT, 0, 0), L"Texture");
    SendMessageW(m_combo1.m_hWnd, CB_SETCURSEL, 1, 0);

    label = CreateWindowExW(0, L"Static", L"View Mode:", 0x50000000, 10, y + 75, 100, 16, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    m_combo2.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"ComboBox", 0, (m_combo2.m_dropLines & 3) | 0x50010000, 10,
                                      y + 91, 100, 60, m_hWnd, (HMENU)0x12D, 0, 0);
    if (m_combo2.m_hWnd)
        m_combo2.setFontId(m_combo2.m_fontId);
    else
        GetLastError();
    if (SendMessageW(m_combo2.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"External view") != CB_ERR)
        m_combo2.m_items.StringTable::put((int)SendMessageW(m_combo2.m_hWnd, CB_GETCOUNT, 0, 0), L"External view");
    if (SendMessageW(m_combo2.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Thrid Person") != CB_ERR)
        m_combo2.m_items.StringTable::put((int)SendMessageW(m_combo2.m_hWnd, CB_GETCOUNT, 0, 0), L"Thrid Person");
    SendMessageW(m_combo2.m_hWnd, CB_SETCURSEL, 0, 0);

    m_viewer->createMouseControler(0);
    return true;
}

bool SModCtrlsPanel::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code == 0)
    {
        Drawable* drawable = m_viewer->m_drawable;
        if (id == 302)
        {
            bool on = SendMessageW(ctrl, BM_GETCHECK, 0, 0) != BST_CHECKED;
            SendMessageW(ctrl, BM_SETCHECK, on, 0);
            if (drawable)
                drawable->setFlag(2, on);
            return true;
        }
        if (id == 303)
        {
            bool checked = Container_toggleCheck(ctrl);
            g_preferences.writeInt(L"3D View", L"Transparency", checked != 0);
            m_viewer->setDrawFlag(0, checked);
            return true;
        }
        return CtrlsPanel::onCommand(0, ctrl, id);
    }
    if (code != CBN_SELCHANGE)
        return CtrlsPanel::onCommand(code, ctrl, id);
    if (id == 300)
    {
        m_viewer->setRenderMode((int)SendMessageW(m_combo1.m_hWnd, CB_GETCURSEL, 0, 0));
        return true;
    }
    if (id == 301)
    {
        int mode = (int)SendMessageW(m_combo2.m_hWnd, CB_GETCURSEL, 0, 0);
        m_viewer->setNavigationMode(mode);
        fPoint rotation;
        if (mode)
            rotation.y = 225.0f;
        setRotation(&rotation);
        return true;
    }
    return CtrlsPanel::onCommand(code, ctrl, id);
}
