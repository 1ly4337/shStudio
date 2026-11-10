#include "DualLayerModel.h"

DualLayerModel::DualLayerModel(int version, int )
{
    m_index = 0;
    m_top = 0;
    m_version = version;
    m_shoe = 0;
    m_empty = 0;
    m_helmet = 0;
    m_bottom = 0;
    m_hand = 0;
}

bool DualLayerModel::copyFrom(const Streamable* src)
{
    const DualLayerModel* p = dynamic_cast<const DualLayerModel*>(src);
    if (!p)
        return false;
    copyParts(p);
    return true;
}

void DualLayerModel::copyParts(const DualLayerModel* src)
{
    m_top = src->m_top;
    m_hand = src->m_hand;
    m_bottom = src->m_bottom;
    m_shoe = src->m_shoe;
    m_empty = src->m_empty;
    m_helmet = src->m_helmet;
}

int DualLayerModel::getInt(DWORD tag) const
{
    if (tag <= 'hlmt')
    {
        switch (tag)
        {
        case 'hlmt': return m_helmet;
        case 'bttm': return m_bottom;
        case 'empt': return m_empty;
        case 'hand': return m_hand;
        }
        return 0;
    }
    if (tag == 'indx')
        return m_index;
    if (tag == 'shoe')
        return m_shoe;
    if (tag != 'top ')
        return 0;
    return m_top;
}

bool DualLayerModel::setInt(DWORD tag, int value)
{
    switch (tag)
    {
    case 'indx': m_index = (short)value; return true;
    case 'top ': m_top = (short)value; return true;
    case 'hand': m_hand = (short)value; return true;
    case 'bttm': m_bottom = (short)value; return true;
    case 'shoe': m_shoe = (short)value; return true;
    case 'empt': m_empty = (short)value; return true;
    case 'hlmt': m_helmet = (short)value; return true;
    default: return false;
    }
}

void DualLayerModel::reset()
{
    m_top = 0;
    m_hand = 0;
    m_bottom = 0;
    m_shoe = 0;
    m_empty = 0;
    m_helmet = 0;
}

void DualLayerModel::read(iStream& in, DWORD format)
{
    if (format == 'file')
    {
        m_index = in.readShort();
        m_top = in.readShort();
        m_hand = in.readShort();
        m_bottom = in.readShort();
        m_shoe = in.readShort();
        m_empty = in.readShort();
        m_helmet = in.readShort();
    }
    else if (format == 'flnt')
    {
        m_index = (short)in.readInt64();
        m_top = (short)in.readInt64();
        m_hand = (short)in.readInt64();
        m_bottom = (short)in.readInt64();
        m_shoe = (short)in.readInt64();
        m_empty = (short)in.readInt64();
        m_helmet = (short)in.readInt64();
    }
}

void DualLayerModel::write(oStream& out, DWORD format) const
{
    if (format == 'file')
    {
        out << (unsigned short)m_index << (unsigned short)m_top << (unsigned short)m_hand << (unsigned short)m_bottom
            << (unsigned short)m_shoe << (unsigned short)m_empty << (unsigned short)m_helmet;
    }
    else if (format == 'flnt')
    {
        __int64 helmet = m_helmet, empty = m_empty, shoe = m_shoe, bottom = m_bottom, hand = m_hand, top = m_top;
        out << (__int64)m_index;
        out << top;
        out << hand;
        out << bottom;
        out << shoe;
        out << empty;
        out << helmet;
    }
}

bool DualLayerModel::toText(String& out, DWORD format) const
{
    wchar_t sep;
    if ((int)format <= 'dump')
    {
        if (format == 'dump' || format == '----' || format == 'csv ')
            sep = L';';
        else
            sep = L',';
    }
    else if (format == 'sql ' || format != 'tabs')
        sep = L',';
    else
        sep = L'\t';
    if (format != '----')
        return false;
    out << m_index << sep << m_top << sep << m_hand << sep << m_bottom << sep << m_shoe << sep
        << m_empty << sep << m_helmet << L"\r\n";
    return true;
}

int DualLayerModel::compare(const Node* other, DWORD tag) const
{
    const DualLayerModel* p = dynamic_cast<const DualLayerModel*>(other);
    short a, b;
    switch (tag)
    {
    case 'indx': b = p->m_index; a = m_index; break;
    case 'top ': b = p->m_top; a = m_top; break;
    case 'hand': b = p->m_hand; a = m_hand; break;
    case 'bttm': b = p->m_bottom; a = m_bottom; break;
    case 'shoe': b = p->m_shoe; a = m_shoe; break;
    case 'empt': b = p->m_empty; a = m_empty; break;
    case 'hlmt': b = p->m_helmet; a = m_helmet; break;
    default: return 0;
    }
    if (a != b)
        return 2 * (a >= b) - 1;
    return 0;
}
