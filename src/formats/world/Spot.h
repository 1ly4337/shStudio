#pragma once
#include "p02_common.h"

int compareFloat(float a, float b);

class Spot : public Node
{
public:
    Spot(int id);
    virtual ~Spot();

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;
    bool copyFrom(const Streamable* src) override;
    int  getInt(DWORD tag) const override;
    void reset() override;
    int  compare(const Node* other, DWORD tag) const override;
    virtual bool copyPos(const Spot* src);

    HWND  m_hItem;
    int   m_id;
    float m_x;
    float m_y;
    float m_z;
};
static_assert(sizeof(Spot) == 0x28, "Spot size");
