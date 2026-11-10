#include "p02_common.h"
#include "CashProducts.h"
#include "CashProduct.h"

int CashProducts::sub_420770()
{
    vLong& bags = m_bags;
    for (int i = 0; i < bags.m_capacity; ++i)
        bags.resetItem(&bags.m_items[i]);
    bags.m_count = 0;
    for (int i = 0; i < m_count; ++i)
    {
        int n = bags.m_count;
        int k = 0;
        for (; k < n; ++k)
            if (bags.m_items[k] == m_items[i]->m_bag)
                break;
        if (k >= n)
            bags.add(m_items[i]->m_bag);
    }
    return m_bags.m_count;
}

CashProduct* CashProducts::sub_420800(wchar_t* code)
{
    for (int i = 0; i < m_count; ++i)
    {
        CashProduct* p = m_items[i];
        bool match;
        if (!code || !*code)
            match = p->m_code.m_length == 0;
        else if (wcslen(code) == p->m_code.m_length)
            match = _wcsnicmp(p->m_code.m_buffer, code, p->m_code.m_length) == 0;
        else
            continue;
        if (match)
            return m_items[i];
    }
    return 0;
}

void CashProducts::clear()
{
    if (m_buffer1)
    {
        delete[] m_buffer1;
        m_buffer1 = 0;
    }
    if (m_buffer2)
    {
        delete[] m_buffer2;
        m_buffer2 = 0;
    }
    deleteAll();
}

HRESULT CashProducts::setCodePage(int version)
{
    m_version = version;
    for (int i = 0; i < m_count; ++i)
        m_items[i]->m_version = m_version;
    return S_OK;
}

void CashProducts::read(iStream& in, DWORD fmt)
{
    if (fmt == 'ltxt')
    {
        if (m_buffer2)
        {
            delete[] m_buffer2;
            m_buffer2 = 0;
        }
        m_buffer2 = (wchar_t*)in.readAlloc(128);
        if (in.readDword() != 2)
            throw L"Invalid or not supported DB format";
        Serializable::checkSignature(in, L"ProductName");
        Serializable::checkSignature(in, L"text");
        int n = in.readDword();
        if (n > 0)
        {
            do
            {
                int row = (int)in.readInt64();
                CashProduct* p = 0;
                for (int i = 0; i < m_count; ++i)
                    if (m_items[i]->m_row == row)
                    {
                        p = m_items[i];
                        break;
                    }
                if (p)
                    p->read(in, 'ltxt');
                else
                {
                    String skip;
                    skip.read(in, 4, (UINT)-1);
                    skip.read(in, 4, (UINT)-1);
                }
            } while (n-- != 1);
        }
        return;
    }

    deleteAll();
    if (m_buffer1)
    {
        delete[] m_buffer1;
        m_buffer1 = 0;
    }
    if (fmt == 'flnt')
    {
        m_buffer1 = (wchar_t*)in.readAlloc(128);
        int dbVersion = in.readDword();
        if (dbVersion == 54)
            m_version = 0x1700;
        else if (dbVersion == 53)
            m_version = 0x1600;
        else
            throw L"Invalid or not supported DB format";
        Serializable::checkSignature(in, L"ProductCode");
        Serializable::checkSignature(in, L"goods_id");
        if (dbVersion == 53)
            Serializable::checkSignature(in, L"BuyCost");
        else
        {
            Serializable::checkSignature(in, L"Multi_BuyCost1");
            Serializable::checkSignature(in, L"Multi_BuyCost2");
        }
        String itemId(L"ItemID");
        String itemCount(L"ItemCount");
        for (int i = 1; i <= 24; ++i)
        {
            Serializable::checkSignature(in, (itemId + i).m_buffer);
            Serializable::checkSignature(in, (itemCount + i).m_buffer);
        }
        Serializable::checkSignature(in, L"type");
        Serializable::checkSignature(in, L"icon");
    }
    CashProduct* item = 0;
    try
    {
        int n = in.readDword();
        for (int i = 0; i < n; ++i)
        {
            item = new CashProduct(m_version, 0, 0);
            item->read(in, fmt);
            add(item);
        }
    }
    catch (...)
    {
        if (item)
            delete item;
        deleteAll();
        throw;
    }
}

void CashProducts::write(oStream& out, DWORD fmt) const
{
    out.wipe();
    switch (fmt)
    {
    case 'file':
        out << (unsigned long)m_count;
        for (int i = 0; i < m_count; ++i)
            m_items[i]->write(out, 'file');
        break;
    case 'flnt':
    {
        out.put(m_buffer1, 128);
        out << (unsigned long)((m_version & 0xFFF) < 0x700 ? 53 : 54);
        out << L"ProductCode";
        out << L"goods_id";
        if ((m_version & 0xFFF) < 0x700)
            out << L"BuyCost";
        else
        {
            out << L"Multi_BuyCost1";
            out << L"Multi_BuyCost2";
        }
        String itemId(L"ItemID");
        String itemCount(L"ItemCount");
        for (int i = 1; i <= 24; ++i)
        {
            out << (const wchar_t*)(itemId + i).m_buffer;
            out << (const wchar_t*)(itemCount + i).m_buffer;
        }
        out << L"type";
        out << L"icon";
        out << (unsigned long)m_count;
        for (int i = 0; i < m_count; ++i)
            m_items[i]->write(out, 'flnt');
        break;
    }
    case 'ltxt':
        out.put(m_buffer2, 128);
        out << (unsigned long)2;
        out << L"ProductName";
        out << L"text";
        out << (unsigned long)m_count;
        for (int i = 0; i < m_count; ++i)
            m_items[i]->write(out, 'ltxt');
        break;
    }
}

void CashProducts::load(iStream& in, int , int )
{
    read(in, 'ltxt');
}

bool CashProducts::toText(String& out, DWORD fmt) const
{
    if (fmt == 'csv ')
    {
        out << L"RowID;Bag;Icon;BuyCost;ProductCode;ProductName;ProductDesc";
        for (int i = 1; i <= 24; ++i)
            out << L";ItemID" << (long)i << L";ItemCount" << (long)i;
        out.newline();
        for (int i = 0; i < m_count; ++i)
            m_items[i]->toText(out, 'csv ');
        return true;
    }
    if (fmt == 'sql ')
        return toSql(out, fmt);
    return false;
}

bool CashProducts::toSql(String& out, DWORD ) const
{
    out.clear();
    out << L"TRUNCATE TABLE [PS_GameDefs].[dbo].[ProductList]" << L"\r\n";
    try
    {
        for (int i = 0; i < m_count; ++i)
            m_items[i]->toText(out, 'sql ');
    }
    catch (...)
    {
        return false;
    }
    return true;
}
