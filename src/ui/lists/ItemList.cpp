#include "ItemList.h"
#include "ItemView.h"
#include "GetItemClassEditor.h"
#include "Data_Item.h"
#include "Data_Items.h"
#include "Data_Item_fields_B.h"
#include "String.hpp"
#include "_imports_A.h"
#include <cstring>
#include "AlertDial.h"
#include "Collection.hpp"

static const wchar_t* g_itemClassNames[12] = {
    L"Fighter", L"Defenser", L"Ranger", L"Archer", L"Mage", L"Priest",
    L"Warrior", L"Guardian", L"Assassin", L"Hunter", L"Pagan", L"Oracle",
};

static BOOL setMenuItemChecked(HMENU menu, UINT id, bool checked)
{
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, 0x2C);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    BOOL result = GetMenuItemInfoW(menu, id, FALSE, &mii);
    if ((mii.fState & (MFS_GRAYED | MFS_DISABLED)) == 0)
    {
        mii.fState = checked ? MFS_CHECKED : 0;
        return SetMenuItemInfoW(menu, id, FALSE, &mii);
    }
    return result;
}

ItemList::ItemList(ItemView* view, Data::Items* items)
    : m_owner(view), m_items(items)
{
    m_dragMode = 2;
    m_view = 1;
    m_needsFill = true;
    m_typeFilter = 1000;
    m_subFilter = 1010;
    m_columnGroups = 0;
    m_allColumns = false;
    m_showEmpty = false;
    m_baseColumns = 0;
    m_faction = 0;
    m_sortColumn = 'type';
    m_prevSortColumn = 'tpid';
}

ItemList::~ItemList()
{
}

BOOL ItemList::show(bool on)
{
    if (on && m_needsFill)
    {
        setItemCount(0);
        ptrArray_detachAll(m_set);
        loadItems(m_items);
        setItemCount(m_set.size());
        m_needsFill = false;
    }
    return ShowWindow(m_hWnd, on ? SW_SHOW : SW_HIDE);
}

DWORD ItemList::getStyle()
{
    return 0x10001009;
}

bool ItemList::onCommand(int code, LPARAM ctrl, int id)
{
    if (code || ctrl || (unsigned)(id - 1000) > 70)
        return ListCtrlOD::onCommand(code, ctrl, id);
    onFilterCommand(m_items, id);
    return true;
}

bool ItemList::onInitMenuPopup(HMENU menu)
{
    ListCtrlOD::onInitMenuPopup(menu);
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, 0x2C);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    mii.fState = MFS_GRAYED | MFS_DISABLED;
    SetMenuItemInfoW(menu, 0xE122, FALSE, &mii);
    return true;
}

void ItemList::onChanged()
{
    m_owner->setModified(true);
}

int ItemList::insertItem(Node* node, int index)
{
    Data::Item* item = dynamic_cast<Data::Item*>(node);
    if (!item)
        return -1;
    if (!DI_Type(item))
    {
        GetItemClassEditor editor(this, m_faction);
        if (!editor.create(m_hWnd) || editor.doModal() != 1)
            return -1;
        int typeId = 1;
        while (m_items->Data::Items::sub_42FC00(editor.m_type, typeId))
        {
            if (++typeId > 255)
                return -1;
        }
        if (typeId < 0)
            return -1;
        DI_Type(item) = (unsigned char)editor.m_type;
        DI_TypeId(item) = (unsigned char)typeId;
        int faction = m_faction;
        if (faction >= 0)
        {
            DI_Country(item) = faction ? 5 : 2;
            DI_Class(item, 0) = editor.m_classChecked[0] != 0;
            DI_Class(item, 1) = editor.m_classChecked[1] != 0;
            DI_Class(item, 2) = editor.m_classChecked[2] != 0;
            DI_Class(item, 3) = editor.m_classChecked[3] != 0;
            DI_Class(item, 4) = editor.m_classChecked[4] != 0;
            DI_Class(item, 5) = editor.m_classChecked[5] != 0;
        }
        else
        {
            DI_Country(item) = (unsigned char)editor.m_usedByIndex;
        }
    }
    if (!static_cast<ptrArray<Data::Item*>*>(m_items)->add(item))
        return -1;
    m_set.insert(item, index);
    int count = m_set.m_count;
    if (count >= 2)
    {
        m_set.m_changes = 0;
        quickSort(m_set, 0, count - 1, 'type', 'tpid', true);
    }
    return m_set.indexOf(item);
}

