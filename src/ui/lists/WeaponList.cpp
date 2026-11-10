#include "WeaponList.h"
#include "WeaponTypes_g5.h"
#include "ClientData.h"
#include "_imports_p07.h"
#include "StudioFrame.h"

#pragma warning(disable : 4066 4498)

bool WeaponList::onCreate()
{
    if (!ItemList::onCreate())
        return false;
    this->ListCtrl::setSmallImages(reinterpret_cast<ClientData*>(reinterpret_cast<BYTE*>(g_studioFrame) + 0x230)
                         ->getImageList());
    addColumn(L"Type", 45, 18, 'type');
    addColumn(L"TypeID", 50, 18, 'tpid');
    addColumn(L"Name", 150, 4096, 'name');
    addColumn(L"Model", 45, 18, 'mdel');
    addColumn(L"Icon", 45, 18, 'icon');
    addColumn(L"Level", 45, 18, 'lvl ');
    addColumn(L"Faction", 75, 8194, 'cntr');
    addColumn(L"Slots", 40, 18, 'slot');
    m_baseColumns = 8;
    return true;
}

void WeaponList::fillTypeCombo(ComboBox* combo)
{
    int count = m_types.m_count;
    for (int i = 0; i < count; ++i)
    {
        unsigned char type;
        if (i < 0 || i >= m_types.m_capacity)
            type = 0;
        else
            type = m_types.m_items[i];
        const wchar_t* name = weaponTypeName(type);
        if (name)
            addTypeItem(combo, type, name);
    }
}
