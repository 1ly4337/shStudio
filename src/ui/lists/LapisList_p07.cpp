#include "LapisList.h"
#include "Data_Items.h"
#include "Data_Item.h"
#include "ItemView.h"
#include "LapisEditor.h"
#include "LapisiaEditor.h"
#include "ClientData.h"
#include "_imports_p07.h"
#include "StudioFrame.h"

#pragma warning(disable : 4066 4498)

bool LapisList::onCreate()
{
    if (!ItemList::onCreate())
        return false;
    ImageStrip* icons = reinterpret_cast<ClientData*>(reinterpret_cast<BYTE*>(g_studioFrame) + 0x230)
                            ->getIcons(L"icon_rapis");
    if (icons)
        setImages(icons);
    addColumn(L"Type", 40, 16, 'type');
    addColumn(L"TypeID", 45, 16, 'tpid');
    addColumn(L"Name", 150, 4096, 'name');
    addColumn(L"Icon", 45, 18, 'icon');
    addColumn(L"Level", 45, 18, 'rInt');
    m_baseColumns = 5;
    addStatColumns();
    m_columnGroups |= 4;
    return true;
}

void LapisList::addClassColumns()
{
    addColumn(L"Helmet", 50, 18, 'cntr');
    addColumn(L"Armor", 50, 18, 'cla1');
    addColumn(L"Pants", 50, 18, 'cla2');
    addColumn(L"Gantlet", 50, 18, 'cla4');
    addColumn(L"Shoes", 50, 18, 'cla5');
    addColumn(L"Shield", 50, 18, 'cla3');
    addColumn(L"Weapon", 50, 18, 'lvl ');
    addColumn(L"Necklace", 50, 18, 'grow');
    addColumn(L"Loop", 50, 18, 'rStr');
    addColumn(L"Ring", 50, 18, 'cla6');
}

void LapisList::loadItems(Data::Items* items)
{
    int count = items->size();
    for (int i = 0; i < count; ++i)
    {
        Data::Item* item = items->m_items[i];
        BYTE type = FIELD(BYTE, item, 0x54);
        if (type == 30 || type == 98)
        {
            int filter = m_subFilter;
            if (filter == 1010
                || (filter == 1011 && FIELD(BYTE, item, 0x5A))
                || (filter == 1012 && FIELD(BYTE, item, 0x5B))
                || (filter == 1013 && FIELD(BYTE, item, 0x5C))
                || (filter == 1014 && FIELD(BYTE, item, 0x5E))
                || (filter == 1015 && FIELD(BYTE, item, 0x5F))
                || (filter == 1024 && FIELD(BYTE, item, 0x5D))
                || (filter == 1030 && FIELD(WORD, item, 0x58))
                || (filter == 1031 && FIELD(BYTE, item, 0x61))
                || (filter == 1032 && FIELD(WORD, item, 0x64))
                || (filter == 1033 && FIELD(BYTE, item, 0x60)))
            {
                m_set.add(items->m_items[i]);
            }
        }
        else if (type == 95)
        {
            int filter = m_subFilter;
            if (filter == 1010
                || (filter == 1011 && FIELD(BYTE, item, 0x5A))
                || (filter == 1012 && FIELD(BYTE, item, 0x5B))
                || (filter == 1013 && FIELD(BYTE, item, 0x5C))
                || (filter == 1014 && FIELD(BYTE, item, 0x5E))
                || (filter == 1015 && FIELD(BYTE, item, 0x5F))
                || (filter == 1024 && FIELD(BYTE, item, 0x5D))
                || (filter == 1030 && FIELD(WORD, item, 0x58)))
            {
                m_set.add(items->m_items[i]);
            }
        }
    }
}

void LapisList::fillTypeCombo(ComboBox* combo)
{
    addTypeItem(combo, 30, L"Lapis");
    addTypeItem(combo, 98, L"Lapis 2nd set");
    addTypeItem(combo, 95, L"Lapisia");
}

void LapisList::editItem(Node* node)
{
    Data::Item* item = static_cast<Data::Item*>(node);
    ItemEditor* editor;
    if (FIELD(BYTE, item, 0x54) != 95)
        editor = new LapisEditor(m_owner, m_items, item);
    else
        editor = new LapisiaEditor(m_owner, m_items, item);
    if (editor->createEditor() && editor->doModal() == 1)
        m_owner->setModified(true);
    delete editor;
}

const wchar_t* LapisList::getLapisText(Node* node, int columnId)
{
    if (!node)
        return 0;
    if (columnId == 'rIg ')
    {
        switch (node->getInt('rIg '))
        {
        case 0:  return L"unset";
        case 30: return L"50 %";
        case 31: return L"46 %";
        case 32: return L"40 %";
        case 33: return L"32 %";
        case 34: return L"24 %";
        case 35: return L"16 %";
        case 36: return L"8 %";
        case 37: return L"2 %";
        case 38: return L"~1 %";
        case 39: return L"1 %";
        default: return L"unk";
        }
    }
    if (columnId == 'rVg ')
    {
        if (node->getInt('rVg ') != 1)
            return L"no";
        return L"yes";
    }
    return node->getString(columnId);
}
