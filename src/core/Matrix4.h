#pragma once
#include "Streamable.h"

class Matrix4 : public Streamable
{
public:
    Matrix4() { for (int i = 0; i < 16; ++i) m[i] = 0; }
    virtual ~Matrix4() {}
    virtual void read(iStream& in, DWORD fmt) override;
    virtual void write(oStream& out, DWORD fmt) const override;
    virtual bool toText(String& out, DWORD fmt) const override;

    float m[16];
};
static_assert(sizeof(Matrix4) == 0x44, "Matrix4 size");
