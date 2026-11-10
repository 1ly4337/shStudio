#include "MountList.h"
#include "MountEditor.h"
#include "ItemView.h"
#include "Data_Item.h"
#include "Data_Items.h"
#include "Preferences.h"
#include "_imports_A.h"

MountList::MountList(ItemView* view, Data::Items* items)
    : ItemList(view, items)
{
    m_faction = -1;
}

void MountList::fillTypeCombo(ComboBox* combo)
{
    addTypeItem(combo, 42, L"Mounts");
}

void MountList::editItem(Node* node)
{
    MountEditor editor(m_owner, m_items, static_cast<Data::Item*>(node));
    if (editor.createEditor())
    {
        if (editor.doModal() == 1)
            m_owner->setModified(true);
    }
}
