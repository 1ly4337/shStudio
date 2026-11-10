#include "ModelList.h"
#include "ModelView.h"
#include "_imports_p06.h"

void ModelList::onChanged()
{
    m_view->setModified(1);
}

bool ModelList::onActivateItem(int index, int keys)
{
    if (index != -1)
        m_view->editModel(index);
    return false;
}

void ModelList::onItemChanged(int index, UINT newState)
{
    bool selected = index != -1;
    m_view->enableMenuItem(0xE122, false);
    m_view->enableMenuItem(0xE123, selected);
    m_view->enableMenuItem(0xE124, IsClipboardFormatAvailable(getClipboardFormat()) == 1);
    m_view->enableMenuItem(0xE12A, true);
    m_view->enableMenuItem(0xE12B, false);
    m_view->enableMenuItem(0xE12C, selected);
    if (index != m_view->m_selected)
        m_view->selectModel(index);
}
