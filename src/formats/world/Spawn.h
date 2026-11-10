#pragma once
#include "Spot.h"

class Spawn : public Spot
{
public:
    static const int kMinRecordSize = 36;
    Spawn(int id, int unused = 0);
    ~Spawn() override {}

    int  getInt(DWORD tag) const override;
    int  compare(const Node* other, DWORD tag) const override;

    void read(iStream& s, DWORD tag) override;
    void write(oStream& s, DWORD tag) const override;
    bool toText(String& out, DWORD fmt) const override;

public:
    int   m_unk20;
    int   m_faction;
    int   m_unk28;
    float m_x2;
    float m_y2;
    float m_z2;
};
static_assert(sizeof(Spawn) == 0x40, "Spawn size");
