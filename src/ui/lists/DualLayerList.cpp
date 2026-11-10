#include "DualLayerList.h"
#include "DualLayerModels.h"
#include "DualLayerModel.h"
#include "ModelView.h"
#include "Node.h"
#include "Collection.hpp"
#include "_imports_p06.h"

Node* DualLayerList::getNode(int index)
{
    return m_models->get(index);
}

int DualLayerList::indexOfNode(Node* node)
{
    return m_models->indexOf(dynamic_cast<DualLayerModel*>(node));
}

bool DualLayerList::sort()
{
    DualLayerModels* models = m_models;
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

Node* DualLayerList::findNode(Node* like)
{
    short index = dynamic_cast<DualLayerModel&>(*like).m_index;
    DualLayerModels* models = m_models;
    for (int i = 0; i < models->m_count; ++i)
    {
        if (models->m_items[i]->m_index == index)
            return models->m_items[i];
    }
    return nullptr;
}

Node* DualLayerList::createNode()
{
    return new DualLayerModel(m_models->serialV5(), 0);
}

bool DualLayerList::removeItem(int index)
{
    return m_models->remove(index);
}

int DualLayerList::insertItem(Node* node, int index)
{
    return m_models->insert(dynamic_cast<DualLayerModel*>(node), index);
}

bool DualLayerList::onCreate()
{
    if (!m_models || !ListCtrlOD::onCreate())
        return false;
    addColumn(L"Index", 40, 18, 'indx');
    addColumn(L"Top", 60, 18, 'top ');
    addColumn(L"Hand", 60, 18, 'hand');
    addColumn(L"Bottom", 60, 18, 'bttm');
    addColumn(L"Shoes", 60, 18, 'shoe');
    addColumn(L"Empty", 60, 18, 'empt');
    addColumn(L"Helmet", 60, 18, 'hlmt');
    return true;
}

bool DualLayerList::moveItemUp(int index)
{
    if (!m_models->swap(index, index - 1))
        return false;
    m_view->setModified(1);
    return true;
}

bool DualLayerList::moveItemDown(int index)
{
    if (!m_models->swap(index, index + 1))
        return false;
    m_view->setModified(1);
    return true;
}

UINT DualLayerList::getClipboardFormat()
{
    return 0x20D;
}

bool DualLayerList::onCopied(Node* node)
{
    return dynamic_cast<DualLayerModel*>(node) != nullptr;
}

void DualLayerList::onPasted(Node* node)
{
    dynamic_cast<DualLayerModel*>(node);
}
