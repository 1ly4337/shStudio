#include "kWorld_Sounds.h"
#include "world_common.h"

namespace kWorld
{
    Sounds::ZoneArea::ZoneArea(const ZoneArea& other)
        : Node(other)
        , m_index(other.m_index)
        , m_pos(other.m_pos)
        , m_rot(other.m_rot)
        , m_x3(other.m_x3)
        , m_itemIndex(other.m_itemIndex)
        , m_unknown(other.m_unknown)
    {
    }

    Sounds::SpotArea::SpotArea(const SpotArea& other)
        : Node(other)
        , m_index(other.m_index)
        , m_itemIndex(other.m_itemIndex)
        , m_pos(other.m_pos)
        , m_radius(other.m_radius)
    {
    }
}
