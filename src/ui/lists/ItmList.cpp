#include "ItmList.h"
#include "ItemModels.h"
#include "ItemModel.h"
#include "ModelView.h"
#include "Node.h"
#include "String.hpp"
#include "Bytes.h"
#include "Clipboard.h"
#include "StudioFrame.h"
#include <cstring>
#include <cwchar>
#include <shlwapi.h>
#include "Collection.hpp"
#include "_imports_p06.h"

wchar_t g_itmListText[16];

Node* ItmList::getNode(int index)
{
    return m_models->get(index);
}

bool ItmList::sort()
{
    ItemModels* models = m_models;
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

bool ItmList::removeItem(int index)
{
    return m_models->remove(index);
}

bool ItmList::moveItemUp(int index)
{
    if (!m_models->swap(index, index - 1))
        return false;
    m_view->setModified(1);
    return true;
}

bool ItmList::moveItemDown(int index)
{
    if (!m_models->swap(index, index + 1))
        return false;
    m_view->setModified(1);
    return true;
}

UINT ItmList::getClipboardFormat()
{
    return 0x209;
}

int ItmList::indexOfNode(Node* node)
{
    return m_models->indexOf(dynamic_cast<ItemModel*>(node));
}

Node* ItmList::findNode(Node* like)
{
    int id = dynamic_cast<ItemModel&>(*like).m_id;
    ItemModels* models = m_models;
    for (int i = 0; i < models->m_count; ++i)
    {
        ItemModel* model = models->m_items[i];
        if (model && model->m_id == id)
            return model;
    }
    return nullptr;
}

Node* ItmList::createNode()
{
    return new ItemModel(m_models->size(), 'ITM ', 0);
}

int ItmList::insertItem(Node* node, int index)
{
    return m_models->insert(dynamic_cast<ItemModel*>(node), index);
}

bool ItmList::onCreate()
{
    if (!m_models || !ListCtrlOD::onCreate())
        return false;
    addColumn(L"Model", 40, 18, 'indx');
    addColumn(L"DDS", 110, 0, 'text');
    addColumn(L"3DC", 110, 0, 'vert');
    addColumn(L"Glow", 45, 18, 'glow');
    addColumn(L"Unk 2", 45, 18, 'unk2');
    addColumn(L"Unk 3", 45, 18, 'unk3');
    addColumn(L"Format", 50, 18, 'frmt');
    addColumn(L"Unk 5", 45, 18, 'unk5');
    addColumn(L"RGBA", 75, 0x2002, 'rgba');
    addColumn(L"Rot°", 50, 0x2002, 'RotY');
    addColumn(L"Scale", 50, 0x2002, 'scal');
    return true;
}

bool ItmList::onCopied(Node* node)
{
    ItemModel* model = dynamic_cast<ItemModel*>(node);
    if (!model)
        return false;
    String dir(FIELD(String, m_node, 0x10));
    dir.removeFileName();
    String texture(dir);
    texture << L"\\DDS\\" << model->m_texture;
    String vertex(dir);
    vertex << L"\\3DO\\" << model->m_vertex;
    Bytes data;
    if (studioLoadFile(&data, &texture) == 0)
        Clipboard::setData(0x228, data, false);
    if (studioLoadFile(&data, &dir) == 0)
        Clipboard::setData(0x22A, data, false);
    return true;
}

void ItmList::onPasted(Node* node)
{
    ItemModel* model = dynamic_cast<ItemModel*>(node);
    if (!model)
        return;
    Bytes data;
    String dir(FIELD(String, m_node, 0x10));
    dir.removeFileName();
    String texture(dir);
    texture << L"\\DDS\\" << model->m_texture;
    String vertex(dir);
    vertex << L"\\3DO\\" << model->m_vertex;
    if (Clipboard::getData(0x228, data) && PathFileExistsW(texture.c_str()) != TRUE)
        data.aStream::save(texture.c_str());
    if (Clipboard::getData(0x22A, data) && PathFileExistsW(vertex.c_str()) != TRUE)
        data.aStream::save(vertex.c_str());
}

const wchar_t* ItmList::getItemText(Node* node, int columnId)
{
    ItemModel* model = dynamic_cast<ItemModel*>(node);
    if (!model || model->m_format < 1)
        return L"";
    memset(g_itmListText, 0, sizeof(g_itmListText));
    switch (columnId)
    {
    case 'rgba':
        swprintf_s(g_itmListText, L"%X", model->m_rgba);
        return g_itmListText;
    case 'RotY':
        swprintf_s(g_itmListText, L"%.3f", (double)model->m_rotY);
        return g_itmListText;
    case 'scal':
        swprintf_s(g_itmListText, L"%.3f", (double)model->m_scale);
        return g_itmListText;
    default:
        return g_itmListText;
    }
}
