#include "p05_common.h"
#include "WorkspaceNode.h"

WorkspaceNode::WorkspaceNode(void* owner, int image, int popupMenu)
    : Node()
{
    m_owner = owner;
    m_image = image;
    m_popupMenu = popupMenu;
}

int WorkspaceNode::getId() const
{
    return m_image;
}

int WorkspaceNode::queryInfo(int id)
{
    if (id == 'size')
        return -1;
    if (id == 'popm')
        return m_popupMenu;
    return id == '1col';
}

int WorkspaceNode::getPopupMenu()
{
    return m_popupMenu;
}
