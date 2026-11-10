#pragma once
#include "Spot.h"

class NamedArea : public Spot
{
public:
    static const int kMinRecordSize = 32;
    NamedArea(int id, int unused = 0);
    ~NamedArea() override {}

    int  getInt(DWORD tag) const override;

    void read(iStream& s, DWORD tag) override;
    void write(oStream& s, DWORD tag) const override;
    bool toText(String& out, DWORD fmt) const override;

public:
    float m_x2;
    float m_y2;
    float m_z2;
    int   m_unk1;
    int   m_unk2;
};
static_assert(sizeof(NamedArea) == 0x3C, "NamedArea size");
