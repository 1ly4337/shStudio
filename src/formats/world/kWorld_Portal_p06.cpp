#include "kWorld_Portal.h"
#include "world_common.h"

namespace kWorld
{
    Portal::Portal(const Portal& other)
        : CaptionNode(other)
        , m_index(other.m_index)
        , m_pos(other.m_pos)
        , m_rot(other.m_rot)
        , m_value70(other.m_value70)
        , m_unkn(other.m_unkn)
        , m_value76(other.m_value76)
        , m_value78(other.m_value78)
        , m_dest(other.m_dest)
    {
    }
}
