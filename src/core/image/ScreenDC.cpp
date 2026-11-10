#include "ScreenDC.h"

ScreenDC::ScreenDC()
    : GDC(0)
{
    m_displayDC = CreateDCW(L"DISPLAY", 0, 0, 0);
    m_hdc = CreateCompatibleDC(m_displayDC);
    m_prevBitmap = 0;
}

ScreenDC::~ScreenDC()
{
    restore();
    if (m_prevBitmap)
    {
        SelectObject(m_hdc, m_prevBitmap);
        m_prevBitmap = 0;
    }
    DeleteDC(m_hdc);
    DeleteDC(m_displayDC);
}
