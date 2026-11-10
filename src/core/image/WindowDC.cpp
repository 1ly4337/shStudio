#include <windows.h>
#include "WindowDC.h"
#include "_imports_p09.h"
#include "_imports_g6.h"

WindowDC::WindowDC(HWND hWnd) : GDC(GetWindowDC(hWnd)), m_hWnd(hWnd)
{
}

WindowDC::~WindowDC()
{
    restore();
    HDC& hdc = m_hdc;
    if (hdc)
    {
        ReleaseDC(m_hWnd, hdc);
        hdc = 0;
    }
}
