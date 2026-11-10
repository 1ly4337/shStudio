#include "p05_common.h"
#include "MonsterSpot.h"

MonsterSpot::MonsterSpot(Node* monster)
    : Node(), m_monster(monster), m_areas()
{
}

MonsterSpot::~MonsterSpot()
{
}

const wchar_t* MonsterSpot::getString(DWORD tag) const
{
    return m_monster->getString(tag);
}

int MonsterSpot::getInt(DWORD tag) const
{
    return m_monster->getInt(tag);
}

int MonsterSpot::compare(const Node* other, DWORD tag) const
{
    const MonsterSpot& spot = dynamic_cast<const MonsterSpot&>(*other);
    return m_monster->compare(spot.m_monster, tag);
}
