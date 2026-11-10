#pragma once
#include <windows.h>
#include "primitiveVector.h"

class HWndVec : public primitiveVector<HWND>
{
public:
    HWndVec() {}
    virtual ~HWndVec();
};
static_assert(sizeof(HWndVec) == 0x14, "HWndVec size");
