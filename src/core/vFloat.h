#pragma once
#include "primitiveVector.h"

class vFloat : public primitiveVector<float>
{
public:
    vFloat() {}
    virtual ~vFloat() {}
};
static_assert(sizeof(vFloat) == 0x14, "vFloat size");
