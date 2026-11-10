#include "p02_common.h"
#include "Data_ItemSets.h"
#include "Data_ItemSet.h"
#include "Data_Items.h"

namespace Data {

void ItemSets::load(iStream& in, int , int )
{
    read(in, 'ltxt');
}

void Items::load(iStream& in, int , int )
{
    read(in, 'ltxt');
}

void ItemSets::read(iStream& in, DWORD fmt)
{
    if (fmt == 'ltxt')
    {
        if (m_buffer2)
        {
            delete[] m_buffer2;
            m_buffer2 = 0;
        }
        m_buffer2 = (wchar_t*)in.readAlloc(128);
        if (in.readDword() != 15)
            throw L"Invalid or not supported DB format";
        Serializable::checkSignature(in, L"id");
        Serializable::checkSignature(in, L"name");
        String setEffect(L"seteff");
        for (int i = 1; i <= 13; ++i)
            Serializable::checkSignature(in, (setEffect + i).m_buffer);
        int n = in.readDword();
        if (n > 0)
        {
            do
            {
                int id = (int)in.readInt64();
                ItemSet* found = 0;
                for (int i = 0; i < m_count; ++i)
                    if (m_items[i]->m_index == id)
                    {
                        found = m_items[i];
                        break;
                    }
                if (found)
                    found->read(in, 'ltxt');
                else
                {
                    ItemSet skip(0, 0);
                    skip.read(in, 'ltxt');
                }
            } while (--n);
        }
        return;
    }
    if (m_buffer1)
    {
        delete[] m_buffer1;
        m_buffer1 = 0;
    }
    deleteAll();
    if (fmt == 'flnt')
    {
        m_buffer1 = (wchar_t*)in.readAlloc(128);
        if (in.readDword() != 27)
            throw L"Invalid or not supported DB format";
        Serializable::checkSignature(in, L"id");
        String category(L"category");
        String number(L"number");
        for (int j = 1; j <= 13; ++j)
        {
            Serializable::checkSignature(in, (category + j).m_buffer);
            Serializable::checkSignature(in, (number + j).m_buffer);
        }
    }
    int n = in.readInt();
    for (int k = 1; k <= n; ++k)
    {
        ItemSet* set = new ItemSet((short)k, 0);
        add(set);
        set->read(in, fmt);
    }
}

void ItemSets::write(oStream& out, DWORD fmt) const
{
    if (fmt == 'ltxt')
    {
        out.put(m_buffer2, 128);
        out << (unsigned long)15;
        out << L"id";
        out << L"name";
        String setEffect(L"seteff");
        for (int i = 1; i <= 13; ++i)
            out << (const wchar_t*)(setEffect + i).m_buffer;
    }
    else if (fmt == 'flnt')
    {
        out.put(m_buffer1, 128);
        out << (unsigned long)27;
        out << L"id";
        String category(L"category");
        String number(L"number");
        for (int j = 1; j <= 13; ++j)
        {
            out << (const wchar_t*)(category + j).m_buffer;
            out << (const wchar_t*)(number + j).m_buffer;
        }
    }
    out << (unsigned long)m_count;
    for (int i = 0; i < m_count; ++i)
        m_items[i]->write(out, fmt);
}

bool ItemSets::exportText(String& out, DWORD fmt)
{
    if (fmt != '----')
        return false;
    out << (long)m_count << L"\r\n";
    for (int i = 0; i < m_count; ++i)
    {
        m_items[i]->exportText(out, '----');
        out.newline();
    }
    return true;
}

}
