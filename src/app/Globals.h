#pragma once
#include <windows.h>
#include "primitiveVector.h"

class Globals : public primitiveVector<void*>
{
public:
    Globals() {}
    virtual ~Globals() {}
    virtual void** resetItem(void** item) override;
};
static_assert(sizeof(Globals) == 0x14, "Globals size");
