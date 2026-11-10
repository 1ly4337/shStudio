#include "NpcLocation.h"
#include "StandardNPC.h"
#include "String.hpp"
#include "iStream.hpp"
#include "oStream.hpp"
#include "w3_util.h"
#include <cwchar>

NpcLocation::Location::Location(int index)
{
    m_index = index;
    m_orient = 0;
    m_z = 0;
    m_y = 0;
    m_x = 0;
    m_unk10 = 0;
}

void NpcLocation::Location::reset()
{
    m_z = 0;
    m_y = 0;
    m_x = 0;
    m_orient = 0;
}

void NpcLocation::Location::read(iStream& s, DWORD )
{
    m_x = s.readFloat();
    m_y = s.readFloat();
    m_z = s.readFloat();
    m_orient = s.readFloat();
}

void NpcLocation::Location::write(oStream& s, DWORD ) const
{
    s << m_x << m_y << m_z << m_orient;
}

int NpcLocation::Location::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 0x696E6478:
        return m_index;
    case 0x706F7378:
        return *(const int*)&m_x;
    case 0x706F7379:
        return *(const int*)&m_y;
    case 0x706F737A:
        return *(const int*)&m_z;
    case 0x6F726E74:
        return *(const int*)&m_orient;
    }
    return 0;
}

int NpcLocation::Location::compare(const Node* other, DWORD tag) const
{
    const Location& o = dynamic_cast<const Location&>(*other);
    float mine, theirs;
    if (tag == 0x706F7378 )
    {
        theirs = o.m_x;
        mine = m_x;
    }
    else
    {
        switch (tag)
        {
        case 0x706F7379:
            mine = m_y;
            theirs = o.m_y;
            break;
        case 0x706F737A:
            mine = m_z;
            theirs = o.m_z;
            break;
        case 0x6F726E74:
            mine = m_orient;
            theirs = o.m_orient;
            break;
        default:
            return 0;
        }
    }
    if (mine != theirs)
        return 2 * (theirs <= mine) - 1;
    return 0;
}

bool NpcLocation::Location::copyFrom(const Streamable* src)
{
    return copyPos(dynamic_cast<const Location*>(src));
}

bool NpcLocation::Location::copyPos(const Location* other)
{
    if (!other)
        return false;
    m_x = other->m_x;
    m_y = other->m_y;
    m_z = other->m_z;
    m_orient = other->m_orient;
    return true;
}

bool NpcLocation::Location::toText(String& out, DWORD fmt) const
{
    wchar_t sep = w3_separatorFor(fmt);
    String& a = out << m_x;
    a << sep;
    String& b = a << m_y;
    b << sep;
    String& c = b << m_z;
    c << sep;
    c << m_orient;
    return true;
}

void NpcLocation::Locations::copyFrom(const Locations& src)
{
    deleteAll();
    for (int i = 0; i < src.m_count; ++i)
    {
        Location* n = new Location(src.m_items[i]);
        add(n);
    }
}

bool NpcLocation::Locations::toText(String& out, DWORD fmt) const
{
    wchar_t sep = w3_separatorFor(fmt);
    for (int i = 0; i < m_count; ++i)
    {
        out << sep;
        m_items[i]->toText(out, fmt);
        out.newline();
    }
    return true;
}

void NpcLocation::Locations::read(iStream& s, DWORD tag)
{
    deleteAll();
    Location* current = 0;
    try
    {
        int count = s.readInt();
        for (int i = 0; i < count; ++i)
        {
            Location* l = new Location(i);
            current = l;
            l->read(s, tag);
            add(l);
        }
    }
    catch (...)
    {
        if (current)
            delete current;
        deleteAll();
        throw;
    }
}

void NpcLocation::Locations::write(oStream& s, DWORD tag) const
{
    s << (unsigned long)m_count;
    for (int i = 0; i < m_count; ++i)
        m_items[i]->write(s, tag);
}

NpcLocation::NpcLocation(int id, int )
    : Spot(id), m_locations()
{
    m_type = 0;
    m_typeId = 0;
}

NpcLocation::~NpcLocation()
{
}

const wchar_t* NpcLocation::getString(DWORD tag) const
{
    if (tag == 0x66616D69 )
        return StandardNPC::typeName(m_type);
    return 0;
}

int NpcLocation::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 0x696E6478:
        return m_id;
    case 0x74797065:
        return m_type;
    case 0x74796964:
        return m_typeId;
    case 0x6D636E74:
        return m_locations.size();
    }
    return 0;
}

int NpcLocation::compare(const Node* other, DWORD tag) const
{
    const NpcLocation& o = dynamic_cast<const NpcLocation&>(*other);
    switch (tag)
    {
    case 0x696E6478:
        if (m_id != o.m_id)
            return 2 * (m_id >= o.m_id) - 1;
        return 0;
    case 0x66616D69:
    {
        const wchar_t* theirs = StandardNPC::typeName(o.m_type);
        const wchar_t* mine = StandardNPC::typeName(m_type);
        if (!mine)
            return -(theirs != 0);
        if (theirs)
            return _wcsicmp(mine, theirs);
        return 1;
    }
    case 0x74797065:
    {
        unsigned char theirs = (unsigned char)o.m_type;
        unsigned char mine = (unsigned char)m_type;
        if (mine != theirs)
            return mine < theirs ? -1 : 1;
        return 0;
    }
    case 0x74796964:
        if (m_typeId != o.m_typeId)
            return 2 * (m_typeId >= o.m_typeId) - 1;
        return 0;
    case 0x6D636E74:
    {
        int theirs = o.m_locations.size();
        int mine = m_locations.size();
        if (mine != theirs)
            return 2 * (mine >= theirs) - 1;
        return 0;
    }
    default:
        return 0;
    }
}

bool NpcLocation::copyFrom(const Streamable* src)
{
    return copyFrom(dynamic_cast<const NpcLocation*>(src));
}

bool NpcLocation::copyFrom(const NpcLocation* other)
{
    if (!other)
        return false;
    m_type = other->m_type;
    m_typeId = other->m_typeId;
    m_locations.copyFrom(other->m_locations);
    return true;
}

bool NpcLocation::toText(String& out, DWORD fmt) const
{
    wchar_t sep = w3_separatorFor(fmt);
    String& a = out << (unsigned long)(unsigned short)(short)(char)m_type;
    a << sep;
    a << (long)m_typeId;
    return m_locations.toText(out, fmt);
}

void NpcLocation::read(iStream& s, DWORD tag)
{
    m_locations.deleteAll();
    unsigned char type = (unsigned char)s.readByte();
    m_typeId = s.readInt();
    m_type = type;
    m_locations.read(s, tag);
}

void NpcLocation::write(oStream& s, DWORD tag) const
{
    s << (char)m_type << (unsigned long)m_typeId;
    m_locations.write(s, tag);
}
