#include "TacticsZone.h"
#include "iStream.hpp"
#include "oStream.hpp"

TacticsZone::TacticsZone(int id, int )
    : Spot(id), m_desc()
{
    m_pX1 = &m_x;
    m_pY1 = &m_y;
    m_pZ1 = &m_z;
    m_originY = 0;
    m_originX = 0;
    m_scaler = 0;
    TacticsZone::reset();
}

TacticsZone::~TacticsZone()
{
}

void TacticsZone::reset()
{
    m_unk1 = 0;
    m_unk2 = 0;
    m_unk0 = 0;
    *m_pZ1 = 0;
    *m_pY1 = 0;
    *m_pX1 = 0;
    m_z2 = 0;
    m_y2 = 0;
    m_x2 = 0;
    m_z3 = 0;
    m_y3 = 0;
    m_x3 = 0;
    m_z4 = 0;
    m_y4 = 0;
    m_x4 = 0;
    m_unk4 = 0;
    m_unk3 = 0;
    m_map = 0;
    m_unk5 = 0;
    m_desc.clear();
}

void TacticsZone::read(iStream& s, DWORD )
{
    m_unk0 = (BYTE)s.readByte();
    m_unk1 = (BYTE)s.readByte();
    m_unk2 = (BYTE)s.readByte();
    *m_pX1 = s.readFloat();
    *m_pY1 = s.readFloat();
    *m_pZ1 = s.readFloat();
    m_x2 = s.readFloat();
    m_y2 = s.readFloat();
    m_z2 = s.readFloat();
    m_x3 = s.readFloat();
    m_y3 = s.readFloat();
    m_z3 = s.readFloat();
    m_x4 = s.readFloat();
    m_y4 = s.readFloat();
    m_z4 = s.readFloat();
    m_unk3 = s.readInt();
    m_unk4 = s.readInt();
    m_map = (BYTE)s.readByte();
    m_unk5 = (BYTE)s.readByte();
    m_desc.read(s, 4, s.m_codePage);
    m_desc.stripCrLf();
}

void TacticsZone::write(oStream& s, DWORD ) const
{
    s << (char)m_unk0 << (char)m_unk1 << (char)m_unk2;
    s << *m_pX1 << *m_pY1 << *m_pZ1 << m_x2 << m_y2 << m_z2 << m_x3 << m_y3 << m_z3 << m_x4 << m_y4 << m_z4;
    s << (unsigned long)m_unk3 << (unsigned long)m_unk4;
    s << (char)m_map << (char)m_unk5;
    m_desc.write(s, 4, s.m_codePage);
}

bool TacticsZone::copyPos(const Spot* other)
{
    const TacticsZone* z = dynamic_cast<const TacticsZone*>(other);
    if (!z)
        return false;
    m_unk0 = z->m_unk0;
    m_unk1 = z->m_unk1;
    m_unk2 = z->m_unk2;
    *m_pX1 = *z->m_pX1;
    *m_pY1 = *z->m_pY1;
    *m_pZ1 = *z->m_pZ1;
    m_x2 = z->m_x2;
    m_y2 = z->m_y2;
    m_z2 = z->m_z2;
    m_x3 = z->m_x3;
    m_y3 = z->m_y3;
    m_z3 = z->m_z3;
    m_x4 = z->m_x4;
    m_y4 = z->m_y4;
    m_z4 = z->m_z4;
    m_unk3 = z->m_unk3;
    m_unk4 = z->m_unk4;
    m_map = z->m_map;
    m_unk5 = z->m_unk5;
    m_desc = z->m_desc;
    return true;
}

const wchar_t* TacticsZone::getString(DWORD tag) const
{
    if (tag == 'desc')
        return m_desc.m_buffer;
    return 0;
}
