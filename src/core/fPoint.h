#pragma once
#include "Streamable.h"

class fPoint : public Streamable
{
public:
    fPoint() : x(0), y(0), z(0) {}
    fPoint(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    virtual ~fPoint() {}
    virtual void read(iStream& in, DWORD fmt) override;
    virtual void write(oStream& out, DWORD fmt) const override;
    virtual bool toText(String& out, DWORD fmt) const override;

    float x;
    float y;
    float z;

    float& at(int index);
    fPoint operator-(const fPoint& rhs) const;
    fPoint unitCross(const fPoint& rhs) const;
    fPoint cross(const fPoint& rhs) const;
};
static_assert(sizeof(fPoint) == 0x10, "fPoint size");
