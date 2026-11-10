#pragma once
#include "Control32.h"

class UpDownCtrl : public Control32 {
public:
    UpDownCtrl();
};
static_assert(sizeof(UpDownCtrl) == 0x0C, "UpDownCtrl size");
