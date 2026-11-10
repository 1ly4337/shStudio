#include "ItemsList.h"
#include <commctrl.h>
#include "Data_Item.h"
#include "iStream.hpp"
#include "String.hpp"
#include "StudioFrame.h"
#include "ClientData.h"
#include "Data_Items.h"
#include "_imports_D.h"

ItemsList::ItemsList(Window* owner, Merchant* merchant)
    : ListCtrlOD(),
      DropTarget(),
      m_owner(owner),
      m_merchant(merchant),
      m_items()
{
    ClientData& clientData = g_studioFrame->m_client.m_clientData;
    clientData.loadSData(&clientData.m_items, L"Item\\item.sdata", L"BinarySData\\DBItemData.sdata", 0);
    m_itemDB = &clientData.m_items;
}

ItemsList::~ItemsList()
{
}

void ItemsList::load(const primitiveArray<long>* ids)
{
    setItemCount(0);
    int count = ids->m_count;
    for (int i = 0; i < count; ++i)
    {
        if (i >= 0 && i < ids->m_count)
        {
            long id = ids->m_items[i];
            if (id)
            {
                ClientData& clientData = g_studioFrame->m_client.m_clientData;
                if (clientData.loadSData(&clientData.m_items, L"Item\\item.sdata", L"BinarySData\\DBItemData.sdata", 0))
                    m_items.add(clientData.m_items.Data::Items::sub_42FC00(id / 1000, id % 1000));
                else
                    m_items.add(0);
            }
        }
    }
    setItemCount(m_items.size());
}

void ItemsList::save(primitiveArray<long>* ids)
{
    ids->m_count = 0;
    int count = m_items.size();
    for (int i = 0; i < count; ++i)
    {
        Data::Item* item = m_items.get(i);
        if (item)
            ids->add(FIELD(unsigned char, item, 0x55) + 1000 * FIELD(unsigned char, item, 0x54));
    }
}

bool ItemsList::acceptsFormat(int format)
{
    return format == 514;
}

void ItemsList::setDropHighlight(bool on)
{
    setHighlight(on);
}

void ItemsList::onDropStream(int format, iStream& stream)
{
    if (format != 514)
        return;
    int count = (int)stream.readInt();
    Data::Item item(0, 0x500);
    for (; count > 0; --count)
    {
        item.read(stream, 'clip');
        unsigned char type = FIELD(unsigned char, &item, 0x54);
        unsigned char id = FIELD(unsigned char, &item, 0x55);
        int key = id + 1000 * type;
        if (!findItem(&m_items, key, key))
        {
            Data::Item* found;
            int dbCount = m_itemDB->m_count;
            int i = 0;
            if (dbCount <= 0)
            {
                found = 0;
            }
            else
            {
                Data::Item** entry = m_itemDB->m_items;
                for (;;)
                {
                    found = *entry;
                    if (FIELD(unsigned char, found, 0x54) == type && FIELD(unsigned char, found, 0x55) == id)
                        break;
                    ++i;
                    ++entry;
                    if (i >= dbCount)
                    {
                        found = 0;
                        break;
                    }
                }
            }
            m_items.add(found);
        }
    }
    setItemCount(m_items.size());
}

DWORD ItemsList::getStyle()
{
    return 0x801009;
}

bool ItemsList::onCreate()
{
    bool result = ListCtrlOD::onCreate();
    if (result)
    {
        addColumn(L"Type", 40, 18, 'type');
        addColumn(L"ID", 40, 18, 'tpid');
        addColumn(L"Name", 120, 4096, 'name');
        addColumn(L"Level", 40, 18, 'lvl ');
        addColumn(L"Slot", 40, 18, 'slot');
        return true;
    }
    return result;
}

bool ItemsList::onActivateItem(int index, int keys)
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

bool ItemsList::onNotify(NMHDR* hdr, LRESULT* result)
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
            SetDlgItemTextW(GetParent(m_hWnd), 111, text.c_str());
        }
    }
    return true;
}

Node* ItemsList::getNode(int index)
{
    return m_items.m_items[index];
}

int ItemsList::indexOfNode(Node* node)
{
    return m_items.indexOf(dynamic_cast<Data::Item*>(node));
}

Node* ItemsList::findNode(Node* like)
{
    Data::Item& item = dynamic_cast<Data::Item&>(*like);
    return m_itemDB->Data::Items::sub_42FC00(FIELD(unsigned char, &item, 0x54), FIELD(unsigned char, &item, 0x55));
}

bool ItemsList::removeItem(int index)
{
    return m_items.detach(index) != 0;
}

bool ItemsList::moveItemUp(int index)
{
    return m_items.swap(index, index - 1);
}

bool ItemsList::moveItemDown(int index)
{
    return m_items.swap(index, index + 1);
}

Data::Item* ItemsList::findItem(Data::ItemsSet* items, int , const int& key)
{
    int count = items->m_count;
    int i = 0;
    if (count <= 0)
        return 0;
    for (Data::Item** entry = items->m_items;
         FIELD(unsigned char, *entry, 0x55) + 1000 * FIELD(unsigned char, *entry, 0x54) != key; ++entry)
    {
        if (++i >= count)
            return 0;
    }
    return items->m_items[i];
}
