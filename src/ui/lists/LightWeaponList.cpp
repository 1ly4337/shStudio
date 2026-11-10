#include "LightWeaponList.h"
#include "ItemView.h"
#include "Data_Item.h"
#include "Data_Items.h"
#include "Preferences.h"
#include "_imports_A.h"

LightWeaponList::LightWeaponList(ItemView* view, Data::Items* items)
    : WeaponList(view, items)
{
    m_types.add(1);
    m_types.add(2);
    m_types.add(5);
    m_types.add(6);
    m_types.add(7);
    m_types.add(8);
    m_types.add(9);
    m_types.add(10);
    m_types.add(12);
    m_types.add(13);
    m_types.add(14);
    m_types.add(15);
    m_types.add(19);
    m_types.add(45);
    m_types.add(46);
    m_types.add(49);
    m_types.add(51);
    m_types.add(53);
    m_types.add(55);
    m_types.add(57);
    m_types.add(58);
    m_types.add(60);
    m_types.add(62);
    m_types.add(64);
    m_types.add(65);
    m_types.add(69);
    m_types.add(75);
}

int LightWeaponList::getHeaderMenu()
{
    return 182;
}
