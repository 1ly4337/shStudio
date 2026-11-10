#include "p05_common.h"
#include "SettingsDial.h"

SettingsDial::SettingsDial(void* )
    : Modal(),
      m_pages(),
      m_panels(256)
{
    m_settings = &g_preferences;
    m_current = -1;
}

SettingsDial::~SettingsDial()
{
}

bool SettingsDial::create(HWND parent)
{
    SIZE size;
    getDialogSize(&size);
    LPCWSTR title = getTitle();
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, size.cx, size.cy);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, title);
    if (result) {
        HWND owner = (HWND)Modal_getTopLevelWindow(parent);
        HWND hWnd = m_hWnd;
        m_owner = owner;
        Window_centerOn(hWnd, owner);
        show(true);
        return true;
    }
    return result;
}

LPCWSTR SettingsDial::getTitle()
{
    return L"Settings";
}

SIZE* SettingsDial::getDialogSize(SIZE* out)
{
    out->cx = 360;
    out->cy = 350;
    return out;
}
