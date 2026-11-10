#pragma once
#include <windows.h>

class Drawable;
class fPoint;

class glControls
{
public:
    glControls() {}
    virtual ~glControls() {}
    virtual void vslot1(Drawable* drawable) {}
    virtual void vslot2(const fPoint* rotation) {}
    virtual void vslot3(const fPoint* position) {}
};
static_assert(sizeof(glControls) == 4, "glControls size");
