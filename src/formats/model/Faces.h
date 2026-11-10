#pragma once
#include "ptrArray.h"
#include "Streamable.h"
#include "Face.h"

class Faces : public ptrArray<Face*>, public Streamable
{
public:
    Faces() : ptrArray<Face*>(1024) {}
    ~Faces() override {}
    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;
};
static_assert(sizeof(Faces) == 0x1C, "Faces size");
