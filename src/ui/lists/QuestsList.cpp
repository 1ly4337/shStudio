#include "QuestsList.h"
#include "QuestsDetails.h"
#include "QuestEditor.h"
#include "QuestDetails.h"
#include "NpcData.h"
#include "DocView.h"
#include "ida_compat.h"
#include "_imports_E.h"
#include "Collection.hpp"

QuestsList::QuestsList(DocView* view, NpcData* data)
    : NpcList(view, data)
{
    m_list = &data->m_quests;
    m_filter0 = false;
    m_filterFlags = 0;
}

bool QuestsList::onCreate()
{
    bool result = NpcList::onCreate();
    if (result)
    {
        addColumn(L"ID", 50, 16, 'quid');
        addColumn(L"Name", 150, 4096, 'name');
        addColumn(L"Lvl min", 55, 18, 'lmin');
        addColumn(L"Lvl max", 55, 18, 'lmax');
        addColumn(L"Type", 55, 2, 'qtyp');
        addColumn(L"Mode", 50, 18, 'qmod');
        addColumn(L"Reward", 50, 18, 'clrw');
        addColumn(L"Default", 50, 18, 'drwd');
        addColumn(L"Repeat", 50, 18, 'rept');
        return true;
    }
    return result;
}

bool QuestsList::onCommand(int code, LPARAM ctrl, int id)
{
    return ListCtrlOD::onCommand(code, ctrl, id);
}

bool QuestsList::onInitMenuPopup(HMENU menu)
{
    NpcList::onInitMenuPopup(menu);
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, 0x2C);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    mii.fState = 0;
    SetMenuItemInfoW(menu, 0xE12B, FALSE, &mii);
    memset(&mii.fMask, 0, 0x2C);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    mii.fState = 0;
    SetMenuItemInfoW(menu, 0xE12C, FALSE, &mii);
    return true;
}

int QuestsList::insertItem(Node* node, int index)
{
    QuestDetails* quest = dynamic_cast<QuestDetails*>(node);
    quest->m_questId = (short)(m_list->size() + 1);
    return m_list->insert(quest, -1);
}

Node* QuestsList::getNode(int index)
{
    return m_list->get(index);
}

static void checkMenuItem(HMENU menu, UINT id, bool checked)
{
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, 0x2C);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    GetMenuItemInfoW(menu, id, FALSE, &mii);
    if ((mii.fState & 3) == 0)
    {
        UINT state = 0;
        if (checked)
            state = MFS_CHECKED;
        mii.fState = state;
        SetMenuItemInfoW(menu, id, FALSE, &mii);
    }
}

void QuestsList::onContextMenu(HMENU menu)
{
    checkMenuItem(menu, 1000, m_filter0);
    checkMenuItem(menu, 1001, (m_filterFlags & 1) != 0);
    checkMenuItem(menu, 1002, ((m_filterFlags >> 1) & 1) != 0);
    checkMenuItem(menu, 1003, (m_filterFlags & 3) != 0);
    checkMenuItem(menu, 1004, ((m_filterFlags >> 2) & 1) != 0);
    checkMenuItem(menu, 1005, (m_filterFlags & 5) != 0);
    checkMenuItem(menu, 1006, (m_filterFlags & 6) != 0);
}

bool QuestsList::sort()
{
    int column2 = m_prevSortColumn;
    QuestsDetails* list = m_list;
    bool ascending = m_sortAscending;
    int count = list->m_count;
    int column = m_sortColumn;
    if (count < 2)
        return false;
    list->m_changes = 0;
    quickSort(*list, 0, count - 1, column, column2, ascending);
    return list->m_changes > 0;
}

bool QuestsList::onActivateItem(int index, int keys)
{
    QuestDetails* quest = m_list->get(index);
    if (quest)
    {
        QuestEditor editor(quest);
        if (editor.open(m_hWnd) && editor.doModal() == IDOK)
        {
            m_view->setModified(true);
            updateItem(index);
        }
    }
    return false;
}

bool QuestsList::removeItem(int index)
{
    int item = index;
    if (index == -2)
        item = getNextSelected(0);
    if (m_list->get(item))
        return m_list->remove(item);
    return false;
}

Node* QuestsList::createNode()
{
    return new QuestDetails(m_list->m_version);
}

bool QuestsList::clearItem(int index)
{
    int item = index;
    if (index == -2)
        item = getNextSelected(0);
    QuestDetails* quest = m_list->get(item);
    if (!quest)
        return false;
    quest->reset();
    return true;
}

int QuestsList::indexOfNode(Node* node)
{
    return m_list->indexOf(dynamic_cast<QuestDetails*>(node));
}

Node* QuestsList::findNode(Node* like)
{
    short id = dynamic_cast<QuestDetails&>(*like).m_questId;
    QuestsDetails* list = m_list;
    int count = list->m_count;
    QuestDetails** items = list->m_items;
    for (int i = 0; i < count; ++i)
    {
        if (items[i] && items[i]->m_questId == id)
            return items[i];
    }
    return 0;
}
