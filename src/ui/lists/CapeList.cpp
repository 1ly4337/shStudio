#include "CapeList.h"
#include "CapeEditor.h"
#include "ItemView.h"
#include "Data_Item.h"
#include "Data_Items.h"
#include "Preferences.h"
#include "_imports_A.h"

CapeList::CapeList(ItemView* view, Data::Items* items)
    : ItemList(view, items)
{
    m_typeFilter = 1000;
    m_subFilter = 1010;
}

int CapeList::getHeaderMenu()
{
    return 186;
}

void CapeList::editItem(Node* node)
{
    CapeEditor editor(m_owner, m_items, static_cast<Data::Item*>(node));
    if (editor.createEditor())
    {
        if (g_preferences.getInt(L"3D View", L"show mantle", 1) == 1)
            editor.CapeEditor::onCommand(0, 0, 200);
        if (editor.doModal() == 1)
            m_owner->setModified(true);
    }
}
