#include "PetsList.h"
#include "PetEditor.h"
#include "ItemView.h"
#include "Data_Item.h"
#include "Data_Items.h"
#include "Preferences.h"
#include "_imports_A.h"

PetsList::PetsList(ItemView* view, Data::Items* items)
    : ItemList(view, items)
{
    m_typeFilter = 1000;
    m_subFilter = 1010;
}

int PetsList::getHeaderMenu()
{
    return 196;
}

void PetsList::editItem(Node* node)
{
    PetEditor editor(m_owner, m_items, static_cast<Data::Item*>(node));
    if (editor.createEditor())
    {
        if (editor.doModal() == 1)
            m_owner->setModified(true);
    }
}
