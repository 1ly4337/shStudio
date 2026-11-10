#include "AltList.h"
#include "AltView.h"
#include "CharActions.h"
#include "CharAction.h"
#include "Node.h"
#include "world_common.h"

int AltList::listV69()
{
    return 'name';
}

UINT AltList::getClipboardFormat()
{
    return 0x205;
}

Node* AltList::getNode(int index)
{
    return (Node*)m_data->m_items[index];
}

int AltList::indexOfNode(Node* node)
{
    return m_data->indexOf(dynamic_cast<CharAction*>(node));
}

Node* AltList::findNode(Node* like)
{
    int id = dynamic_cast<CharAction&>(*like).m_id;
    int count = m_data->m_count;
    for (int i = 0; i < count; ++i)
    {
        if (m_data->m_items[i]->m_id == id)
            return (Node*)m_data->m_items[i];
    }
    return 0;
}

void AltList::onItemChanged(int index, UINT newState)
{
    bool selected = index != -1;
    m_view->enableMenuItem(0xE123, selected);
    m_view->enableMenuItem(0xE122, selected);
    m_view->enableMenuItem(0xE12B, false);
    m_view->enableMenuItem(0xE12C, selected);
}

bool AltList::onInitMenuPopup(HMENU menu)
{
    ListCtrlOD::onInitMenuPopup(menu);
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, sizeof(mii) - sizeof(mii.cbSize));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    mii.fState = MFS_DISABLED | MFS_GRAYED;
    SetMenuItemInfoW(menu, 0xE12B, FALSE, &mii);
    return true;
}
