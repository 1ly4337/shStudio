#include "VaniCtrlsPanel.h"
#include "Doc.h"
#include "Vani.h"
#include "ida_compat.h"
#include "_imports_p06.h"

static inline HWND createLabel(HWND parent, LPCWSTR text, int x, int y, int cx, int cy)
{
    HWND hwnd = CreateWindowExW(0, L"Static", text, WS_CHILD | WS_VISIBLE, x, y, cx, cy, parent, 0, 0, 0);
    if (hwnd)
        SendMessageW(hwnd, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    return hwnd;
}

void VaniCtrlsPanel::setVani(Vani* vani)
{
    m_steps.setInt(FIELD(int, vani, 0x70));
    m_subItems.setInt(FIELD(int, vani, 0x6C));
}

bool VaniCtrlsPanel::onCreate()
{
    bool ok = CtrlsPanel::onCreate();
    if (!ok)
        return ok;
    int y = m_bottom;
    createLabel(m_hWnd, L"# of animation steps", 30, y + 2, 100, 16);
    m_steps.createNumber(m_hWnd, (HMENU)141, 140, y, 60, 18, 0, WS_DISABLED, 0);
    y += 24;
    createLabel(m_hWnd, L"# of 3D sub-items", 30, y + 2, 100, 16);
    m_subItems.createNumber(m_hWnd, (HMENU)140, 140, y, 60, 18, 0, WS_DISABLED, 0);
    return true;
}

bool VaniCtrlsPanel::onCommand(UINT code, HWND ctrl, UINT id)
{
    m_doc->setStatusText(0, 0);
    if (code || id != 250)
        return CtrlsPanel::onCommand(code, ctrl, id);
    return true;
}
