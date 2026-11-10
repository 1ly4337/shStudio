#include "p05_common.h"
#include "ListGears.h"

ListGears::~ListGears()
{
}

Node* ListGears::getNode(int index)
{
    return (Node*)m_set.get(index);
}
