#include "p05_common.h"
#include "Window.h"

Window::~Window()
{
    HWND hWnd = m_hWnd;
    if (hWnd) {
        SendMessageW(hWnd, WM_SETFONT, 0, 0);
        DestroyWindow(m_hWnd);
        m_hWnd = 0;
    }
}
