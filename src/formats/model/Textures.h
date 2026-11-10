#pragma once
#include <windows.h>
#include <GL/gl.h>
#include "HTable.h"

class Textures : public HTable<long, GLuint>
{
public:
    Textures() : HTable<long, GLuint>(512, 128) {}
    virtual ~Textures() {}
    virtual Entry* clearEntry(Entry* e) override;
};
static_assert(sizeof(Textures) == 0x14, "Textures size");
