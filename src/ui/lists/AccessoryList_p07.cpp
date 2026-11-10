#include "AccessoryList.h"
#include "Data_Items.h"
#include "ComboBox.h"
#include "ItemTypeNames_g4.h"
#include "_imports_p07.h"
#include <cwchar>

#include "itemfields_g4.h"

const wchar_t* accessoryTypeName(unsigned type)
{
    const wchar_t* result;
    switch (type)
    {
    case 0x16: result = L"Rings (first set)"; break;
    case 0x17: result = L"Amulet (first set)"; break;
    case 0x25: result = L"Rings (episo 6)"; break;
    case 0x28: result = L"Loops (first set)"; break;
    case 0x60: result = L"Amulet (episo 6)"; break;
    case 0x61: result = L"Armlet (episo 6)"; break;
    default:   result = 0; break;
    }
    return result;
}

bool AccessoryList::onCreate()
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
    addColumn(L"Drop", 45, 18, 0x67726465 );
    addColumn(L"Slots", 40, 18, 0x736C6F74 );
    m_baseColumns = 9;
    return true;
}

void AccessoryList::loadItems(Data::Items* items)
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
        if (!m_showEmpty && G4_ITEM_NAME_LEN(item) >= 2 && wcsncmp(G4_ITEM_NAME_BUF(item), L"??", 2) == 0)
            continue;

        int subFilter;
        switch (type)
        {
        case 22:
        case 37:
            subFilter = 1033;
            break;
        case 23:
        case 96:
            subFilter = 1031;
            break;
        case 40:
        case 97:
            subFilter = 1032;
            break;
        default:
            continue;
        }
        unsigned char faction = G4_ITEM_FACTION(item);
        bool factionOk = m_typeFilter == 1000
                      || (m_typeFilter == 1001 && (faction == 2 || faction == 6))
                      || (m_typeFilter == 1002 && (faction == 5 || faction == 6));
        if (!factionOk)
            continue;
        if ((m_subFilter == 1010 && m_types.contains(type)) || m_subFilter == subFilter)
            m_set.add(item);
    }
}

void AccessoryList::fillTypeCombo(ComboBox* combo)
{
    int count = m_types.m_count;
    for (int i = 0; i < count; ++i)
    {
        unsigned char type;
        if (i < 0 || i >= m_types.m_capacity)
            type = 0;
        else
            type = m_types.m_items[i];
        const wchar_t* name = accessoryTypeName(type);
        if (name)
            addTypeItem(combo, type, name);
    }
}
