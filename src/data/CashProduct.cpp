#include "CashProduct.h"
#include "RecordSet.h"

CashProduct::CashProduct(int version, int row, int )
{
    m_version = version;
    m_row = row;
    m_bag = 1;
    reset();
}

CashProduct::~CashProduct()
{
}

int CashProduct::getVersion() const
{
    return 513;
}

bool CashProduct::copyFrom(const Streamable* src)
{
    const CashProduct* p = dynamic_cast<const CashProduct*>(src);
    if (!p)
        return false;
    *this = *p;
    return true;
}

CashProduct& CashProduct::operator=(const CashProduct& src)
{
    m_row = src.m_row;
    m_bag = src.m_bag;
    m_icon = src.m_icon;
    m_cost = src.m_cost;
    m_cost2 = src.m_cost2;
    if (&src.m_name != &m_name)
    {
        m_name.clear();
        m_name.append(src.m_name.m_buffer, src.m_name.m_length);
    }
    if (&src.m_code != &m_code)
    {
        m_code.clear();
        m_code.append(src.m_code.m_buffer, src.m_code.m_length);
    }
    if (&src.m_desc != &m_desc)
    {
        m_desc.clear();
        m_desc.append(src.m_desc.m_buffer, src.m_desc.m_length);
    }
    if (&src.m_title != &m_title)
    {
        m_title.clear();
        m_title.append(src.m_title.m_buffer, src.m_title.m_length);
    }
    m_itemTotal = src.m_itemTotal;
    for (int i = 0; i < 24; ++i)
    {
        m_itemId[i] = src.m_itemId[i];
        m_itemCount[i] = src.m_itemCount[i];
    }
    return *this;
}

int CashProduct::getId() const
{
    return m_icon;
}

const wchar_t* CashProduct::getString(DWORD tag) const
{
    switch (tag)
    {
    case 'name': return m_name.m_buffer;
    case 'code': return m_code.m_buffer;
    case 'desc': return m_desc.m_buffer;
    case 'ttle': return m_title.m_buffer;
    }
    return L"";
}

int CashProduct::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 'row ': return m_row;
    case 'type': return m_bag;
    case 'icon': return m_icon;
    case 'cost': return m_cost;
    case 'cst2': return m_cost2;
    case 'it01': return m_itemId[0];
    case 'it02': return m_itemId[1];
    case 'it03': return m_itemId[2];
    case 'it04': return m_itemId[3];
    case 'it05': return m_itemId[4];
    case 'it06': return m_itemId[5];
    case 'it07': return m_itemId[6];
    case 'it08': return m_itemId[7];
    case 'it09': return m_itemId[8];
    case 'it10': return m_itemId[9];
    case 'it11': return m_itemId[10];
    case 'it12': return m_itemId[11];
    case 'it13': return m_itemId[12];
    case 'it14': return m_itemId[13];
    case 'it15': return m_itemId[14];
    case 'it16': return m_itemId[15];
    case 'it17': return m_itemId[16];
    case 'it18': return m_itemId[17];
    case 'it19': return m_itemId[18];
    case 'it20': return m_itemId[19];
    case 'it21': return m_itemId[20];
    case 'it22': return m_itemId[21];
    case 'it23': return m_itemId[22];
    case 'it24': return m_itemId[23];
    case 'ic01': return m_itemCount[0];
    case 'ic02': return m_itemCount[1];
    case 'ic03': return m_itemCount[2];
    case 'ic04': return m_itemCount[3];
    case 'ic05': return m_itemCount[4];
    case 'ic06': return m_itemCount[5];
    case 'ic07': return m_itemCount[6];
    case 'ic08': return m_itemCount[7];
    case 'ic09': return m_itemCount[8];
    case 'ic10': return m_itemCount[9];
    case 'ic11': return m_itemCount[10];
    case 'ic12': return m_itemCount[11];
    case 'ic13': return m_itemCount[12];
    case 'ic14': return m_itemCount[13];
    case 'ic15': return m_itemCount[14];
    case 'ic16': return m_itemCount[15];
    case 'ic17': return m_itemCount[16];
    case 'ic18': return m_itemCount[17];
    case 'ic19': return m_itemCount[18];
    case 'ic20': return m_itemCount[19];
    case 'ic21': return m_itemCount[20];
    case 'ic22': return m_itemCount[21];
    case 'ic23': return m_itemCount[22];
    case 'ic24': return m_itemCount[23];
    case '#cnt': return m_itemTotal;
    }
    return 0;
}

