#pragma once
#include <windows.h>

class fPoint;

class MouseControler
{
public:
    MouseControler(fPoint* rotation, fPoint* position)
        : m_rotation(rotation), m_position(position), m_flyMode(0) {}
    virtual ~MouseControler() {}
    virtual void begin(const POINT* pt) = 0;
    virtual void move(const POINT* pt) = 0;

public:
    fPoint* m_rotation;
    fPoint* m_position;
    int     m_flyMode;
};
static_assert(sizeof(MouseControler) == 0x10, "MouseControler size");
