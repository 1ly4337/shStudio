#include "NamedArea.h"
#include "String.hpp"
#include "iStream.hpp"
#include "oStream.hpp"
#include "w3_util.h"

NamedArea::NamedArea(int id, int )
    : Spot(id)
{
    m_z2 = 0;
    m_y2 = 0;
    m_x2 = 0;
    m_unk2 = 0;
    m_unk1 = 0;
}

void NamedArea::read(iStream& s, DWORD )
{
    m_x = s.readFloat();
    m_y = s.readFloat();
    m_z = s.readFloat();
    m_x2 = s.readFloat();
    m_y2 = s.readFloat();
    m_z2 = s.readFloat();
    m_unk1 = s.readInt();
    m_unk2 = s.readInt();
}

void NamedArea::write(oStream& s, DWORD ) const
{
    s << m_x << m_y << m_z << m_x2 << m_y2 << m_z2 << (unsigned long)m_unk1 << (unsigned long)m_unk2;
}

int NamedArea::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 0x756E6B31:
        return m_unk1;
    case 0x756E6B32:
        return m_unk2;
    case 0x70737832:
        return *(const int*)&m_x2;
    case 0x70737932:
        return *(const int*)&m_y2;
    case 0x70737A32:
        return *(const int*)&m_z2;
    }
    return Spot::getInt(tag);
}

bool NamedArea::toText(String& out, DWORD fmt) const
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
    String& g = f << (long)m_unk1;
    g << sep;
    g << (long)m_unk2;
    return true;
}
