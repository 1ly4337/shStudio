#include "p05_common.h"
#include "MlxList.h"
#include "ModelView.h"
#include "ModelsMLX.h"
#include "ModelMLX.h"

DWORD MlxList::getStyle()
{
    return 32781;
}

void MlxList::onChanged()
{
    m_view->setModified(true);
}

DWORD MlxList::getExStyle()
{
    return 33;
}

int MlxList::insertItem(Node* node, int index)
{
    return models()->insert(dynamic_cast<ModelMLX*>(node), index);
}

Node* MlxList::getNode(int index)
{
    return (Node*)models()->get(index);
}

int MlxList::getItemMenu()
{
    return 198;
}

bool MlxList::onActivateItem(int index, int )
{
    if (index != -1)
        m_view->editModel(index);
    return false;
}

void MlxList::onItemChanged(int index, UINT )
{
    bool selected = index != -1;
    m_view->enableMenuItem(0xE122, false);
    m_view->enableMenuItem(0xE123, selected);
    UINT fmt = getClipboardFormat();
    bool canPaste = IsClipboardFormatAvailable(fmt) != 0;
    m_view->enableMenuItem(0xE124, canPaste);
    m_view->enableMenuItem(0xE12A, true);
    m_view->enableMenuItem(0xE12B, false);
    m_view->enableMenuItem(0xE12C, selected);
    if (index != m_view->m_selected)
        m_view->selectModel(index);
}

bool MlxList::removeItem(int index)
{
    return models()->remove(index);
}

Node* MlxList::createNode()
{
    return new ModelMLX();
}

bool MlxList::clearItem(int index)
{
    Node* node = (Node*)models()->get(index);
    node->reset();
    return true;
}

int MlxList::indexOfNode(Node* node)
{
    return models()->indexOf(dynamic_cast<ModelMLX*>(node));
}

Node* MlxList::findNode(Node* )
{
    int index = getNextSelected(0);
    if (index < 0)
        return 0;
    return (Node*)models()->get(index);
}

UINT MlxList::getClipboardFormat()
{
    return 519;
}
