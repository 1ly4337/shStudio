#include "Spot.h"

int compareFloat(float a, float b)
{
    if (a == b)
        return 0;
    return 2 * (b <= a) - 1;
}

Spot::~Spot()
{
    if (m_hItem)
        SetWindowLongW(m_hItem, GWLP_USERDATA, 0);
}

void Spot::reset()
{
    m_z = 0;
    m_y = 0;
    m_x = 0;
}

void Spot::read(iStream& in, DWORD )
{
    m_x = in.readFloat();
    m_y = in.readFloat();
    m_z = in.readFloat();
}

void Spot::write(oStream& out, DWORD ) const
{
    out << m_x << m_y << m_z;
}

bool Spot::copyFrom(const Streamable* src)
{
    return copyPos(dynamic_cast<const Spot*>(src));
}

bool Spot::copyPos(const Spot* src)
{
    if (!src)
        return false;
    m_x = src->m_x;
    m_y = src->m_y;
    m_z = src->m_z;
    return true;
}

int Spot::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 'indx': return m_id;
    case 'posx': return *(const int*)&m_x;
    case 'posy': return *(const int*)&m_y;
    case 'posz': return *(const int*)&m_z;
    }
    return 0;
}

int Spot::compare(const Node* other, DWORD tag) const
{
    const Spot* p = dynamic_cast<const Spot*>(other);
    if (tag == 'indx')
    {
        if (m_id != p->m_id)
            return 2 * (m_id >= p->m_id) - 1;
        return 0;
    }
    float a, b;
    if (tag == 'posx')
    {
        b = p->m_x;
        a = m_x;
    }
    else if (tag == 'posy')
    {
        a = m_y;
        b = p->m_y;
    }
    else
    {
        if (tag != 'posz')
            return 0;
        a = m_z;
        b = p->m_z;
    }
    if (a != b)
        return 2 * (b <= a) - 1;
    return 0;
}

bool Spot::toText(String& out, DWORD fmt) const
{
    wchar_t sep;
    if ((int)fmt <= 'dump')
    {
        if (fmt == 'dump' || fmt == '----' || fmt == 'csv ')
            sep = L';';
        else
            sep = L',';
    }
    else if (fmt == 'sql ' || fmt != 'tabs')
        sep = L',';
    else
        sep = L'\t';
    out << m_x << sep << m_y << sep << m_z;
    return true;
}
