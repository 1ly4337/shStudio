#pragma once
#include "ptrArray.h"
#include "Sequence.h"

class Sequence;

class Sequences : public ptrArray<Sequence*> {
public:
    Sequences() : ptrArray<Sequence*>(256) {}
};
static_assert(sizeof(Sequences) == 0x18, "Sequences size");
