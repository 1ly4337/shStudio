#include "LightWeaponList.h"
#include "Data_Items.h"
#include "_imports_p07.h"

void LightWeaponList::loadItems(Data::Items* items)
{
    int count = items->size();
    for (int i = 0; i < count; ++i)
    {
        Data::Item* item = items->m_items[i];
        bool showAll = m_showEmpty;
        if (!showAll)
        {
            BYTE faction = FIELD(BYTE, item, 0x5A);
            if (faction != 2 && faction != 6)
                continue;
            if (FIELD(short, item, 0x58) <= 0)
                continue;
        }
        int cat = m_subFilter;
        BYTE type = FIELD(BYTE, item, 0x54);
        bool match = false;
        if (cat == 1010)
        {
            int n = m_types.m_count;
            BYTE* types = m_types.m_items;
            for (int k = 0; k < n; ++k)
            {
                if (types[k] == type)
                {
                    match = true;
                    break;
                }
            }
        }
        if (!match)
        {
            match = (cat == 1011 && (type == 1 || type == 45))
                 || (cat == 1012 && (type == 2 || type == 46))
                 || (cat == 1013 && (type == 5 || type == 49))
                 || (cat == 1014 && (type == 6 || type == 51))
                 || (cat == 1015 && (type == 7 || type == 53))
                 || (cat == 1016 && (type == 8 || type == 55))
                 || (cat == 1017 && (type == 9 || type == 57))
                 || (cat == 1018 && (type == 10 || type == 58))
                 || (cat == 1022 && (type == 12 || type == 60))
                 || (cat == 1019 && (type == 13 || type == 62))
                 || (cat == 1020 && (type == 14 || type == 64))
                 || (cat == 1023 && (type == 15 || type == 65))
                 || (cat == 1024 && (type == 19 || type == 69 || type == 75));
        }
        if (!match)
            continue;
        int typeFilter = m_typeFilter;
        if (showAll || typeFilter == 1000 || FIELD(BYTE, item, typeFilter - 910))
            m_set.add(item);
    }
}
