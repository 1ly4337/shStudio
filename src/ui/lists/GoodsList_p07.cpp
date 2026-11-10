#include "GoodsList.h"
#include "ItemView.h"
#include "Data_Items.h"
#include "Data_Item.h"
#include "GoodsEditor.h"
#include "itemfields_g4.h"
#include "_imports_p07.h"

bool GoodsList::onCreate()
{
    if (!ItemList::onCreate())
        return false;
    addColumn(L"Type", 50, 18, 0x74797065 );
    addColumn(L"TypeID", 50, 18, 0x74706964 );
    addColumn(L"Name", 150, 4096, 0x6E616D65 );
    addColumn(L"Icon", 45, 18, 0x69636F6E );
    addColumn(L"Quest", 45, 8194, 0x736C6F74 );
    addColumn(L"Faction", 75, 8194, 0x636E7472 );
    addColumn(L"Level", 45, 18, 0x6C766C20 );
    addColumn(L"Skill", 45, 18, 0x72616E67 );
    addColumn(L"Effect", 45, 18, 0x73706563 );
    m_baseColumns = 9;
    return true;
}

void GoodsList::loadItems(Data::Items* items)
{
    int count = items->size();
    for (int i = 0; i < count; ++i)
    {
        Data::Item* item = items->m_items[i];
        unsigned char type = G4_ITEM_TYPE(item);
        if (!m_showEmpty && G4_ITEM_NAME_LEN(item) && *G4_ITEM_NAME_BUF(item) == L'?')
            continue;

        auto contains = [type](const primitiveVector<unsigned char>& v) {
            for (int k = 0; k < v.m_count; ++k)
                if (v.m_items[k] == type)
                    return true;
            return false;
        };
        int sub = m_subFilter;
        if ((sub == 1010 && (contains(m_types2) || contains(m_types)))
            || (sub == 1011 && contains(m_types2))
            || (sub == 1012 && contains(m_types)))
            m_set.add(item);
    }
}

void GoodsList::editItem(Node* node)
{
    GoodsEditor editor(m_owner, m_items, reinterpret_cast<Data::Item*>(node));
    if (editor.createEditor() && editor.doModal() == 1)
        m_owner->setModified(true);
}

const wchar_t* GoodsList::getItemText(Node* node, int columnId)
{
    if (columnId != 0x736C6F74 )
        return ItemList::getItemText(node, columnId);
    Data::Item* item = dynamic_cast<Data::Item*>(node);
    if (!item)
        return L"no";
    const wchar_t* result = L"yes";
    if (G4_ITEM_SLOT(item) == 0)
        return L"no";
    return result;
}
