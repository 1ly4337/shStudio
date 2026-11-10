#include "PaintDC.h"

PaintDC::PaintDC(HWND hWnd)
    : GDC(0), m_hWnd(hWnd)
{
    m_hdc = BeginPaint(hWnd, &m_ps);
}

PaintDC::~PaintDC()
{
    restore();
    EndPaint(m_hWnd, &m_ps);
}
