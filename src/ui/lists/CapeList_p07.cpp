#include "CapeList.h"
#include "Data_Items.h"
#include "itemfields_g4.h"
#include "_imports_p07.h"
#include <cwchar>

bool CapeList::onCreate()
{
    if (!ItemList::onCreate())
        return false;
    addColumn(L"Type", 45, 18, 0x74797065 );
    addColumn(L"TypeID", 50, 18, 0x74706964 );
    addColumn(L"Name", 150, 4096, 0x6E616D65 );
    addColumn(L"Model", 45, 18, 0x6D64656C );
    addColumn(L"Icon", 45, 18, 0x69636F6E );
    addColumn(L"Level", 45, 18, 0x6C766C20 );
    addColumn(L"Faction", 75, 8194, 0x636E7472 );
    addColumn(L"Mode", 45, 18, 0x67726F77 );
    m_baseColumns = 8;
    return true;
}

void CapeList::loadItems(Data::Items* items)
{
    int count = items->size();
    for (int i = 0; i < count; ++i)
    {
        Data::Item* item = items->m_items[i];
        if (!item)
            continue;
        if (!m_showEmpty && G4_ITEM_LEVEL(item) <= 0)
            continue;
        unsigned char type = G4_ITEM_TYPE(item);
        if (type != 24 && type != 39)
            continue;
        if (!m_showEmpty && G4_ITEM_NAME_LEN(item) >= 2 && wcsncmp(G4_ITEM_NAME_BUF(item), L"??", 2) == 0)
            continue;
        if (m_typeFilter == 1000
            || (m_typeFilter == 1001 && G4_ITEM_FACTION(item) == 5)
            || (m_typeFilter == 1002 && G4_ITEM_FACTION(item) == 2))
            m_set.add(item);
    }
}

void CapeList::fillTypeCombo(ComboBox* combo)
{
    addTypeItem(combo, 24, L"Light Capes");
    addTypeItem(combo, 39, L"Dark Capes");
}
