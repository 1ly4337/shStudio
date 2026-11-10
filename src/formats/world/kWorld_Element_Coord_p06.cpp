#include "kWorld_Element.h"

namespace kWorld
{
    Element::Coord::Coord(const Coord& other)
        : Node()
    {
        m_mode = other.m_mode;
        m_unknown = other.m_unknown;
        m_itemIndex = other.m_itemIndex;
        m_pos.x = other.m_pos.x;
        m_pos.y = other.m_pos.y;
        m_pos.z = other.m_pos.z;
        m_rot.x = other.m_rot.x;
        m_rot.y = other.m_rot.y;
        m_rot.z = other.m_rot.z;
        m_scale.x = other.m_scale.x;
        m_scale.y = other.m_scale.y;
        m_scale.z = other.m_scale.z;
        m_index = other.m_index;
    }
}