bool CashProduct::setString(DWORD tag, const wchar_t* text)
{
    if (tag == 'name')
    {
        m_name = text;
        m_title.clear();
        m_title.append(m_name.m_buffer, m_name.m_length) << L" - " << (long)m_cost << L" Pts";
        return true;
    }
    if (tag == 'code')
    {
        m_code = text;
        return true;
    }
    if (tag == 'desc')
    {
        m_desc = text;
        return true;
    }
    return false;
}

bool CashProduct::setInt(DWORD tag, int value)
{
    switch (tag)
    {
    case 'row ': m_row = value; return true;
    case 'type': m_bag = value; return true;
    case 'icon': m_icon = value; return true;
    case 'cost':
        m_cost = value;
        m_title.clear();
        m_title.append(m_name.m_buffer, m_name.m_length) << L" - " << (long)m_cost << L" Pts";
        return true;
    case 'cst2': m_cost2 = value; return true;
    case 'it01': m_itemId[0] = value; return true;
    case 'it02': m_itemId[1] = value; return true;
    case 'it03': m_itemId[2] = value; return true;
    case 'it04': m_itemId[3] = value; return true;
    case 'it05': m_itemId[4] = value; return true;
    case 'it06': m_itemId[5] = value; return true;
    case 'it07': m_itemId[6] = value; return true;
    case 'it08': m_itemId[7] = value; return true;
    case 'it09': m_itemId[8] = value; return true;
    case 'it10': m_itemId[9] = value; return true;
    case 'it11': m_itemId[10] = value; return true;
    case 'it12': m_itemId[11] = value; return true;
    case 'it13': m_itemId[12] = value; return true;
    case 'it14': m_itemId[13] = value; return true;
    case 'it15': m_itemId[14] = value; return true;
    case 'it16': m_itemId[15] = value; return true;
    case 'it17': m_itemId[16] = value; return true;
    case 'it18': m_itemId[17] = value; return true;
    case 'it19': m_itemId[18] = value; return true;
    case 'it20': m_itemId[19] = value; return true;
    case 'it21': m_itemId[20] = value; return true;
    case 'it22': m_itemId[21] = value; return true;
    case 'it23': m_itemId[22] = value; return true;
    case 'it24': m_itemId[23] = value; return true;
    case 'ic01': m_itemCount[0] = (BYTE)value; return true;
    case 'ic02': m_itemCount[1] = (BYTE)value; return true;
    case 'ic03': m_itemCount[2] = (BYTE)value; return true;
    case 'ic04': m_itemCount[3] = (BYTE)value; return true;
    case 'ic05': m_itemCount[4] = (BYTE)value; return true;
    case 'ic06': m_itemCount[5] = (BYTE)value; return true;
    case 'ic07': m_itemCount[6] = (BYTE)value; return true;
    case 'ic08': m_itemCount[7] = (BYTE)value; return true;
    case 'ic09': m_itemCount[8] = (BYTE)value; return true;
    case 'ic10': m_itemCount[9] = (BYTE)value; return true;
    case 'ic11': m_itemCount[10] = (BYTE)value; return true;
    case 'ic12': m_itemCount[11] = (BYTE)value; return true;
    case 'ic13': m_itemCount[12] = (BYTE)value; return true;
    case 'ic14': m_itemCount[13] = (BYTE)value; return true;
    case 'ic15': m_itemCount[14] = (BYTE)value; return true;
    case 'ic16': m_itemCount[15] = (BYTE)value; return true;
    case 'ic17': m_itemCount[16] = (BYTE)value; return true;
    case 'ic18': m_itemCount[17] = (BYTE)value; return true;
    case 'ic19': m_itemCount[18] = (BYTE)value; return true;
    case 'ic20': m_itemCount[19] = (BYTE)value; return true;
    case 'ic21': m_itemCount[20] = (BYTE)value; return true;
    case 'ic22': m_itemCount[21] = (BYTE)value; return true;
    case 'ic23': m_itemCount[22] = (BYTE)value; return true;
    case 'ic24': m_itemCount[23] = (BYTE)value; return true;
    }
    return false;
}

