#include "p05_common.h"
#include "TreeList.h"
#include "Node.h"

BOOL TreeList::invalidateItem(int item)
{
    RECT rc;
    LPRECT r = m_tree.getItemRect(&rc, (HTREEITEM)item);
    return InvalidateRect(m_tree.m_hWnd, r, FALSE);
}

int TreeList::getNodeImage(Node* node)
{
    if (node)
        return node->getId();
    return -1;
}

DWORD TreeList::getDragEffects()
{
    return 1;
}
