#pragma once
#include "primitiveVector.h"

class vLong : public primitiveVector<long>
{
public:
    vLong() {}
    virtual ~vLong() {}
};
static_assert(sizeof(vLong) == 0x14, "vLong size");
