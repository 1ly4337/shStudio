#pragma once
#include "StandardNPC.h"

struct GateTarget {
    String  name;
    int16_t mapId;
    int16_t _pad22;
    float   x;
    float   y;
    float   z;
    int     cost;

    GateTarget();
    void read(iStream& s, int tag);
    void write(oStream& s, int tag) const;
};
static_assert(sizeof(GateTarget) == 0x34, "GateTarget size");

class GateKeeper : public StandardNPC {
public:
    GateKeeper();
    virtual ~GateKeeper();

    virtual const wchar_t* getString(DWORD tag) const;
    virtual bool setString(DWORD tag, const wchar_t* text);
    virtual void reset();
    virtual int  compare(const Node* other, DWORD tag) const;
    virtual void assign(const StandardNPC* src);
    virtual void read(iStream& s, DWORD tag);
    virtual void write(oStream& s, DWORD tag) const;

public:
    GateTarget m_targets[3];
};
static_assert(sizeof(GateKeeper) == 0x170, "GateKeeper size");
