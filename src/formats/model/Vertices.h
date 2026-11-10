#pragma once
#include "ptrArray.h"
#include "Vertex.h"

class String;

class Vertices : public ptrArray<Vertex*>
{
public:
    Vertices() : ptrArray<Vertex*>(1024) {}
    virtual bool toText(String& out, DWORD fmt) const;
};
static_assert(sizeof(Vertices) == 0x18, "Vertices size");
