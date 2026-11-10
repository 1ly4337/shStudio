#include "kWorld_CaptionNode.h"
#include "world_common.h"

namespace kWorld
{
    CaptionNode::CaptionNode(const CaptionNode& other)
        : Node(other)
        , m_name1(other.m_name1)
        , m_name2(other.m_name2)
    {
    }
}
