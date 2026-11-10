#pragma once
#include "GDC.h"

class PaintDC : public GDC
{
public:
    explicit PaintDC(HWND hWnd);
    virtual ~PaintDC();

public:
    HWND        m_hWnd;
    PAINTSTRUCT m_ps;
};
static_assert(sizeof(PaintDC) == 0x5C, "PaintDC size");
