#include "CustomList.h"
#include "Data_Items.h"
#include "itemfields_g4.h"
#include "ctlhelpers_g4.h"
#include "_imports_p07.h"

bool CustomList::onCreate()
{
    if (!ItemList::onCreate())
        return false;
    setSmallImages(g4_clientData()->getImageList());
    addColumn(L"Type", 55, 18, 0x74797065 );
    addColumn(L"TypeID", 50, 18, 0x74706964 );
    addColumn(L"Name", 150, 4096, 0x6E616D65 );
    addColumn(L"Model", 50, 18, 0x6D64656C );
    addColumn(L"Icon", 45, 18, 0x69636F6E );
    addColumn(L"Level", 45, 18, 0x6C766C20 );
    addColumn(L"Faction", 75, 8194, 0x636E7472 );
    m_baseColumns = 9;
    return true;
}

void CustomList::loadItems(Data::Items* items)
{
    int count = items->size();
    for (int i = 0; i < count; ++i)
    {
        Data::Item* item = items->m_items[i];
        if (item && G4_ITEM_TYPE(item) == 0x96 && (m_showEmpty || G4_ITEM_MODEL(item)))
            m_set.add(items->m_items[i]);
    }
}
