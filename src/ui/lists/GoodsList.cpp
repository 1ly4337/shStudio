#include "GoodsList.h"
#include "ItemView.h"
#include "Data_Item.h"
#include "Data_Items.h"
#include "Preferences.h"
#include "_imports_A.h"

GoodsList::GoodsList(ItemView* view, Data::Items* items)
    : ItemList(view, items)
{
    m_faction = -1;
    m_subFilter = 1010;
    m_types.add(25);
    m_types.add(38);
    m_types.add(41);
    m_types.add(43);
    m_types.add(44);
    m_types.add(78);
    m_types.add(79);
    m_types.add(80);
    m_types.add(94);
    m_types.add(100);
    m_types.add(101);
    m_types.add(102);
    m_types.add(103);
    m_types2.add(27);
    m_types2.add(28);
    m_types2.add(29);
    m_types2.add(99);
}

GoodsList::~GoodsList()
{
}

int GoodsList::getHeaderMenu()
{
    return 187;
}
