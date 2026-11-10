#pragma once
#include <windows.h>
#include "ptrArray.h"
#include "Monster.h"

class Monster;

class MonstersSet : public ptrArray<Monster*>
{
public:
    MonstersSet() {}
    virtual ~MonstersSet();

};
static_assert(sizeof(MonstersSet) == 0x18, "MonstersSet size");
