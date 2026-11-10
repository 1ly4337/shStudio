#include "GateKeeperList.h"
#include "GateKeeperEditor.h"
#include "DocView.h"
#include "StandardNPC.h"
#include "GateKeeper.h"
#include "ptrArray.h"
#include "_imports_D.h"

bool GateKeeperList::onCreate()
{
    bool result = NpcList::onCreate();
    if (result)
    {
        addColumn(L"TypeID", 50, 18, 'uid ');
        addColumn(L"Name", 150, 4096, 'name');
        addColumn(L"Cntr", 45, 18, 'cntr');
        addColumn(L"Shape", 45, 18, 'shpe');
        addColumn(L"Dest 1", 120, 4096, 'loc1');
        addColumn(L"Dest 2", 120, 4096, 'loc2');
        addColumn(L"Dest 3", 120, 4096, 'loc3');
        return true;
    }
    return result;
}

bool GateKeeperList::onActivateItem(int index, int keys)
{
    GateKeeper* gateKeeper = dynamic_cast<GateKeeper*>(m_list->get(index));
    if (gateKeeper)
    {
        GateKeeperEditor editor(gateKeeper);
        if (editor.create(m_hWnd))
        {
            if (editor.doModal() == 1)
            {
                m_view->setModified(true);
                updateItem(index);
            }
            selectItem(index);
        }
    }
    return false;
}

Node* GateKeeperList::createNode()
{
    GateKeeper* gateKeeper = new GateKeeper();
    if (GateKeeperEditor::edit(m_view->m_hWnd, gateKeeper))
        return gateKeeper;
    if (gateKeeper)
        delete gateKeeper;
    return 0;
}

int GateKeeperList::insertItem(Node* node, int index)
{
    GateKeeper* gateKeeper = dynamic_cast<GateKeeper*>(node);
    if (gateKeeper && m_list->add(gateKeeper))
        return m_npcs.insert(gateKeeper, index);
    return -1;
}

bool GateKeeperList::removeItem(int index)
{
    if (index == -2)
        index = getNextSelected(0);
    GateKeeper* gateKeeper = dynamic_cast<GateKeeper*>(m_list->get(index));
    if (gateKeeper && gateKeeper->m_id == m_list->size())
        return m_list->remove(index);
    return false;
}

bool GateKeeperList::clearItem(int index)
{
    GateKeeper* gateKeeper = dynamic_cast<GateKeeper*>(m_list->get(index));
    if (!gateKeeper)
        return false;
    gateKeeper->reset();
    return true;
}
