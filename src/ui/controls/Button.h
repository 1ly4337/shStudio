#pragma once
#include "Control32.h"

class Button : public Control32 {
public:
    Button();
    virtual LRESULT setCheck(bool checked);
    virtual BOOL isChecked();
};
static_assert(sizeof(Button) == 0x0C, "Button size");
