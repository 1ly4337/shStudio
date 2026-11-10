#include "WeaponList.h"
#include "WeaponEditor.h"
#include "ItemView.h"
#include "Data_Item.h"
#include "Data_Items.h"
#include "Preferences.h"
#include "_imports_A.h"

WeaponList::WeaponList(ItemView* view, Data::Items* items)
    : ItemList(view, items)
{
    m_typeFilter = 1000;
    m_subFilter = 1010;
}

WeaponList::~WeaponList()
{
}

void WeaponList::editItem(Node* node)
{
    WeaponEditor editor(m_owner, m_items, static_cast<Data::Item*>(node));
    if (editor.createEditor())
    {
        if (g_preferences.getInt(L"3D View", L"show weapon", 1) == 1)
            editor.WeaponEditor::onCommand(0, 0, 200);
        if (editor.doModal() == 1)
            m_owner->setModified(true);
    }
}
