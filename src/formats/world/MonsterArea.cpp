#include "MonsterArea.h"
#include "String.hpp"
#include "iStream.hpp"
#include "oStream.hpp"
#include "ida_compat.h"
#include "ClientData.h"
#include "StudioFrame.h"
#include "_imports_w3.h"
#include "w3_util.h"

MonsterArea::MonsterID::MonsterID()
{
    m_count = 0;
    m_mobId = 0;
}

void MonsterArea::MonsterID::reset()
{
    m_count = 0;
    m_mobId = 0;
}

void MonsterArea::MonsterID::read(iStream& s, DWORD )
{
    m_mobId = s.readInt();
    m_count = s.readInt();
}

void MonsterArea::MonsterID::write(oStream& s, DWORD ) const
{
    s << (unsigned long)m_mobId << (unsigned long)m_count;
}

int MonsterArea::MonsterID::getInt(DWORD tag) const
{
    if (tag == 0x6D626964 )
        return m_mobId;
    if (tag == 0x6D636E74 )
        return m_count;
    return 0;
}

bool MonsterArea::MonsterID::setInt(DWORD tag, int value)
{
    if (tag == 0x6D626964 )
    {
        m_mobId = value;
        return true;
    }
    if (tag == 0x6D636E74 )
    {
        m_count = value;
        return true;
    }
    return false;
}

int MonsterArea::MonsterID::compare(const Node* other, DWORD tag) const
{
    const MonsterID& o = dynamic_cast<const MonsterID&>(*other);
    if (tag == 0x6D626964 )
    {
        if (m_mobId != o.m_mobId)
            return 2 * (m_mobId >= o.m_mobId) - 1;
        return 0;
    }
    if (tag == 0x6D636E74 )
    {
        if (m_count != o.m_count)
            return 2 * (m_count >= o.m_count) - 1;
        return 0;
    }
    return 0;
}

bool MonsterArea::MonsterID::copyFrom(const Streamable* src)
{
    const MonsterID* m = dynamic_cast<const MonsterID*>(src);
    if (!m)
        return false;
    m_mobId = m->m_mobId;
    m_count = m->m_count;
    return true;
}

bool MonsterArea::MonsterID::toText(String& out, DWORD fmt) const
{
    wchar_t sep = w3_separatorFor(fmt);
    String& s = out << (long)m_mobId;
    s << sep;
    s << (long)m_count;
    return true;
}

const wchar_t* MonsterArea::MonsterID::getString(DWORD tag) const
{
    if (tag != 0x6E616D65 )
        return 0;
    ClientData& client = g_studioFrame->m_client.m_clientData;
    client.loadMonsters();
    int count = client.m_monsters.m_count;
    if (count <= 0)
        return L"Unknown";
    int i = 0;
    while (FIELD(int, client.m_monsters.m_items[i], 0x14) != m_mobId)
    {
        if (++i >= count)
            return L"Unknown";
    }
    Node* mob = (Node*)client.m_monsters.m_items[i];
    if (mob)
        return mob->getString('name');
    return L"Unknown";
}

void MonsterArea::MonsterIDs::copyFrom(const MonsterIDs& src)
{
    deleteAll();
    for (int i = 0; i < src.m_count; ++i)
    {
        MonsterID* n = new MonsterID(src.m_items[i]);
        add(n);
    }
}

bool MonsterArea::MonsterIDs::toText(String& out, DWORD fmt) const
{
    wchar_t sep = w3_separatorFor(fmt);
    for (int i = 0; i < m_count; ++i)
    {
        out << sep;
        m_items[i]->toText(out, fmt);
    }
    return true;
}

void MonsterArea::MonsterIDs::read(iStream& s, DWORD tag)
{
    deleteAll();
    MonsterID* current = 0;
    try
    {
        int count = s.readInt();
        for (int i = 0; i < count; ++i)
        {
            MonsterID* m = new MonsterID();
            current = m;
            m->read(s, tag);
            add(m);
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

void MonsterArea::MonsterIDs::write(oStream& s, DWORD tag) const
{
    s << (unsigned long)m_count;
    for (int i = 0; i < m_count; ++i)
        m_items[i]->write(s, tag);
}

MonsterArea::MonsterArea(int id, int )
    : Spot(id), m_ids()
{
    m_z = 0;
    m_y = 0;
    m_x = 0;
    m_z2 = 0;
    m_y2 = 0;
    m_x2 = 0;
    m_ids.deleteAll();
}

MonsterArea::~MonsterArea()
{
}

bool MonsterArea::copyFrom(const Streamable* src)
{
    if (!copyPos(dynamic_cast<const Spot*>(src)))
        return false;
    const MonsterArea* m = dynamic_cast<const MonsterArea*>(src);
    if (!m)
        return false;
    m_x2 = m->m_x2;
    m_y2 = m->m_y2;
    m_z2 = m->m_z2;
    m_ids.copyFrom(m->m_ids);
    return true;
}

int MonsterArea::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 0x70737832:
        return *(int*)&m_x2;
    case 0x70737932:
        return *(int*)&m_y2;
    case 0x70737A32:
        return *(int*)&m_z2;
    }
    return Spot::getInt(tag);
}

void MonsterArea::reset()
{
    m_z = 0;
    m_y = 0;
    m_x = 0;
    m_z2 = 0;
    m_y2 = 0;
    m_x2 = 0;
    m_ids.deleteAll();
}

bool MonsterArea::toText(String& out, DWORD fmt) const
{
    wchar_t sep = w3_separatorFor(fmt);
    String& a = out << m_x;
    a << sep;
    String& b = a << m_y;
    b << sep;
    String& c = b << m_z << m_x2;
    c << sep;
    String& d = c << m_y2;
    d << sep;
    d << m_z2;
    return m_ids.toText(out, fmt);
}

void MonsterArea::read(iStream& s, DWORD tag)
{
    m_ids.deleteAll();
    m_x = s.readFloat();
    m_y = s.readFloat();
    m_z = s.readFloat();
    m_x2 = s.readFloat();
    m_y2 = s.readFloat();
    m_z2 = s.readFloat();
    m_ids.read(s, tag);
}

void MonsterArea::write(oStream& s, DWORD tag) const
{
    s << (float)m_x << (float)m_y << (float)m_z << (float)m_x2 << (float)m_y2 << (float)m_z2;
    m_ids.write(s, tag);
}
