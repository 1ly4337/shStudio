#pragma once
#include <windows.h>
#include "ptrArray.h"
#include "Serializable.h"
#include "EffectSet.h"
class iStream;
class oStream;

class EffectSets : public ptrArray<EffectSet*>, public Serializable
{
public:
    EffectSets();
    ~EffectSets() override;

    virtual int vslot13(int arg);

    bool isEmpty() const override { return m_count == 0; }
    void clear() override { deleteAll(); }

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;

public:
    DWORD m_field2C;
    DWORD m_field30;
    DWORD m_field34;
    DWORD m_field38;
};
static_assert(sizeof(EffectSets) == 0x44, "EffectSets size");
