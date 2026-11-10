#include "Data_ItemSet.h"

namespace Data {

ItemSet::ItemSet(short index, int )
{
    m_index = index;
    reset();
}

ItemSet::~ItemSet()
{
}

int ItemSet::nodeType() const
{
    return 'iset';
}

bool ItemSet::copyFrom(const Streamable* src)
{
    const ItemSet* p = dynamic_cast<const ItemSet*>(src);
    if (p)
        return assign(p);
    return false;
}

void ItemSet::reset()
{
    m_name.clear();
    for (int i = 0; i < 13; ++i)
    {
        m_types[i] = 0;
        m_ids[i] = 0;
        m_bonus[i].clear();
    }
}

const wchar_t* ItemSet::getString(DWORD tag) const
{
    switch (tag)
    {
    case 'name': return m_name.m_buffer;
    case 'bn01': return m_bonus[0].m_buffer;
    case 'bn02': return m_bonus[1].m_buffer;
    case 'bn03': return m_bonus[2].m_buffer;
    case 'bn04': return m_bonus[3].m_buffer;
    case 'bn05': return m_bonus[4].m_buffer;
    case 'bn06': return m_bonus[5].m_buffer;
    case 'bn07': return m_bonus[6].m_buffer;
    case 'bn08': return m_bonus[7].m_buffer;
    case 'bn09': return m_bonus[8].m_buffer;
    case 'bn10': return m_bonus[9].m_buffer;
    case 'bn11': return m_bonus[10].m_buffer;
    case 'bn12': return m_bonus[11].m_buffer;
    case 'bn13': return m_bonus[12].m_buffer;
    }
    return 0;
}

int ItemSet::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 'indx': return m_index;
    case 'tp01': return m_types[0];
    case 'tp02': return m_types[1];
    case 'tp03': return m_types[2];
    case 'tp04': return m_types[3];
    case 'tp05': return m_types[4];
    case 'tp06': return m_types[5];
    case 'tp07': return m_types[6];
    case 'tp08': return m_types[7];
    case 'tp09': return m_types[8];
    case 'tp10': return m_types[9];
    case 'tp11': return m_types[10];
    case 'tp12': return m_types[11];
    case 'tp13': return m_types[12];
    case 'id01': return m_ids[0];
    case 'id02': return m_ids[1];
    case 'id03': return m_ids[2];
    case 'id04': return m_ids[3];
    case 'id05': return m_ids[4];
    case 'id06': return m_ids[5];
    case 'id07': return m_ids[6];
    case 'id08': return m_ids[7];
    case 'id09': return m_ids[8];
    case 'id10': return m_ids[9];
    case 'id11': return m_ids[10];
    case 'id12': return m_ids[11];
    case 'id13': return m_ids[12];
    }
    return 0;
}

bool ItemSet::setInt(DWORD tag, int value)
{
    switch (tag)
    {
    case 'tp01': m_types[0] = (short)value; return true;
    case 'tp02': m_types[1] = (short)value; return true;
    case 'tp03': m_types[2] = (short)value; return true;
    case 'tp04': m_types[3] = (short)value; return true;
    case 'tp05': m_types[4] = (short)value; return true;
    case 'tp06': m_types[5] = (short)value; return true;
    case 'tp07': m_types[6] = (short)value; return true;
    case 'tp08': m_types[7] = (short)value; return true;
    case 'tp09': m_types[8] = (short)value; return true;
    case 'tp10': m_types[9] = (short)value; return true;
    case 'tp11': m_types[10] = (short)value; return true;
    case 'tp12': m_types[11] = (short)value; return true;
    case 'tp13': m_types[12] = (short)value; return true;
    case 'id01': m_ids[0] = (short)value; return true;
    case 'id02': m_ids[1] = (short)value; return true;
    case 'id03': m_ids[2] = (short)value; return true;
    case 'id04': m_ids[3] = (short)value; return true;
    case 'id05': m_ids[4] = (short)value; return true;
    case 'id06': m_ids[5] = (short)value; return true;
    case 'id07': m_ids[6] = (short)value; return true;
    case 'id08': m_ids[7] = (short)value; return true;
    case 'id09': m_ids[8] = (short)value; return true;
    case 'id10': m_ids[9] = (short)value; return true;
    case 'id11': m_ids[10] = (short)value; return true;
    case 'id12': m_ids[11] = (short)value; return true;
    case 'id13': m_ids[12] = (short)value; return true;
    default:
        return false;
    }
}

