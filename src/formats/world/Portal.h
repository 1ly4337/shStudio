#pragma once
#include "Spot.h"

class Portal : public Spot
{
public:
    static const int kMinRecordSize = 36;
    Portal(int id, int unused = 0);
    ~Portal() override {}

    int  getInt(DWORD tag) const override;
    void reset() override;
    virtual bool copyFrom(const Portal* other);

    void read(iStream& s, DWORD tag) override;
    void write(oStream& s, DWORD tag) const override;
    bool toText(String& out, DWORD fmt) const override;
    bool copyFrom(const Streamable* src) override;

public:
    int   m_faction;
    short m_levelMin;
    short m_levelMax;
    int   m_destMap;
    float m_destX;
    float m_destY;
    float m_destZ;
};
static_assert(sizeof(Portal) == 0x40, "Portal size");
