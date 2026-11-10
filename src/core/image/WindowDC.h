#pragma once
#include <windows.h>
#include "GDC.h"

class WindowDC : public GDC
{
public:
    explicit WindowDC(HWND hWnd);
    ~WindowDC() override;

public:
    HWND m_hWnd;
};
static_assert(sizeof(WindowDC) == 0x1C, "WindowDC size");
