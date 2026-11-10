#pragma once
#include "p04_common.h"
#include "Node.h"
#include "String.hpp"
#include "fPoint.h"

namespace kWorld
{
    inline int floatBits(const float& f) { return *(const int*)&f; }

    class CaptionNode : public Node
    {
    public:
        CaptionNode();
        CaptionNode(const CaptionNode& other);
        virtual ~CaptionNode();

    public:
        String m_name1;
        String m_name2;
    };
}
