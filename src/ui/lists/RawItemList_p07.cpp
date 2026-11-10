#include "RawItemList.h"
#include "ItemView.h"
#include "Data_Items.h"
#include "ItemEditor.h"
#include "ctlhelpers_g4.h"
#include "_imports_p07.h"

bool RawItemList::onCreate()
{
    if (!ItemList::onCreate())
        return false;
    setSmallImages(g4_clientData()->getImageList());
    addColumn(L"Type", 45, 18, 0x74797065 );
    addColumn(L"TypeID", 45, 18, 0x74706964 );
    addColumn(L"Name", 150, 4096, 0x6E616D65 );
    for (int i = 0; g_itemColumnTags[4 + i]; ++i)
        addColumn(g_itemColumnNames[4 + i], 50, 18, g_itemColumnTags[4 + i]);
    if ((m_items->m_value30 & 0x1000) != 0)
    {
        for (int i = 0; g_itemColumnTagsEp6[i]; ++i)
            addColumn(g_itemColumnNamesEp6[i], 50, 18, g_itemColumnTagsEp6[i]);
    }
    return true;
}

void RawItemList::editItem(Node* node)
{
    if (openItemEditor(m_owner, m_items, reinterpret_cast<Data::Item*>(node)))
        m_owner->setModified(true);
}

void RawItemList::loadItems(Data::Items* )
{
    int count = m_items->size();
    for (int i = 0; i < count; ++i)
        m_set.add(m_items->get(i));
    setItemCount(m_items->size());
}
