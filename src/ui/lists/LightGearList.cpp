#include "LightGearList.h"
#include "ItemView.h"
#include "Data_Item.h"
#include "Data_Items.h"
#include "Preferences.h"
#include "_imports_A.h"

LightGearList::LightGearList(ItemView* view, Data::Items* items)
    : GearList(view, items)
{
    m_typeFilter = 1000;
    m_subFilter = 1010;
    m_faction = 0;
}

int LightGearList::getHeaderMenu()
{
    return 180;
}
