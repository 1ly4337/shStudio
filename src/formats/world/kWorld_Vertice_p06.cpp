#include "kWorld_Vertice.h"
#include "world_common.h"

namespace kWorld
{
    Vertice::Vertice(const Vertice& other)
        : Node(other)
        , m_index(other.m_index)
        , m_pos(other.m_pos)
        , m_rot(other.m_rot)
        , m_p3(other.m_p3)
        , m_unkn(other.m_unkn)
    {
    }
}
