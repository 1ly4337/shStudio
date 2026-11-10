#include "MapHeightView.h"
#include "MapHeight.h"
#include "Drawable.h"

DWORD MapHeightView::getStyle()
{
    return 0x80CE0000;
}

int MapHeightView::vslot78()
{
    return 0xB0;
}

void MapHeightView::onDestroy()
{
    static_cast<Drawable*>(m_doc)->clear();
}
