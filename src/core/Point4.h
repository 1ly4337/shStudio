#pragma once
#include "Streamable.h"

class Point4 : public Streamable
{
public:
    Point4() : x(0), y(0), z(0), w(0) {}
    virtual void read(iStream& in, DWORD fmt) override;
    virtual void write(oStream& out, DWORD fmt) const override;
    virtual bool toText(String& out, DWORD fmt) const override;

    float x;
    float y;
    float z;
    float w;
};
static_assert(sizeof(Point4) == 0x14, "Point4 size");
