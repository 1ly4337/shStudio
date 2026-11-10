#pragma once
#include <windows.h>

class GDC
{
public:
    explicit GDC(HDC hdc);
    virtual ~GDC();
    virtual HGDIOBJ restore();

    BOOL frameRect(const RECT* rc);
    BOOL frameRect(const RECT* rc, COLORREF color);
    BOOL fillRect(const RECT* rc, COLORREF color);
    BOOL frameRect(int left, int top, int right, int bottom, COLORREF color);
    BOOL line(int x1, int y1, int x2, int y2);
    HGDIOBJ selectPen(COLORREF color, int width, int style);

public:
    HDC     m_hdc;
    HGDIOBJ m_savedPen;
    HGDIOBJ m_saved0C;
    HGDIOBJ m_saved10;
    HGDIOBJ m_saved14;
};
static_assert(sizeof(GDC) == 0x18, "GDC size");