bool ItemSet::setString(DWORD tag, const wchar_t* value)
{
    switch (tag)
    {
    case 'name': m_name = value; return true;
    case 'bn01': m_bonus[0] = value; return true;
    case 'bn02': m_bonus[1] = value; return true;
    case 'bn03': m_bonus[2] = value; return true;
    case 'bn04': m_bonus[3] = value; return true;
    case 'bn05': m_bonus[4] = value; return true;
    case 'bn06': m_bonus[5] = value; return true;
    case 'bn07': m_bonus[6] = value; return true;
    case 'bn08': m_bonus[7] = value; return true;
    case 'bn09': m_bonus[8] = value; return true;
    case 'bn10': m_bonus[9] = value; return true;
    case 'bn11': m_bonus[10] = value; return true;
    case 'bn12': m_bonus[11] = value; return true;
    case 'bn13': m_bonus[12] = value; return true;
    default:
        return false;
    }
}

int ItemSet::compare(const Node* other, DWORD tag) const
{
    const ItemSet* p = dynamic_cast<const ItemSet*>(other);
    if (tag == 'indx')
    {
        if (m_index != p->m_index)
            return 2 * (m_index >= p->m_index) - 1;
        return 0;
    }
    if (tag == 'name')
        return String::compareLogical(m_name, p->m_name);
    return 0;
}

bool ItemSet::assign(const ItemSet* src)
{
    if (&src->m_name != &m_name)
    {
        m_name.clear();
        m_name.append(src->m_name.m_buffer, src->m_name.m_length);
    }
    for (int i = 0; i < 13; ++i)
    {
        m_types[i] = src->m_types[i];
        m_ids[i] = src->m_ids[i];
        if (&src->m_bonus[i] != &m_bonus[i])
        {
            m_bonus[i].clear();
            m_bonus[i].append(src->m_bonus[i].m_buffer, src->m_bonus[i].m_length);
        }
    }
    return true;
}

bool ItemSet::exportText(String& out, DWORD fmt) const
{
    if (fmt != '----')
        return false;
    (out << m_index << L';').append(m_name.m_buffer, m_name.m_length);
    for (int i = 0; i < 13; ++i)
    {
        out << L';';
        String& s = out << m_types[i] << L';' << m_ids[i] << L';';
        s.append(m_bonus[i].m_buffer, m_bonus[i].m_length);
    }
    return true;
}

void ItemSet::read(iStream& in, DWORD fmt)
{
    switch (fmt)
    {
    case 'file':
    {
        m_index = in.readShort();
        m_name.read(in, 4, in.m_codePage);
        for (int i = 0; i < 13; ++i)
        {
            m_types[i] = in.readShort();
            m_ids[i] = in.readShort();
        }
        String text;
        for (int i = 0; i < 13; ++i)
        {
            text.read(in, 4, in.m_codePage);
            String crlf = text.lfToCrLf();
            if (&crlf != &m_bonus[i])
            {
                m_bonus[i].clear();
                m_bonus[i].append(crlf.m_buffer, crlf.m_length);
            }
        }
        break;
    }
    case 'flnt':
        m_index = (short)in.readInt64();
        for (int i = 0; i < 13; ++i)
        {
            m_types[i] = (short)in.readInt64();
            m_ids[i] = (short)in.readInt64();
        }
        break;
    case 'ltxt':
    {
        m_name.read(in, 4, (UINT)-1);
        String text;
        for (int i = 0; i < 13; ++i)
        {
            text.read(in, 4, (UINT)-1);
            String crlf = text.lfToCrLf();
            if (&crlf != &m_bonus[i])
            {
                m_bonus[i].clear();
                m_bonus[i].append(crlf.m_buffer, crlf.m_length);
            }
        }
        break;
    }
    }
}

void ItemSet::write(oStream& out, DWORD fmt) const
{
    switch (fmt)
    {
    case 'file':
    {
        oStream& o = out << (unsigned short)m_index;
        m_name.write(o, 4, o.m_codePage);
        for (int i = 0; i < 13; ++i)
            out << (unsigned short)m_types[i] << (unsigned short)m_ids[i];
        for (int i = 0; i < 13; ++i)
            m_bonus[i].crLfToLf().write(out, 4, out.m_codePage);
        break;
    }
    case 'flnt':
        out << (__int64)m_index;
        for (int i = 0; i < 13; ++i)
        {
            short id = m_ids[i];
            out << (__int64)m_types[i];
            out << (__int64)id;
        }
        break;
    case 'ltxt':
        out << (__int64)m_index;
        m_name.write(out, 4, (UINT)-1);
        for (int i = 0; i < 13; ++i)
            m_bonus[i].crLfToLf().write(out, 4, (UINT)-1);
        break;
    }
}

}
