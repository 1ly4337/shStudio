#include "kWorld_Spawn.h"
#include "world_common.h"

namespace kWorld
{
    Spawn::Spawn(const Spawn& other)
        : Node(other)
        , m_index(other.m_index)
        , m_x3(other.m_x3)
        , m_pos(other.m_pos)
        , m_rot(other.m_rot)
        , m_y3(other.m_y3)
        , m_unkn(other.m_unkn)
        , m_z3(other.m_z3)
    {
    }
}
