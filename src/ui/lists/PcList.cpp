#include "PcList.h"
#include "PcView.h"
#include "CapesData.h"
#include "CapeData.h"
#include "CapeDataEditor.h"
#include "_imports_p07.h"

UINT PcList::getClipboardFormat()
{
    return 524;
}

void PcList::onItemChanged(int index, UINT newState)
{
    bool hasSelection = index != -1;
    m_view->enableMenuItem(0xE122, false);
    m_view->enableMenuItem(0xE123, hasSelection);
    m_view->enableMenuItem(0xE124, (BYTE)IsClipboardFormatAvailable(getClipboardFormat()) != 0);
    m_view->enableMenuItem(0xE12A, true);
    m_view->enableMenuItem(0xE12B, false);
    m_view->enableMenuItem(0xE12C, hasSelection);
}

int PcList::indexOfNode(Node* node)
{
    return m_capes->indexOf(dynamic_cast<CapeData*>(node));
}

Node* PcList::findNode(Node* like)
{
    int index = FIELD(int, &dynamic_cast<CapeData&>(*like), 0x0C) ;
    CapesData* capes = m_capes;
    int count = capes->m_count;
    for (int i = 0; i < count; i++)
    {
        if (FIELD(int, capes->m_items[i], 0x0C)  == index)
            return capes->m_items[i];
    }
    return 0;
}

Node* PcList::createNode()
{
    return new CapeData(m_capes->size());
}

bool PcList::onCreate()
{
    if (!m_capes || !ListCtrlOD::onCreate())
        return false;
    addColumn(L"Model", 40, 18, 'indx');
    addColumn(L"3DC", 45, 18, 'b3DC');
    addColumn(L"DDS", 45, 18, 'bDDS');
    addColumn(L"Unk 1", 45, 18, 'unk1');
    addColumn(L"Unk 2", 45, 18, 'unk2');
    addColumn(L"Unk 3", 45, 18, 'unk3');
    addColumn(L"Unk 4a", 50, 18, 'uk41');
    addColumn(L"Unk 4b", 50, 18, 'uk42');
    addColumn(L"Unk 4c", 50, 18, 'uk43');
    addColumn(L"Unk 4d", 50, 18, 'uk44');
    return true;
}

bool PcList::onActivateItem(int index, int keys)
{
    if (index != -1)
    {
        PcView* view = dynamic_cast<PcView*>(m_view);
        if (view)
            view->editModel(index);
    }
    return false;
}

int PcList::insertItem(Node* node, int index)
{
    CapeData* cape = dynamic_cast<CapeData*>(node);
    if (!cape)
        return 0;
    int result;
    CapeDataEditor editor(cape);
    if (editor.create(m_hWnd) && editor.doModal() == IDOK)
        result = m_capes->insert(cape, index);
    else
        result = 0;
    return result;
}