void CashProduct::reset()
{
    m_bag = 1;
    m_icon = 0;
    m_cost = 0;
    m_cost2 = 0;
    for (int i = 0; i < 24; ++i)
    {
        m_itemId[i] = 0;
        m_itemCount[i] = 0;
    }
    m_itemTotal = 0;
}

void CashProduct::read(iStream& in, DWORD format)
{
    switch (format)
    {
    case 'file':
    {
        m_itemTotal = 0;
        m_row = in.readInt();
        m_bag = in.readInt();
        m_icon = in.readInt();
        m_cost = in.readInt();
        for (int i = 0; i < 24; ++i)
        {
            m_itemId[i] = in.readInt();
            m_itemCount[i] = (BYTE)in.readByte();
            if (m_itemId[i])
                ++m_itemTotal;
        }
        m_name.read(in, 4, in.m_codePage);
        m_code.read(in, 4, in.m_codePage);
        m_desc.read(in, 4, in.m_codePage);
        m_title.clear();
        m_title.append(m_name.m_buffer, m_name.m_length) << L" - " << (long)m_cost << L" Pts";
        m_desc = m_desc.unescape();
        break;
    }
    case 'flnt':
    {
        m_itemTotal = 0;
        m_code.read(in, 1, 0);
        m_row = (int)in.readInt64();
        m_cost = (int)in.readInt64();
        if ((m_version & 0xFFF) >= 0x700)
            m_cost2 = (int)in.readInt64();
        for (int i = 0; i < 24; ++i)
        {
            m_itemId[i] = (int)in.readInt64();
            m_itemCount[i] = (BYTE)in.readInt64();
            if (m_itemId[i])
                ++m_itemTotal;
        }
        m_bag = (int)in.readInt64();
        m_icon = (int)in.readInt64();
        break;
    }
    case 'ltxt':
    {
        m_name.read(in, 4, (UINT)-1);
        m_title.clear();
        m_title.append(m_name.m_buffer, m_name.m_length) << L" - " << (long)m_cost << L" Pts";
        String text;
        text.read(in, 4, (UINT)-1);
        m_desc = text.unescape();
        break;
    }
    }
}

void CashProduct::write(oStream& out, DWORD format) const
{
    switch (format)
    {
    case 'file':
    {
        out << (unsigned long)m_row << (unsigned long)m_bag << (unsigned long)m_icon << (unsigned long)m_cost;
        for (int i = 0; i < 24; ++i)
        {
            BYTE count = m_itemCount[i];
            out << (unsigned long)m_itemId[i];
            out << (char)count;
        }
        (m_name + L'\0').write(out, 4, out.m_codePage);
        (m_code + L'\0').write(out, 4, out.m_codePage);
        (m_desc.escape() + L'\0').write(out, 4, out.m_codePage);
        break;
    }
    case 'flnt':
    {
        m_code.write(out, 1, 0);
        int cost = m_cost;
        out << (__int64)m_row;
        out << (__int64)cost;
        if ((m_version & 0xFFF) >= 0x700)
            out << (__int64)m_cost2;
        for (int i = 0; i < 24; ++i)
        {
            int count = m_itemCount[i];
            out << (__int64)m_itemId[i];
            out << (__int64)count;
        }
        int icon = m_icon;
        out << (__int64)m_bag;
        out << (__int64)icon;
        break;
    }
    case 'ltxt':
    {
        out << (__int64)m_row;
        m_name.write(out, 4, (UINT)-1);
        m_desc.escape().write(out, 4, (UINT)-1);
        break;
    }
    }
}

