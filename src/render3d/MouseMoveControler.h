#pragma once
#include "MouseControler.h"

class MouseMoveControler : public MouseControler
{
public:
    MouseMoveControler(fPoint* rotation, fPoint* position)
        : MouseControler(rotation, position)
    {
        m_last.y = 0;
        m_last.x = 0;
    }
    void begin(const POINT* pt) override;
    void move(const POINT* pt) override;

public:
    POINT m_last;
};
static_assert(sizeof(MouseMoveControler) == 0x18, "MouseMoveControler size");
