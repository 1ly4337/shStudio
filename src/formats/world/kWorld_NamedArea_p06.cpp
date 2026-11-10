#include "kWorld_NamedArea.h"
#include "world_common.h"

namespace kWorld
{
    NamedArea::NamedArea(const NamedArea& other)
        : CaptionNode(other)
        , m_index(other.m_index)
        , m_pos(other.m_pos)
        , m_rot(other.m_rot)
        , m_x3(other.m_x3)
        , m_y3(other.m_y3)
        , m_z3(other.m_z3)
    {
    }
}
