#pragma once
#include "GDC.h"

class ScreenDC : public GDC
{
public:
    ScreenDC();
    virtual ~ScreenDC();

public:
    HDC     m_displayDC;
    HGDIOBJ m_prevBitmap;
};
static_assert(sizeof(ScreenDC) == 0x20, "ScreenDC size");
