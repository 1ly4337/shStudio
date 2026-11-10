#pragma once
#include "p02_common.h"
#include "vLong.h"
#include "vFloat.h"

class Sequence
{
public:
    Sequence() {}
    void read(iStream& in, DWORD fmt);
    bool toText(String& out, DWORD fmt) const;

    String m_name;
    vLong  m_frames;
    vFloat m_delays;
};
static_assert(sizeof(Sequence) == 0x48, "Sequence size");
