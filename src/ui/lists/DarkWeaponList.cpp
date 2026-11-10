#include "DarkWeaponList.h"
#include "ItemView.h"
#include "Data_Item.h"
#include "Data_Items.h"
#include "Preferences.h"
#include "_imports_A.h"

DarkWeaponList::DarkWeaponList(ItemView* view, Data::Items* items)
    : WeaponList(view, items)
{
    m_types.add(3);
    m_types.add(4);
    m_types.add(5);
    m_types.add(6);
    m_types.add(7);
    m_types.add(8);
    m_types.add(9);
    m_types.add(10);
    m_types.add(11);
    m_types.add(12);
    m_types.add(13);
    m_types.add(15);
    m_types.add(34);
    m_types.add(47);
    m_types.add(48);
    m_types.add(50);
    m_types.add(52);
    m_types.add(54);
    m_types.add(56);
    m_types.add(57);
    m_types.add(58);
    m_types.add(59);
    m_types.add(61);
    m_types.add(63);
    m_types.add(65);
    m_types.add(84);
    m_types.add(90);
}

int DarkWeaponList::getHeaderMenu()
{
    return 183;
}
