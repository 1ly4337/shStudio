#pragma once
#include <windows.h>
#include "ptrArray.h"
#include "fPoint.h"

class iStream;

class fPoints : public ptrArray<fPoint*>
{
public:
    fPoints() : ptrArray<fPoint*>() {}
    virtual ~fPoints() {}

    bool read(iStream& in, DWORD fmt);
};
static_assert(sizeof(fPoints) == 0x18, "fPoints size");
