#include "p05_common.h"
#include "WorldList.h"
#include "DocView.h"

DWORD WorldList::getExStyle()
{
    return WS_EX_ACCEPTFILES;
}

void WorldList::onItemsChanged()
{
    m_view->setModified(true);
}
