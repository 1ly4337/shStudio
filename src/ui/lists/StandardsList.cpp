#include "StandardsList.h"
#include "StandardEditor.h"
#include "NpcData.h"
#include "StandardNPC.h"
#include "DocView.h"
#include "ida_compat.h"
#include "_imports_E.h"

StandardsList::StandardsList(DocView* view, NpcData* data)
    : NpcList(view, data)
{
}

bool StandardsList::onCreate()
{
    bool result = NpcList::onCreate();
    if (result)
    {
        addColumn(L"Type", 50, 18, 'type');
        addColumn(L"Type name", 80, 2, 'fami');
        addColumn(L"UID", 50, 18, 'uid ');
        addColumn(L"Name", 150, 4096, 'name');
        addColumn(L"Cntr", 45, 18, 'cntr');
        addColumn(L"Shape", 45, 18, 'shpe');
        addColumn(L"#Qst In", 50, 18, 'kqin');
        addColumn(L"#Qst Out", 50, 18, 'kqou');
        addColumn(L"Qst In", 50, 0, 'qsti');
        addColumn(L"Qst Out", 50, 0, 'qsto');
        return true;
    }
    return result;
}

bool StandardsList::onActivateItem(int index, int keys)
{
    StandardNPC* npc = m_npcs.get(index);
    if (npc)
    {
        unsigned char oldType = FIELD(BYTE, npc, 0x0C);
        StandardEditor editor(npc, false);
        if (editor.create(m_hWnd) && editor.doModal() == IDOK)
        {
            m_view->setModified(true);
            if (FIELD(int, npc, 0x0C) != oldType)
            {
                StandardNPCs* from = npcTypeList(m_data, oldType);
                from->detachItem(npc);
                StandardNPCs* to = npcTypeList(m_data, FIELD(BYTE, npc, 0x0C));
                to->add(npc);
            }
            updateItem(index);
            selectItem(index);
        }
    }
    return false;
}

Node* StandardsList::createNode()
{
    StandardNPC* npc = new StandardNPC(0);
    StandardEditor editor(npc, true);
    if (!editor.create(m_hWnd) || editor.doModal() != IDOK)
    {
        if (npc)
            delete npc;
        npc = 0;
    }
    return npc;
}

bool StandardsList::removeItem(int index)
{
    int item = index;
    if (index == -2)
        item = getNextSelected(0);
    StandardNPC* npc = m_npcs.get(item);
    if (!npc)
        return false;
    if ((unsigned char)(FIELD(BYTE, npc, 0x0C) - 1) > 12)
        return false;
    StandardNPCs* list = npcTypeList(m_data, FIELD(BYTE, npc, 0x0C));
    if (!list || FIELD(short, npc, 0x10) != list->size() || !list->detachItem(npc))
        return false;
    list->destroyItem(npc);
    m_npcs.detach(item);
    return true;
}

void StandardsList::refill()
{
    for (int i = 0; i < m_npcs.m_count; ++i)
        m_npcs.m_items[i] = 0;
    m_npcs.m_count = 0;
    unsigned char type = 3;
    do
    {
        StandardNPCs* list = npcTypeList(m_data, type);
        int count = list->size();
        for (int i = 0; i < count; ++i)
            m_npcs.add(list->m_items[i]);
        ++type;
    } while (type < 14);
    setItemCount(m_npcs.size());
}
