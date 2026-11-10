#include "StockList.h"
#include <commctrl.h>
#include "Data_Item.h"
#include "String.hpp"
#include "StudioFrame.h"
#include "ClientData.h"
#include "Data_Items.h"
#include "_imports_D.h"
#include "Collection.hpp"

StockList::StockList(Window* owner)
    : ListCtrlOD(),
      m_owner(owner),
      m_items()
{
    ClientData& clientData = g_studioFrame->m_client.m_clientData;
    clientData.loadSData(&clientData.m_items, L"Item\\item.sdata", L"BinarySData\\DBItemData.sdata", 0);
    m_itemDB = &clientData.m_items;
    m_dragMode = 2;
}

StockList::~StockList()
{
}

bool StockList::load(int itemType, unsigned char subType, bool lightFaction, int itemClass)
{
    SendMessageW(m_hWnd, LVM_DELETEALLITEMS, 0, 0);
    setItemCount(0);
    bool ok = m_itemDB->Data::Items::sub_42FCC0(itemType, subType, lightFaction, itemClass, &m_items);
    if (ok)
    {
        int count = m_items.m_count;
        if (count >= 2)
        {
            m_items.m_changes = 0;
            quickSort(m_items, 0, count - 1, 'type', 'tpid', true);
        }
        setItemCount(m_items.size());
        return ok;
    }
    return ok;
}

DWORD StockList::getStyle()
{
    return 0x801009;
}

bool StockList::onCreate()
{
    bool result = ListCtrlOD::onCreate();
    if (result)
    {
        addColumn(L"Type", 40, 18, 'type');
        addColumn(L"ID", 40, 18, 'tpid');
        addColumn(L"Name", 120, 4096, 'name');
        addColumn(L"Level", 40, 18, 'lvl ');
        return true;
    }
    return result;
}

bool StockList::onActivateItem(int index, int keys)
{
    Data::Item* item = m_items.get(index);
    if (item)
    {
        ClientData& clientData = g_studioFrame->m_client.m_clientData;
        clientData.loadSData(&clientData.m_items, L"Item\\item.sdata", L"BinarySData\\DBItemData.sdata", 0);
        if (openItemEditor(m_owner, &clientData.m_items, item))
        {
            void* file = g_studioFrame->findFile(L"Item\\item.sdata");
            if (file)
                FIELD(int, file, 0xFC) = 2;
        }
    }
    return true;
}

bool StockList::onNotify(NMHDR* hdr, LRESULT* result)
{
    if ((int)hdr->code != LVN_ITEMCHANGED)
        return ListCtrlOD::onNotify(hdr, result);
    int index = reinterpret_cast<NMLISTVIEW*>(hdr)->iItem;
    if (index != -1)
    {
        Data::Item* item = dynamic_cast<Data::Item*>(getNode(index));
        if (item)
        {
            String text(item->getString('dtls'), (unsigned)-1);
            unsigned char slots = FIELD(unsigned char, item, 0x7B);
            if (slots)
                (text << L", Slot(s): ") << slots;
            SetDlgItemTextW(GetParent(m_hWnd), 123, text.c_str());
        }
    }
    return true;
}

Node* StockList::getNode(int index)
{
    return m_items.m_items[index];
}

bool StockList::sort()
{
    int sortColumn = m_sortColumn;
    bool ascending = m_sortAscending;
    int count = m_items.m_count;
    int prevColumn = m_prevSortColumn;
    if (count < 2)
        return false;
    m_items.m_changes = 0;
    quickSort(m_items, 0, count - 1, sortColumn, prevColumn, ascending);
    return m_items.m_changes > 0;
}

int StockList::indexOfNode(Node* node)
{
    return m_items.indexOf(dynamic_cast<Data::Item*>(node));
}

Node* StockList::findNode(Node* like)
{
    Data::Item& item = dynamic_cast<Data::Item&>(*like);
    return m_itemDB->Data::Items::sub_42FC00(FIELD(unsigned char, &item, 0x54), FIELD(unsigned char, &item, 0x55));
}
