#pragma once
#include "p02_common.h"

class Point5 : public Streamable
{
public:
    Point5() : a(0), b(0), c(0), d(0), e(0) {}
    virtual ~Point5() {}
    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;

    float a;
    float b;
    float c;
    float d;
    float e;
};
static_assert(sizeof(Point5) == 0x18, "Point5 size");
