#include "MerchantList.h"
#include "MerchantEditor.h"
#include "DocView.h"
#include "StandardNPC.h"
#include "Merchant.h"
#include "ptrArray.h"
#include "_imports_D.h"

bool MerchantList::onCreate()
{
    bool result = NpcList::onCreate();
    if (result)
    {
        addColumn(L"TypeID", 50, 18, 'uid ');
        addColumn(L"Name", 150, 4096, 'name');
        addColumn(L"Cntr", 45, 18, 'cntr');
        addColumn(L"Shape", 45, 18, 'shpe');
        addColumn(L"Profession", 150, 0, 'prof');
        addColumn(L"# Items", 70, 18, 'kitm');
        return true;
    }
    return result;
}

bool MerchantList::onActivateItem(int index, int keys)
{
    Merchant* merchant = dynamic_cast<Merchant*>(m_list->get(index));
    if (!merchant || !MerchantEditor::edit(m_view, merchant))
        return false;
    m_view->setModified(true);
    updateItem(index);
    return true;
}

Node* MerchantList::createNode()
{
    Merchant* merchant = new Merchant();
    if (MerchantEditor::edit(m_view, merchant))
        return merchant;
    if (merchant)
        delete merchant;
    return 0;
}

int MerchantList::insertItem(Node* node, int index)
{
    Merchant* merchant = dynamic_cast<Merchant*>(node);
    if (merchant && FIELD(ptrArray<Node*>, m_data, 0x34).add(merchant))
        return m_npcs.insert(merchant, index);
    return -1;
}

bool MerchantList::removeItem(int index)
{
    if (index == -2)
        index = getNextSelected(0);
    Merchant* merchant = dynamic_cast<Merchant*>(m_list->get(index));
    if (merchant && merchant->m_id == m_list->size())
        return m_list->remove(index);
    return false;
}
