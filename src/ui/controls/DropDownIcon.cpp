#include <windows.h>
#include "DropDownIcon.h"
#include "ImageStrip.h"
#include "GDC.h"
#include "_imports_p09.h"
#include "_imports_g6.h"

int DropDownIcon::getItemCount()
{
    return m_icons->m_count;
}

void DropDownIcon::drawItem(GDC* dc, int index, RECT* rc)
{
    m_icons->draw(dc->m_hdc, rc->left, rc->top, index);
    if (index == m_hot)
        dc->frameRect(rc, 0xFF);
}
