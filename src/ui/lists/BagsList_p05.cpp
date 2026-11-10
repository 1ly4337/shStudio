#include "p05_common.h"
#include "BagsList.h"

BagsList::Items::~Items()
{
    deleteAll();
}

BagsList::~BagsList()
{
}

DWORD BagsList::getStyle()
{
    return WS_VISIBLE;
}

Node* BagsList::getNode(int index)
{
    return (Node*)m_items.get(index);
}

int BagsList::indexOfNode(Node* node)
{
    return m_items.indexOf(dynamic_cast<Item*>(node));
}

bool BagsList::moveItemUp(int index)
{
    return m_items.swap(index, index - 1);
}

bool BagsList::moveItemDown(int index)
{
    return m_items.swap(index, index + 1);
}

int BagsList::getListKind()
{
    return 32781;
}
