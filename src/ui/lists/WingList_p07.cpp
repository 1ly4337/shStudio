#include "WingList.h"
#include "Data_Items.h"
#include "ClientData.h"
#include "_imports_p07.h"
#include "StudioFrame.h"

#pragma warning(disable : 4066 4498)

bool WingList::onCreate()
{
    if (!ItemList::onCreate())
        return false;
    this->ListCtrl::setSmallImages(reinterpret_cast<ClientData*>(reinterpret_cast<BYTE*>(g_studioFrame) + 0x230)
                         ->getImageList());
    addColumn(L"Type", 55, 18, 'type');
    addColumn(L"TypeID", 50, 18, 'tpid');
    addColumn(L"Name", 150, 4096, 'name');
    addColumn(L"Model", 50, 18, 'mdel');
    addColumn(L"Icon", 45, 18, 'icon');
    addColumn(L"Level", 45, 18, 'lvl ');
    addColumn(L"Faction", 75, 8194, 'cntr');
    m_baseColumns = 7;
    return true;
}

void WingList::loadItems(Data::Items* items)
{
    int count = items->size();
    for (int i = 0; i < count; ++i)
    {
        Data::Item* item = items->m_items[i];
        if (item && FIELD(BYTE, item, 0x54) == 121
            && (m_showEmpty || FIELD(BYTE, item, 0x56)))
        {
            m_set.add(items->m_items[i]);
        }
    }
}
