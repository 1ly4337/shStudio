#pragma once
#include <windows.h>
#include "Control32.h"

class ColorChooser : public Control32
{
public:
    ColorChooser() : m_color(0xFF000000) {}

    bool create(HWND parent, HMENU id, int x, int y, int cx, int cy, COLORREF color, int unused1, int unused2);
    static LRESULT CALLBACK subclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
                                         DWORD_PTR refData);

public:
    COLORREF m_color;
};
static_assert(sizeof(ColorChooser) == 0x10, "ColorChooser size");

extern COLORREF g_customColors[16];
