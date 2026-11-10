#include "EffectList.h"
#include "EffectView.h"
#include "EffectSets.h"
#include "EffectSet.h"
#include "Node.h"
#include <cstring>
#include "Collection.hpp"
#include "_imports_p06.h"

DWORD EffectList::getStyle()
{
    return 0xD;
}

bool EffectList::onCreate()
{
    if (!m_data || !ListCtrlOD::onCreate())
        return false;
    addColumn(L"ID", 45, 18, 'id  ');
    addColumn(L"Version", 55, 66, 'vrs3');
    return true;
}

bool EffectList::onInitMenuPopup(HMENU menu)
{
    ListCtrlOD::onInitMenuPopup(menu);
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, sizeof(mii) - sizeof(mii.cbSize));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    mii.fState = MFS_DISABLED;
    SetMenuItemInfoW(menu, 0xE12B, FALSE, &mii);
    return true;
}

void EffectList::onChanged()
{
    m_view->setModified(1);
}

int EffectList::insertItem(Node* node, int index)
{
    return m_data->insert(dynamic_cast<EffectSet*>(node), index);
}

Node* EffectList::getNode(int index)
{
    return m_data->m_items[index];
}

DWORD EffectList::getExStyle()
{
    return 0x21;
}

bool EffectList::sort()
{
    EffectSets* data = m_data;
    int prevColumn = m_prevSortColumn;
    bool ascending = m_sortAscending;
    int count = data->m_count;
    int column = m_sortColumn;
    if (count < 2)
        return false;
    data->m_changes = 0;
    quickSort(*data, 0, count - 1, column, prevColumn, ascending);
    return data->m_changes > 0;
}

void EffectList::onItemChanged(int index, UINT newState)
{
    bool selected = index != -1;
    m_view->enableMenuItem(0xE123, selected);
    m_view->enableMenuItem(0xE122, selected);
    m_view->enableMenuItem(0xE124, IsClipboardFormatAvailable(getClipboardFormat()) == 1);
    m_view->enableMenuItem(0xE12B, selected);
    m_view->enableMenuItem(0xE12C, selected);
}

bool EffectList::removeItem(int index)
{
    return m_data->remove(index);
}

Node* EffectList::createNode()
{
    return new EffectSet();
}

bool EffectList::clearItem(int index)
{
    m_data->get(index)->reset();
    return true;
}

bool EffectList::moveItemUp(int index)
{
    if (!m_data->swap(index, index - 1))
        return false;
    EffectSet* a = m_data->get(index);
    EffectSet* b = m_data->get(index - 1);
    int id = FIELD(int, a, 0x0C);
    FIELD(int, a, 0x0C) = FIELD(int, b, 0x0C);
    FIELD(int, b, 0x0C) = id;
    m_view->setModified(1);
    return true;
}

bool EffectList::moveItemDown(int index)
{
    if (!m_data->swap(index, index + 1))
        return false;
    EffectSet* a = m_data->get(index);
    EffectSet* b = m_data->get(index + 1);
    int id = FIELD(int, a, 0x0C);
    FIELD(int, a, 0x0C) = FIELD(int, b, 0x0C);
    FIELD(int, b, 0x0C) = id;
    m_view->setModified(1);
    return true;
}

int EffectList::indexOfNode(Node* node)
{
    return m_data->indexOf(dynamic_cast<EffectSet*>(node));
}

Node* EffectList::findNode(Node* like)
{
    int id = FIELD(int, &dynamic_cast<EffectSet&>(*like), 0x0C);
    EffectSets* data = m_data;
    for (int i = 0; i < data->m_count; ++i)
    {
        if (FIELD(int, data->m_items[i], 0x0C) == id)
            return data->m_items[i];
    }
    return nullptr;
}

UINT EffectList::getClipboardFormat()
{
    return 0x203;
}
