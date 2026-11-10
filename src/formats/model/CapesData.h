#pragma once
#include "ptrArray.h"
#include "CapeData.h"

class CapesData : public ptrArray<CapeData*>
{
public:
    CapesData() : ptrArray<CapeData*>(256) {}
};
static_assert(sizeof(CapesData) == 0x18, "CapesData size");
