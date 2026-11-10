#include "Spawn.h"
#include "String.hpp"
#include "iStream.hpp"
#include "oStream.hpp"
#include "w3_util.h"

Spawn::Spawn(int id, int )
    : Spot(id)
{
    m_unk28 = 0;
    m_faction = 0;
    m_unk20 = 0;
    m_z2 = 0;
    m_y2 = 0;
    m_x2 = 0;
}

int Spawn::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 0x70737832:
        return *(const int*)&m_x2;
    case 0x70737932:
        return *(const int*)&m_y2;
    case 0x70737A32:
        return *(const int*)&m_z2;
    case 0x66616374:
        return m_faction;
    }
    return Spot::getInt(tag);
}

int Spawn::compare(const Node* other, DWORD tag) const
{
    const Spawn& o = dynamic_cast<const Spawn&>(*other);
    if (tag != 0x66616374 )
        return Spot::compare(other, tag);
    if (m_faction != o.m_faction)
        return 2 * (m_faction >= o.m_faction) - 1;
    return 0;
}

void Spawn::read(iStream& s, DWORD )
{
    m_unk20 = s.readInt();
    m_faction = s.readInt();
    m_unk28 = s.readInt();
    m_x = s.readFloat();
    m_y = s.readFloat();
    m_z = s.readFloat();
    m_x2 = s.readFloat();
    m_y2 = s.readFloat();
    m_z2 = s.readFloat();
}

void Spawn::write(oStream& s, DWORD ) const
{
    s << (unsigned long)m_unk20 << (unsigned long)m_faction << (unsigned long)m_unk28
      << m_x << m_y << m_z << m_x2 << m_y2 << m_z2;
}

bool Spawn::toText(String& out, DWORD fmt) const
{
    wchar_t sep = w3_separatorFor(fmt);
    String& a = out << m_x;
    a << sep;
    String& b = a << m_y;
    b << sep;
    String& c = b << m_z;
    c << sep;
    String& d = c << m_x2;
    d << sep;
    String& e = d << m_y2;
    e << sep;
    String& f = e << m_z2;
    f << sep;
    String& g = f << (long)m_unk20;
    g << sep;
    String& h = g << (long)m_faction;
    h << sep;
    h << (long)m_unk28;
    return true;
}
