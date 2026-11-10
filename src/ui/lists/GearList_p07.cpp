#include "GearList.h"
#include "LightGearList.h"
#include "DarkGearList.h"
#include "Data_Items.h"
#include "itemfields_g4.h"
#include "ctlhelpers_g4.h"
#include "_imports_p07.h"

bool GearList::onCreate()
{
    if (!ItemList::onCreate())
        return false;
    setSmallImages(g4_clientData()->getImageList());
    m_allColumns = false;
    addColumn(L"Type", 45, 18, 0x74797065 );
    addColumn(L"TypeID", 50, 18, 0x74706964 );
    addColumn(L"Name", 200, 4096, 0x6E616D65 );
    addColumn(L"Level", 45, 18, 0x6C766C20 );
    addColumn(L"Faction", 75, 8194, 0x636E7472 );
    addColumn(L"Slots", 45, 18, 0x736C6F74 );
    addColumn(L"Model", 45, 18, 0x6D64656C );
    addColumn(L"Icon", 45, 18, 0x69636F6E );
    m_baseColumns = 8;
    return true;
}

void LightGearList::fillTypeCombo(ComboBox* combo)
{
    addTypeItem(combo, 16, L"Helmet (first set)");
    addTypeItem(combo, 17, L"Armor (first set)");
    addTypeItem(combo, 18, L"Pants (first set)");
    addTypeItem(combo, 20, L"Gantlet (first set)");
    addTypeItem(combo, 21, L"Boots (first set)");
    addTypeItem(combo, 66, L"Helmet (second set)");
    addTypeItem(combo, 67, L"Armor (second set)");
    addTypeItem(combo, 68, L"Pants (second set)");
    addTypeItem(combo, 70, L"Gantlet (second set)");
    addTypeItem(combo, 71, L"Boots (second set)");
    if ((m_items->m_value30 & 0xFFF) >= 0x600)
    {
        addTypeItem(combo, 72, L"Helmet (third set)");
        addTypeItem(combo, 73, L"Armor (third set)");
        addTypeItem(combo, 74, L"Pants (third set)");
        addTypeItem(combo, 76, L"Gantlet (third set)");
        addTypeItem(combo, 77, L"Boots (third set)");
    }
}

void LightGearList::loadItems(Data::Items* items)
{
    int count = items->size();
    for (int i = 0; i < count; ++i)
    {
        Data::Item* item = items->m_items[i];
        bool all = m_showEmpty;
        unsigned char faction;
        if (!all
            && !(G4_ITEM_FLAG_E0(item)
                 && ((faction = G4_ITEM_FACTION(item)) <= 2 || faction == 6)
                 && G4_ITEM_LEVEL(item) > 0))
            continue;
        int sub = m_subFilter;
        unsigned char type = G4_ITEM_TYPE(item);
        if ((sub == 1010 && (((unsigned char)(type - 16) <= 5 && type != 19)
                             || ((unsigned char)(type - 66) <= 5 && type != 69)
                             || ((unsigned char)(type - 72) <= 5 && type != 75)))
            || (sub == 1011 && (type == 16 || type == 72))
            || (sub == 1012 && (type == 17 || type == 67 || type == 73))
            || (sub == 1013 && (type == 18 || type == 68 || type == 74))
            || (sub == 1014 && (type == 20 || type == 70 || type == 76))
            || (sub == 1015 && (type == 21 || type == 71 || type == 77)))
        {
            if (all || m_typeFilter == 1000 || FIELD(unsigned char, item, m_typeFilter - 910))
                m_set.add(item);
        }
    }
}

void DarkGearList::fillTypeCombo(ComboBox* combo)
{
    addTypeItem(combo, 31, L"Cap (first set)");
    addTypeItem(combo, 32, L"Protector (first set)");
    addTypeItem(combo, 33, L"Leggings (first set)");
    addTypeItem(combo, 35, L"Muffler (first set)");
    addTypeItem(combo, 36, L"Walkers (first set)");
    addTypeItem(combo, 81, L"Cap (second set)");
    addTypeItem(combo, 82, L"Protector (second set)");
    addTypeItem(combo, 83, L"Leggings (second set)");
    addTypeItem(combo, 85, L"Muffler (second set)");
    addTypeItem(combo, 86, L"Walkers (second set)");
    if ((m_items->m_value30 & 0xFFF) >= 0x600)
    {
        addTypeItem(combo, 87, L"Cap (third set)");
        addTypeItem(combo, 88, L"Protector (third set)");
        addTypeItem(combo, 89, L"Leggings (third set)");
        addTypeItem(combo, 91, L"Muffler (third set)");
        addTypeItem(combo, 92, L"Walkers (third set)");
    }
}

void DarkGearList::loadItems(Data::Items* items)
{
    int count = items->size();
    for (int i = 0; i < count; ++i)
    {
        Data::Item* item = items->m_items[i];
        bool all = m_showEmpty;
        if (!all && !(G4_ITEM_FLAG_E0(item) && G4_ITEM_FACTION(item) >= 3 && G4_ITEM_LEVEL(item) > 0))
            continue;
        int sub = m_subFilter;
        unsigned char type = G4_ITEM_TYPE(item);
        if ((sub == 1010 && (((unsigned char)(type - 31) <= 5 && type != 34)
                             || ((unsigned char)(type - 81) <= 5 && type != 84)
                             || ((unsigned char)(type - 87) <= 5 && type != 90)))
            || (sub == 1011 && (type == 31 || type == 87))
            || (sub == 1012 && (type == 32 || type == 82 || type == 88))
            || (sub == 1013 && (type == 33 || type == 83 || type == 89))
            || (sub == 1014 && (type == 35 || type == 85 || type == 91))
            || (sub == 1015 && (type == 36 || type == 86 || type == 92)))
        {
            if (all || m_typeFilter == 1000 || FIELD(unsigned char, item, m_typeFilter - 910))
                m_set.add(item);
        }
    }
}
