#include "GDC.h"

HGDIOBJ GDC::selectPen(COLORREF color, int width, int style)
{
    HPEN pen = CreatePen(style, width, color);
    HGDIOBJ previous = SelectObject(m_hdc, pen);
    if (m_savedPen)
        return (HGDIOBJ)(INT_PTR)DeleteObject(previous);
    m_savedPen = previous;
    return previous;
}