Node* ItemList::getNode(int index)
{
    return m_set.get(index);
}

const wchar_t* ItemList::getItemText(Node* node, int columnId)
{
    Data::Item* item = dynamic_cast<Data::Item*>(node);
    if (!item || columnId != 'cntr')
        return node->getString(columnId);
    switch (DI_Country(item))
    {
    case 0: return L"Human";
    case 1: return L"Elf";
    case 2: return L"All lights";
    case 3: return L"Deatheater";
    case 4: return L"Vail";
    case 5: return L"All furies";
    case 6: return L"All factions";
    default: return L"invalid";
    }
}

int ItemList::listV69()
{
    return 'name';
}

void ItemList::onContextMenu(HMENU menu)
{
    setMenuItemChecked(menu, m_typeFilter, true);
    setMenuItemChecked(menu, m_subFilter, true);
    setMenuItemChecked(menu, 1040, m_showEmpty);
    setMenuItemChecked(menu, 1041, !m_showEmpty);
    setMenuItemChecked(menu, 1050, m_allColumns);
    setMenuItemChecked(menu, 1051, (m_columnGroups & 1) != 0);
    setMenuItemChecked(menu, 1052, ((m_columnGroups >> 1) & 1) != 0);
    setMenuItemChecked(menu, 1053, ((m_columnGroups >> 2) & 1) != 0);
    setMenuItemChecked(menu, 1054, ((m_columnGroups >> 3) & 1) != 0);
    setMenuItemChecked(menu, 1055, ((m_columnGroups >> 4) & 1) != 0);
    setMenuItemChecked(menu, 1056, (m_columnGroups & 0x20) != 0);
}

DWORD ItemList::getExStyle()
{
    return 0x30;
}

bool ItemList::sort()
{
    int key1 = m_sortColumn;
    int key2 = m_prevSortColumn;
    bool ascending = m_sortAscending;
    int count = m_set.m_count;
    if (count < 2)
        return false;
    m_set.m_changes = 0;
    quickSort(m_set, 0, count - 1, key1, key2, ascending);
    return m_set.m_changes > 0;
}

bool ItemList::onActivateItem(int index, int keys)
{
    if (m_set.m_items[index])
        editItem(m_set.m_items[index]);
    return false;
}

void ItemList::onItemChanged(int index, UINT newState)
{
    bool selected = index != -1;
    m_owner->enableMenuItem(0xE12B, selected);
    m_owner->enableMenuItem(0xE12C, selected);
}

bool ItemList::removeItem(int index)
{
    Data::Item* item = m_set.get(index);
    ptrArray<Data::Item*>* all = m_items;
    if (!all->detachItem(item))
        return false;
    all->destroyItem(item);
    m_set.detachItem(item);
    return true;
}

Node* ItemList::createNode()
{
    return new Data::Item(m_items->size() + 1, m_items->m_value30);
}

bool ItemList::clearItem(int index)
{
    Data::Item* item = dynamic_cast<Data::Item*>(getNode(index));
    if (!item)
        return false;
    item->reset();
    if (!m_showEmpty && m_set.detachItem(item))
        setItemCount(getCount(0) - 1);
    return true;
}

int ItemList::indexOfNode(Node* node)
{
    return m_set.indexOf(dynamic_cast<Data::Item*>(node));
}

Node* ItemList::findNode(Node* like)
{
    Data::Item* item = dynamic_cast<Data::Item*>(like);
    if (!item)
        return 0;
    Data::Item* found = m_items->Data::Items::sub_42FC00(DI_Type(item), DI_TypeId(item));
    if (!found)
        return 0;
    if (!m_set.contains(found))
    {
        m_set.add(found);
        setItemCount(getCount(0) + 1);
        sort();
    }
    return found;
}

UINT ItemList::getClipboardFormat()
{
    return 514;
}

