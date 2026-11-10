#include "ListGears.h"
#include "Data_Items.h"
#include "ItemsSet.h"
#include "primitiveVector.h"
#include "_imports_p07.h"
#include "StudioFrame.h"

#pragma warning(disable : 4066 4498)

#define GEARS_DB(l)    FIELD(Data::Items*, (l), 0xF4)
#define GEARS_ITEMS(l) FIELD(Data::ItemsSet, (l), 0xF8)

#define ITEM_TYPE(it)       FIELD(unsigned char, (it), 0x54)
#define ITEM_MODEL(it)      FIELD(unsigned char, (it), 0x56)
#define ITEM_RACE(it, slot) FIELD(unsigned char, (it), 0x5B + (slot))

bool ListGears::onCreate()
{
    bool result = ListCtrl::onCreate();
    if (result)
    {
        addColumn(L"Type", 40, 16, 'type');
        addColumn(L"TypeID", 50, 16, 'tpid');
        addColumn(L"Name", 180, 0, 'name');
        return true;
    }
    return result;
}

void ListGears::fill(const primitiveVector<unsigned char>& types, int modelIndex, unsigned int raceSlot)
{
    SendMessageW(m_hWnd, LVM_DELETEALLITEMS, 0, 0);
    Data::ItemsSet& listed = GEARS_ITEMS(this);
    for (int i = 0; i < listed.m_count; ++i)
        listed.m_items[i] = 0;
    listed.m_count = 0;

    if (types.m_count && raceSlot <= 5)
    {
        int count = GEARS_DB(this)->size();
        if (!count)
        {
            if (studioItemData() != GEARS_DB(this))
                OutputDebugStringW(L"bug\n");
            count = GEARS_DB(this)->size();
        }
        for (int i = 0; i < count; ++i)
        {
            Data::Item* item = GEARS_DB(this)->get(i);
            if (!item)
                continue;
            int t = 0;
            for (; t < types.m_count; ++t)
            {
                if (types.m_items[t] == ITEM_TYPE(item))
                    break;
            }
            if (t >= types.m_count)
                continue;
            if (ITEM_MODEL(item) != modelIndex || ITEM_RACE(item, raceSlot) != 1)
                continue;
            listed.add(item);
            insertTextItem(LPSTR_TEXTCALLBACKW, I_IMAGECALLBACK, -1);
        }
    }
}
