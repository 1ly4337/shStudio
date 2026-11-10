#include "p02_common.h"
#include "Vertex.h"

void Vertex::assign(const Vertex* src)
{
    m_pos.x = src->m_pos.x;
    m_pos.y = src->m_pos.y;
    m_pos.z = src->m_pos.z;
    m_u = src->m_u;
    m_v = src->m_v;
}

bool Vertex::copyFrom(const Streamable* src)
{
    const Vertex* v = dynamic_cast<const Vertex*>(src);
    if (!v)
        return false;
    assign(v);
    return true;
}
