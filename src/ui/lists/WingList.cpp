#include "WingList.h"
#include "WingEditor.h"
#include "ItemView.h"
#include "Data_Item.h"
#include "Data_Items.h"
#include "Preferences.h"
#include "_imports_A.h"

WingList::WingList(ItemView* view, Data::Items* items)
    : ItemList(view, items)
{
    m_typeFilter = 1000;
    m_subFilter = 1010;
}

int WingList::getHeaderMenu()
{
    return 197;
}

void WingList::editItem(Node* node)
{
    WingEditor editor(m_owner, m_items, static_cast<Data::Item*>(node));
    if (editor.createEditor())
    {
        if (editor.doModal() == 1)
            m_owner->setModified(true);
    }
}
