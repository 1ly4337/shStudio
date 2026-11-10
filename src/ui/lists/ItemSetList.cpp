#include "ItemSetList.h"
#include "ItemSetView.h"
#include "SetEditor.h"
#include "Data_ItemSets.h"
#include "_imports_p07.h"

ItemSetList::ItemSetList(ItemSetView* owner)
    : ListCtrlOD()
{
    m_owner = owner;
    m_sets = 0;
}

bool ItemSetList::onCreate()
{
    bool ok = ListCtrl::onCreate();
    if (!ok)
        return ok;
    addColumn(L"Index", 50, 18, 'indx');
    addColumn(L"Name", 300, 0, 'name');
    return true;
}

bool ItemSetList::onActivateItem(int index, int keys)
{
    Data::ItemSet* set = dynamic_cast<Data::ItemSet*>(getNode(index));
    if (set) {
        SetEditor dlg(set);
        if (dlg.createDialog(m_hWnd, 660, 430, 1, L"Item Set Editor") && dlg.doModal() == 1 && dlg.m_modified)
            m_owner->setModified(true);
    }
    return false;
}

void ItemSetList::onItemChanged(int index, UINT newState)
{
    (void)dynamic_cast<Data::ItemSet*>(getNode(index));
}

int ItemSetList::indexOfNode(Node* node)
{
    return m_sets->indexOf(dynamic_cast<Data::ItemSet*>(node));
}
