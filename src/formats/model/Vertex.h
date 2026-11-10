#pragma once
#include "Streamable.h"
#include "fPoint.h"

class Vertex : public Streamable
{
public:
    Vertex() : m_u(0), m_v(0) {}

    bool copyFrom(const Streamable* src) override;
    virtual void assign(const Vertex* src);

    fPoint m_pos;
    float  m_u;
    float  m_v;
};
static_assert(sizeof(Vertex) == 0x1C, "Vertex size");
