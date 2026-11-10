#include "MonList.h"
#include "ModelView.h"
#include "MonsterModels.h"
#include "MonsterModel.h"
#include "Collection.hpp"
#include "mon_g1.h"
#include "_imports_p07.h"

bool MonList::onCreate()
{
    if (!m_node || !ModelList::onCreate())
        return false;
    addColumn(L"Index", 40, 16, 'indx');
    addColumn(L"Name", 150, 0, 'name');
    addColumn(L"# Vert.", 70, 18, '#ver');
    addColumn(L"# Text.", 70, 18, '#txt');
    addColumn(L"Height", 70, 130, 'hght');
    addColumn(L"Unk1", 50, 18, 'unk1');
    addColumn(L"# Eff.", 70, 18, '#eff');
    addColumn(L"Effects", 100, 0, 'effc');
    return true;
}

int MonList::insertItem(Node* node, int index)
{
    ptrArray<MonsterModel*>& list = *models();
    return list.insert(dynamic_cast<MonsterModel*>(node), index);
}

Node* MonList::getNode(int index)
{
    MonsterModels* data = models();
    if (index < 0 || index >= data->m_count)
        return 0;
    return (Node*)data->m_items[index];
}

bool MonList::sort()
{
    bool ascending = m_sortAscending;
    int prevColumn = m_prevSortColumn;
    int column = m_sortColumn;
    ptrArray<MonsterModel*>& list = *models();
    int count = list.m_count;
    if (count >= 2)
    {
        list.m_changes = 0;
        quickSort(list, 0, count - 1, column, prevColumn, ascending);
    }
    return true;
}

void MonList::onItemChanged(int index, UINT newState)
{
    ModelList::onItemChanged(index, newState);
    MonsterModels* data = models();
    bool enable = data && index == ((ptrArray<MonsterModel*>&)*data).size();
    m_view->enableMenuItem(0xE12B, enable);
}

bool MonList::removeItem(int index)
{
    ptrArray<MonsterModel*>& list = *models();
    if (index + 1 == list.size())
        return list.remove(index);
    return false;
}

Node* MonList::createNode()
{
    MonsterModels* data = models();
    ptrArray<MonsterModel*>& list = *data;
    return (Node*)new MonsterModel(data->m_signature, list.size());
}

bool MonList::clearItem(int index)
{
    MonsterModels* data = models();
    MonsterModel* model = (index < 0 || index >= data->m_count) ? 0 : data->m_items[index];
    ((Node*)model)->reset();
    return true;
}

bool MonList::moveItemUp(int index)
{
    ptrArray<MonsterModel*>& list = *models();
    bool result = list.swap(index, index - 1);
    if (result)
    {
        m_view->setModified(true);
        return true;
    }
    return result;
}

bool MonList::moveItemDown(int index)
{
    ptrArray<MonsterModel*>& list = *models();
    bool result = list.swap(index, index + 1);
    if (result)
    {
        m_view->setModified(true);
        return true;
    }
    return result;
}

int MonList::indexOfNode(Node* node)
{
    ptrArray<MonsterModel*>& list = *models();
    return list.indexOf(dynamic_cast<MonsterModel*>(node));
}

Node* MonList::findNode(Node* like)
{
    MonsterModel* model = dynamic_cast<MonsterModel*>(like);
    if (model)
    {
        int index = MM_INDEX(model);
        MonsterModels* data = models();
        if (index >= 0 && index < data->m_count)
            return (Node*)data->m_items[index];
    }
    return 0;
}
