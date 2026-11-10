#include "p05_common.h"
#include "TextView.h"

TextView::TextView(FileTree* node, bool readOnly)
    : SimpleView(node), m_edit(), m_text()
{
    m_84 = 0;
    m_readOnly = readOnly;
}

TextView::~TextView()
{
}

void TextView::setText(String& text)
{
    if (&text != &m_text) {
        wchar_t* src = text.m_buffer;
        int len = text.m_length;
        wchar_t* old = m_text.m_buffer;
        m_text.m_length = 0;
        m_text.m_capacity = 0;
        if (old) {
            delete[] old;
            m_text.m_buffer = 0;
        }
        m_text.append(src, len);
    }
}

LPCWSTR TextView::getClassName()
{
    return L"TextView";
}

void TextView::onRegisterClass(WNDCLASSEXW* wc)
{
    int cy = GetSystemMetrics(SM_CYICON);
    int cx = GetSystemMetrics(SM_CXICON);
    wc->hIcon = (HICON)LoadImageW(hModule, (LPCWSTR)0x81, IMAGE_ICON, cx, cy, LR_SHARED);
    int cySm = GetSystemMetrics(SM_CYSMICON);
    int cxSm = GetSystemMetrics(SM_CXSMICON);
    wc->hIconSm = (HICON)LoadImageW(hModule, (LPCWSTR)0x81, IMAGE_ICON, cxSm, cySm, LR_SHARED);
}

int TextView::getMenuId()
{
    return 164;
}

void TextView::onActivate(bool active)
{
    if (active)
        SetFocus(m_edit.m_hWnd);
}

void TextView::fillScriptMenu(HMENU menu)
{
    Popup::appendMenuItem(menu, 0xE435);
}