bool CashProduct::loadRecord(RecordSet& rs, bool full)
{
    try
    {
        if (full)
            m_name = rs.getString(L"ProductName");
        m_code = rs.getString(L"ProductCode");
        m_itemTotal = 0;
        m_cost = rs.getInt(L"BuyCost");
        wchar_t column[32] = { 0 };
        for (int i = 0; i < 24; ++i)
        {
            swprintf_s(column, 32, L"ItemID%i", i + 1);
            m_itemId[i] = rs.getInt(column);
            swprintf_s(column, 32, L"ItemCount%i", i + 1);
            m_itemCount[i] = (BYTE)rs.getInt(column);
            if (m_itemId[i])
                ++m_itemTotal;
        }
        if (full)
        {
            m_title.clear();
            m_title.append(m_name.m_buffer, m_name.m_length) << L" - " << (long)m_cost << L" Pts";
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

int CashProduct::compare(const Node* other, DWORD tag) const
{
    const CashProduct* p = dynamic_cast<const CashProduct*>(other);
    switch (tag)
    {
    case 'row ':
        if (m_row != p->m_row) return 2 * (m_row >= p->m_row) - 1;
        return 0;
    case 'type':
        if (m_bag != p->m_bag) return 2 * (m_bag >= p->m_bag) - 1;
        return 0;
    case 'icon':
        if (m_icon != p->m_icon) return 2 * (m_icon >= p->m_icon) - 1;
        return 0;
    case 'cost':
        if (m_cost != p->m_cost) return 2 * (m_cost >= p->m_cost) - 1;
        return 0;
    case 'name':
        return String::compareLogical(m_name, p->m_name);
    case 'code':
        return String::compareLogical(m_code, p->m_code);
    case 'desc':
        return String::compareLogical(m_desc, p->m_desc);
    case '#cnt':
        if (m_itemTotal != p->m_itemTotal) return 2 * (m_itemTotal >= p->m_itemTotal) - 1;
        return 0;
    default:
        return 0;
    }
}

bool CashProduct::toText(String& out, DWORD format) const
{
    if (format == 'sql ')
    {
        String columns(L"[ProductName],[ProductCode],[BuyCost]");
        String values;
        {
            String code = String::sqlQuote(m_code.m_buffer);
            String name = String::sqlQuote(m_name.m_buffer);
            values.append(name.m_buffer, name.m_length) << L',';
            values.append(code.m_buffer, code.m_length) << L',';
            values << (long)m_cost;
        }
        for (int i = 1; i <= 24; ++i)
        {
            columns << L",[ItemID" << (long)i << L"],[ItemCount" << (long)i << L']';
            values << L',' << (long)m_itemId[i - 1] << L',' << (unsigned char)m_itemCount[i - 1];
        }
        out << L"INSERT INTO [PS_GameDefs].[dbo].[ProductList] (";
        out.append(columns.m_buffer, columns.m_length);
        out << L")) values (";
        out.append(values.m_buffer, values.m_length);
        out << L')';
        out.newline();
        return true;
    }
    if (format != 'csv ')
        return false;
    {
        String desc = m_desc.escape();
        String& s = out << (long)m_row << L';' << (long)m_bag << L';' << (long)m_icon << L';' << (long)m_cost << L';';
        s.append(m_code.m_buffer, m_code.m_length) << L';';
        s.append(m_name.m_buffer, m_name.m_length) << L';';
        s.append(desc.m_buffer, desc.m_length);
    }
    for (int i = 0; i < 24; ++i)
        out << L';' << (long)m_itemId[i] << L';' << (unsigned char)m_itemCount[i];
    out << L"\r\n";
    return true;
}
