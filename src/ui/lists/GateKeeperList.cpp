#include "GateKeeperList.h"
#include "NpcData.h"
#include "StandardNPC.h"
#include "GateKeeper.h"
#include "DocView.h"
#include "ida_compat.h"
#include "_imports_E.h"
#include "Collection.hpp"

GateKeeperList::GateKeeperList(DocView* view, NpcData* data)
    : NpcList(view, data)
{
    m_list = &data->m_npcs[2];
}

Node* GateKeeperList::getNode(int index)
{
    return m_list->get(index);
}

bool GateKeeperList::sort()
{
    int column2 = m_prevSortColumn;
    StandardNPCs* list = m_list;
    bool ascending = m_sortAscending;
    int count = list->m_count;
    int column = m_sortColumn;
    if (count < 2)
        return false;
    list->m_changes = 0;
    quickSort(*list, 0, count - 1, column, column2, ascending);
    return list->m_changes > 0;
}

int GateKeeperList::indexOfNode(Node* node)
{
    return m_list->indexOf(dynamic_cast<GateKeeper*>(node));
}

Node* GateKeeperList::findNode(Node* like)
{
    short id = FIELD(short, &dynamic_cast<StandardNPC&>(*like), 0x10);
    StandardNPCs* list = m_list;
    int count = list->m_count;
    for (int i = 0; i < count; ++i)
    {
        if (FIELD(short, list->m_items[i], 0x10) == id)
            return list->m_items[i];
    }
    return 0;
}
