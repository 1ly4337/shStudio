#include "CustomList.h"
#include "CustomEditor.h"
#include "ItemView.h"
#include "Data_Item.h"
#include "Data_Items.h"
#include "Preferences.h"
#include "_imports_A.h"

CustomList::CustomList(ItemView* view, Data::Items* items)
    : ItemList(view, items)
{
    m_typeFilter = 1000;
    m_subFilter = 1010;
    m_types.add(22);
    m_types.add(37);
    m_types.add(23);
    m_types.add(96);
    m_types.add(40);
    m_types.add(97);
}

CustomList::~CustomList()
{
}

int CustomList::getHeaderMenu()
{
    return 189;
}

void CustomList::editItem(Node* node)
{
    CustomEditor editor(m_owner, m_items, static_cast<Data::Item*>(node));
    if (editor.createEditor())
    {
        if (editor.doModal() == 1)
            m_owner->setModified(true);
    }
}
