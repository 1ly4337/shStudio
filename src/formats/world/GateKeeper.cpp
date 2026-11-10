#include "GateKeeper.h"
#include "_w2_inl.h"

GateTarget::GateTarget()
{
    mapId = 0;
    z = 0;
    y = 0;
    x = 0;
    cost = 0;
}

void GateTarget::read(iStream& s, int tag)
{
    switch (tag) {
    case 0x66696C65 :
        mapId = s.readShort();
        x = s.readFloat();
        y = s.readFloat();
        z = s.readFloat();
        name.read(s, 4, static_cast<aStream&>(s).m_codePage);
        cost = s.readInt();
        break;
    case 0x666C6E74 :
        mapId = s.readShort();
        x = s.readFloat();
        y = s.readFloat();
        z = s.readFloat();
        cost = s.readInt();
        break;
    case 0x6C747874 :
        name.read(s, 4, 0xFFFFFFFF);
        break;
    }
}

void GateTarget::write(oStream& s, int tag) const
{
    switch (tag) {
    case 0x66696C65 : {
        oStream& o = s << (unsigned short)mapId << x << y << z;
        name.write(o, 4, static_cast<aStream&>(o).m_codePage);
        o << (unsigned long)cost;
        break;
    }
    case 0x666C6E74 :
        s << (unsigned short)mapId << x << y << z << (unsigned long)cost;
        break;
    case 0x6C747874 :
        name.write(s, 4, 0xFFFFFFFF);
        break;
    }
}

GateKeeper::GateKeeper()
    : StandardNPC(2)
{
}

GateKeeper::~GateKeeper()
{
}

void GateKeeper::reset()
{
    StandardNPC::reset();
    for (int k = 0; k < 3; ++k) {
        m_targets[k].name.clear();
        m_targets[k].mapId = 0;
        m_targets[k].z = 0;
        m_targets[k].y = 0;
        m_targets[k].x = 0;
        m_targets[k].cost = 0;
    }
}

const wchar_t* GateKeeper::getString(DWORD tag) const
{
    switch (tag) {
    case 0x6C6F6331 : return m_targets[0].name.m_buffer;
    case 0x6C6F6332 : return m_targets[1].name.m_buffer;
    case 0x6C6F6333 : return m_targets[2].name.m_buffer;
    }
    return StandardNPC::getString(tag);
}

bool GateKeeper::setString(DWORD tag, const wchar_t* text)
{
    if (tag != 0x6C6F6331  && tag != 0x6C6F6332  && tag != 0x6C6F6333 )
        return StandardNPC::setString(tag, text);
    if (tag == 0x6C6F6331 )
        m_targets[0].name = text;
    else if (tag == 0x6C6F6332 )
        m_targets[1].name = text;
    else
        m_targets[2].name = text;
    return true;
}

int GateKeeper::compare(const Node* otherNode, DWORD tag) const
{
    if ((tag & 0x6C6F6300) != 0x6C6F6300)
        return StandardNPC::compare(otherNode, tag);
    const GateKeeper* other = &dynamic_cast<const GateKeeper&>(*otherNode);
    switch (tag) {
    case 0x6C6F6331 :
        return String::compareLogical(m_targets[0].name, other->m_targets[0].name);
    case 0x6C6F6332 :
        return String::compareLogical(m_targets[1].name, other->m_targets[1].name);
    case 0x6C6F6333 :
        return String::compareLogical(m_targets[2].name, other->m_targets[2].name);
    default:
        return StandardNPC::compare(otherNode, tag);
    }
}

void GateKeeper::assign(const StandardNPC* src)
{
    StandardNPC::assign(src);
    const GateKeeper* g = dynamic_cast<const GateKeeper*>(src);
    if (g) {
        for (int k = 0; k < 3; ++k) {
            if (&g->m_targets[k].name != &m_targets[k].name)
                m_targets[k].name = g->m_targets[k].name;
            m_targets[k].mapId = g->m_targets[k].mapId;
            m_targets[k].x = g->m_targets[k].x;
            m_targets[k].y = g->m_targets[k].y;
            m_targets[k].z = g->m_targets[k].z;
            m_targets[k].cost = g->m_targets[k].cost;
        }
    }
}

void GateKeeper::read(iStream& s, DWORD tag)
{
    this->readMain(s, tag);
    m_targets[0].read(s, tag);
    m_targets[1].read(s, tag);
    m_targets[2].read(s, tag);
    this->readLists(s, tag);
}

void GateKeeper::write(oStream& s, DWORD tag) const
{
    this->writeMain(s, tag);
    m_targets[0].write(s, tag);
    m_targets[1].write(s, tag);
    m_targets[2].write(s, tag);
    this->writeLists(s, tag);
}