bool ItemList::confirm(const wchar_t* verb, int count)
{
    String text;
    text.append(L"For most of item's types, items SHALL NOT be deleted\n"
                L"(most of banks SHALL contain 255 items).\n"
                L"Are you sure you want ", (unsigned)-1);
    text << verb;
    if (count == 1)
        text << L" the selected item?";
    else
        text << L" these " << (unsigned long)count << L" items?";
    return AlertDial::alert(m_hWnd, text.m_buffer, 0, MB_YESNO | MB_ICONQUESTION) == IDYES;
}

Data::Item* ItemList::getItem(int index)
{
    return m_set.get(index);
}

void ItemList::exportText(String& out, DWORD fmt)
{
    int count = m_set.size();
    for (int i = 0; i < count; ++i)
    {
        Data::Item* item = m_set.get(i);
        if (item)
        {
            item->toText(out, fmt);
            out.newline();
        }
    }
}

void ItemList::onFilterCommand(Data::Items* items, int id)
{
    if (id == 1050)
    {
        toggleAllColumns();
        return;
    }
    if ((unsigned)(id - 1051) <= 18)
    {
        m_columnGroups ^= 1u << (id - 1051);
        rebuildColumns();
        return;
    }
    if (id == 1040)
        m_showEmpty = true;
    else if (id == 1041)
        m_showEmpty = false;
    else if (id >= 1000)
    {
        if (id >= 1010)
        {
            if (id < 1040)
                m_subFilter = id;
        }
        else
        {
            m_typeFilter = id;
        }
    }
    for (int i = 0; i < m_set.m_count; ++i)
        m_set.m_items[i] = 0;
    m_set.m_count = 0;
    setItemCount(0);
    loadItems(items);
    setItemCount(m_set.size());
}

void ItemList::removeExtraColumns()
{
    for (int count = m_columnCount; count > m_baseColumns; count = m_columnCount)
    {
        if (!deleteColumn(count - 1))
            break;
    }
}

void ItemList::rebuildColumns()
{
    removeExtraColumns();
    if (m_columnGroups & 1)
        addClassColumns();
    if (m_columnGroups & 2)
        addAttackColumns();
    if (m_columnGroups & 4)
        addStatColumns();
    if (m_columnGroups & 8)
        addRequirementColumns();
    if (m_columnGroups & 0x10)
        addMiscColumns();
    if (m_columnGroups & 0x20)
        addPriceColumns();
}

void ItemList::toggleAllColumns()
{
    removeExtraColumns();
    m_allColumns = !m_allColumns;
    if (m_allColumns)
    {
        m_columnGroups = 63;
        addClassColumns();
        addColumn(g_itemColumnNames[14], 50, 18, g_itemColumnTags[14]);
        addRequirementColumns();
        addStatColumns();
        addAttackColumns();
        addMiscColumns();
        addPriceColumns();
    }
    else
    {
        m_columnGroups = 0;
    }
}

void ItemList::addClassColumns()
{
    int faction = m_faction;
    if (faction < 0)
        faction = 0;
    const wchar_t** names = &g_itemClassNames[6 * faction];
    for (int i = 0; i < 6; ++i)
        addColumn(names[i], 50, 18, g_itemColumnTags[8 + i]);
}

void ItemList::addAttackColumns()
{
    for (int i = 0; i < 4; ++i)
        addColumn(g_itemColumnNames[32 + i], 50, 18, g_itemColumnTags[32 + i]);
}

void ItemList::addStatColumns()
{
    for (int i = 0; i < 9; ++i)
        addColumn(g_itemColumnNames[36 + i], 50, 18, g_itemColumnTags[36 + i]);
}

void ItemList::addRequirementColumns()
{
    for (int i = 0; i < 9; ++i)
        addColumn(g_itemColumnNames[17 + i], 50, 18, g_itemColumnTags[17 + i]);
}

void ItemList::addMiscColumns()
{
    for (int i = 26; i < 32; ++i)
    {
        if (i != 30)
            addColumn(g_itemColumnNames[i], 50, 18, g_itemColumnTags[i]);
    }
    for (int j = 0; j < 2; ++j)
        addColumn(g_itemColumnNames[45 + j], 50, 18, g_itemColumnTags[45 + j]);
}

void ItemList::addPriceColumns()
{
    for (int i = 0; i < 6; ++i)
        addColumn(g_itemColumnNames[47 + i], 50, 18, g_itemColumnTags[47 + i]);
}
