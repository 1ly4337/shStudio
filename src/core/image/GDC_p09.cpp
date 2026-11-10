#include <windows.h>
#include "GDC.h"
#include "_imports_p09.h"
#include "_imports_g6.h"

BOOL GDC::frameRect(int left, int top, int right, int bottom, COLORREF color)
{
    HPEN pen = CreatePen(PS_SOLID, 1, color);
    HGDIOBJ previous = SelectObject(m_hdc, pen);
    if (m_savedPen)
        DeleteObject(previous);
    else
        m_savedPen = previous;
    MoveToEx(m_hdc, left, 0, 0);
    LineTo(m_hdc, right - 1, 0);
    LineTo(m_hdc, right - 1, bottom - 1);
    LineTo(m_hdc, left, bottom - 1);
    return LineTo(m_hdc, left, 0);
}
