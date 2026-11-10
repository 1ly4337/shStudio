#include "Portal.h"
#include "String.hpp"
#include "iStream.hpp"
#include "oStream.hpp"
#include "w3_util.h"

Portal::Portal(int id, int )
    : Spot(id)
{
    m_z = 0;
    m_y = 0;
    m_x = 0;
    m_faction = 0;
    m_levelMin = 0;
    m_levelMax = 99;
    m_destMap = 0;
    m_destZ = 0;
    m_destY = 0;
    m_destX = 0;
}

void Portal::read(iStream& s, DWORD )
{
    m_x = s.readFloat();
    m_y = s.readFloat();
    m_z = s.readFloat();
    m_faction = s.readInt();
    m_levelMin = s.readShort();
    m_levelMax = s.readShort();
    m_destMap = s.readInt();
    m_destX = s.readFloat();
    m_destY = s.readFloat();
    m_destZ = s.readFloat();
}

void Portal::write(oStream& s, DWORD ) const
{
    s << m_x << m_y << m_z << (unsigned long)m_faction
      << (unsigned short)m_levelMin << (unsigned short)m_levelMax
      << (unsigned long)m_destMap << m_destX << m_destY << m_destZ;
}

int Portal::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 0x696E6478:
        return m_id;
    case 0x66616374:
        return m_faction;
    case 0x6C6D696E:
        return m_levelMin;
    case 0x6C6D6178:
        return m_levelMax;
    case 0x64657374:
        return m_destMap;
    case 0x70737832:
        return *(const int*)&m_destX;
    case 0x70737932:
        return *(const int*)&m_destY;
    case 0x70737A32:
        return *(const int*)&m_destZ;
    }
    return Spot::getInt(tag);
}

void Portal::reset()
{
    m_z = 0;
    m_y = 0;
    m_x = 0;
    m_faction = 0;
    m_levelMin = 0;
    m_levelMax = 99;
    m_destMap = 0;
    m_destZ = 0;
    m_destY = 0;
    m_destX = 0;
}

bool Portal::copyFrom(const Streamable* src)
{
    return copyFrom(dynamic_cast<const Portal*>(src));
}

bool Portal::copyFrom(const Portal* other)
{
    if (!other)
        return false;
    m_x = other->m_x;
    m_y = other->m_y;
    m_z = other->m_z;
    m_faction = other->m_faction;
    m_levelMin = other->m_levelMin;
    m_levelMax = other->m_levelMax;
    m_destMap = other->m_destMap;
    m_destX = other->m_destX;
    m_destY = other->m_destY;
    m_destZ = other->m_destZ;
    return true;
}

bool Portal::toText(String& out, DWORD fmt) const
{
    wchar_t sep = w3_separatorFor(fmt);
    String& a = out << m_x;
    a << sep;
    String& b = a << m_y;
    b << sep;
    String& c = b << m_z;
    c << sep;
    String& d = c << (long)m_faction;
    d << sep;
    String& e = d << m_levelMin;
    e << sep;
    String& f = e << m_levelMax;
    f << sep;
    String& g = f << (long)m_destMap;
    g << sep;
    String& h = g << m_destX;
    h << sep;
    String& k = h << m_destY;
    k << sep;
    k << m_destZ;
    return true;
}
