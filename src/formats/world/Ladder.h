#pragma once
#include "Spot.h"

class Ladder : public Spot
{
public:
    static const int kMinRecordSize = 12;
    Ladder(int id, int unused = 0);
    ~Ladder() override {}
};
static_assert(sizeof(Ladder) == 0x28, "Ladder size");
