#include "NpcList.h"
#include "NpcData.h"
#include "StandardNPC.h"
#include "DocView.h"
#include "ida_compat.h"
#include "_imports_E.h"
#include "Collection.hpp"

NpcList::NpcList(DocView* view, NpcData* data)
    : m_view(view), m_data(data)
{
}

NpcList::~NpcList()
{
    for (int i = 0; i < m_npcs.m_count; ++i)
        m_npcs.m_items[i] = 0;
    m_npcs.m_count = 0;
}

static void setMenuItemState(HMENU menu, UINT id, UINT state)
{
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, 0x2C);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    mii.fState = state;
    SetMenuItemInfoW(menu, id, FALSE, &mii);
}

bool NpcList::onInitMenuPopup(HMENU menu)
{
    ListCtrlOD::onInitMenuPopup(menu);
    setMenuItemState(menu, 0xE122, MFS_DISABLED);
    setMenuItemState(menu, 0xE12B, MFS_DISABLED);
    setMenuItemState(menu, 0xE12A, 0);
    return true;
}

void NpcList::onChanged()
{
    m_view->setModified(true);
}

int NpcList::insertItem(Node* node, int index)
{
    StandardNPC* npc = dynamic_cast<StandardNPC*>(node);
    if (!npc)
        return -1;
    int type = FIELD(int, npc, 0x0C);
    if (type < 3 || type >= 14)
        return -1;
    StandardNPCs* list = npcTypeList(m_data, (unsigned char)type);
    if (!list)
        return -1;
    if (!FIELD(short, npc, 0x10))
        FIELD(short, npc, 0x10) = (short)(list->size() + 1);
    if (!list->add(npc))
        return -1;
    m_npcs.insert(npc, -1);
    m_npcs.sort('type', 'uid ', true);
    return m_npcs.indexOf(npc);
}

Node* NpcList::getNode(int index)
{
    return m_npcs.get(index);
}

bool NpcList::sort()
{
    int column2 = m_prevSortColumn;
    bool ascending = m_sortAscending;
    int count = m_npcs.m_count;
    int column = m_sortColumn;
    if (count < 2)
        return false;
    m_npcs.m_changes = 0;
    quickSort(m_npcs, 0, count - 1, column, column2, ascending);
    return m_npcs.m_changes > 0;
}

void NpcList::onItemChanged(int index, UINT newState)
{
    bool enable;
    if (index == -1 || (enable = true, !newState))
        enable = false;
    m_view->enableMenuItem(0xE12B, enable);
    m_view->enableMenuItem(0xE12C, enable);
}

bool NpcList::clearItem(int index)
{
    StandardNPC* npc = dynamic_cast<StandardNPC*>(getNode(index));
    if (!npc)
        return false;
    npc->reset();
    return true;
}

int NpcList::indexOfNode(Node* node)
{
    return m_npcs.indexOf(dynamic_cast<StandardNPC*>(node));
}

Node* NpcList::findNode(Node* like)
{
    StandardNPC* npc = dynamic_cast<StandardNPC*>(like);
    if (npc)
        return m_data->find(FIELD(BYTE, npc, 0x0C), FIELD(short, npc, 0x10));
    return 0;
}

void NpcList::onPasteDone(bool added)
{
    if (added)
        sort();
}
