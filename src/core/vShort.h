#pragma once
#include "primitiveVector.h"

class vShort : public primitiveVector<short> {
public:
    vShort() {}
    virtual ~vShort() {}
};
static_assert(sizeof(vShort) == 0x14, "vShort size");
