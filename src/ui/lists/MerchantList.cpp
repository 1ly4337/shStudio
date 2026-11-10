#include "MerchantList.h"
#include "NpcData.h"
#include "StandardNPC.h"
#include "Merchant.h"
#include "DocView.h"
#include "ida_compat.h"
#include "_imports_E.h"
#include "Collection.hpp"

MerchantList::MerchantList(DocView* view, NpcData* data)
    : NpcList(view, data)
{
    m_list = &data->m_npcs[1];
}

Node* MerchantList::getNode(int index)
{
    return m_list->get(index);
}

bool MerchantList::sort()
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

bool MerchantList::clearItem(int index)
{
    int item = index;
    if (index == -2)
        item = getNextSelected(0);
    if (item == -1)
        return false;
    Merchant* merchant = dynamic_cast<Merchant*>(getNode(item));
    if (!merchant)
        return false;
    merchant->reset();
    SendMessageW(m_hWnd, LVM_REDRAWITEMS, item, item);
    m_view->setModified(true);
    return true;
}

int MerchantList::indexOfNode(Node* node)
{
    return m_list->indexOf(dynamic_cast<Merchant*>(node));
}

Node* MerchantList::findNode(Node* like)
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
