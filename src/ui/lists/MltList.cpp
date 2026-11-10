#include "MltList.h"
#include "ModelsMLT.h"
#include "ModelMLT.h"
#include "ModelView.h"
#include "Node.h"
#include "Collection.hpp"
#include "_imports_p06.h"

Node* MltList::getNode(int index)
{
    return m_models->m_items[index];
}

bool MltList::sort()
{
    ModelsMLT* models = m_models;
    int prevColumn = m_prevSortColumn;
    bool ascending = m_sortAscending;
    int count = models->m_count;
    int column = m_sortColumn;
    if (count < 2)
        return false;
    models->m_changes = 0;
    quickSort(*models, 0, count - 1, column, prevColumn, ascending);
    return models->m_changes > 0;
}

bool MltList::removeItem(int index)
{
    return m_models->remove(index);
}

bool MltList::moveItemUp(int index)
{
    if (!m_models->swap(index, index - 1))
        return false;
    ModelMLT* a = m_models->get(index);
    ModelMLT* b = m_models->get(index - 1);
    int id = a->m_id;
    a->m_id = b->m_id;
    b->m_id = id;
    m_view->setModified(1);
    return true;
}

bool MltList::moveItemDown(int index)
{
    if (!m_models->swap(index, index + 1))
        return false;
    ModelMLT* a = m_models->get(index);
    ModelMLT* b = m_models->get(index + 1);
    int id = a->m_id;
    a->m_id = b->m_id;
    b->m_id = id;
    m_view->setModified(1);
    return true;
}

int MltList::indexOfNode(Node* node)
{
    return m_models->indexOf(dynamic_cast<ModelMLT*>(node));
}

Node* MltList::findNode(Node* like)
{
    int id = dynamic_cast<ModelMLT&>(*like).m_id;
    ModelsMLT* models = m_models;
    for (int i = 0; i < models->m_count; ++i)
    {
        if (models->m_items[i]->m_id == id)
            return models->m_items[i];
    }
    return nullptr;
}

Node* MltList::createNode()
{
    return new ModelMLT(m_models->size(), L"", L"", 0, 0);
}

int MltList::insertItem(Node* node, int index)
{
    return m_models->insert(dynamic_cast<ModelMLT*>(node), index);
}

bool MltList::onCreate()
{
    if (!m_models || !ListCtrlOD::onCreate())
        return false;
    addColumn(L"Model", 45, 18, 'indx');
    addColumn(L"Alpha", 45, 18, 'alph');
    addColumn(L"DDS", 150, 0, 'text');
    addColumn(L"3DC", 150, 0, 'vert');
    addColumn(L"Version", 55, 66, 'vrs3');
    return true;
}
