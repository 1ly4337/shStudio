#include <windows.h>
#include "Control.h"

BOOL Control::invalidate(bool erase)
{
    return InvalidateRect(m_hWnd, NULL, erase != 0);
}
