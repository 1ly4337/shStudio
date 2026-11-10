#include "GDC.h"

BOOL GDC::line(int x1, int y1, int x2, int y2)
{
    MoveToEx(m_hdc, x1, y1, 0);
    return LineTo(m_hdc, x2, y2);
}
