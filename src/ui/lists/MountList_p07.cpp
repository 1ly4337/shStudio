#include "MountList.h"
#include "Data_Items.h"
#include "_imports_p07.h"

#pragma warning(disable : 4066 4498)

bool MountList::onCreate()
{
    if (!ItemList::onCreate())
        return false;
    addColumn(L"TypeID", 50, 16, 'tpid');
    addColumn(L"Name", 200, 4096, 'name');
    addColumn(L"Faction", 75, 8194, 'cntr');
    addColumn(L"Icon", 45, 18, 'icon');
    addColumn(L"Model", 45, 18, 'mdel');
    addColumn(L"Level", 45, 18, 'lvl ');
    addColumn(L"Range", 45, 18, 'rang');
    addColumn(L"Group", 45, 18, 'rIg ');
    m_baseColumns = 8;
    return true;
}

void MountList::loadItems(Data::Items* items)
{
    int count = items->size();
    for (int i = 0; i < count; ++i)
    {
        Data::Item* item = items->m_items[i];
        if (FIELD(BYTE, item, 0x54) == 42)
            m_set.add(item);
    }
    setItemCount(m_set.size());
}
