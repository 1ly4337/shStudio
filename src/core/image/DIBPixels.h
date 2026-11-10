#pragma once
#include <windows.h>

extern BYTE g_nullPixel[4];

class DIBPixels
{
public:
    DIBPixels() : m_bits(0), m_lastRow(0), m_width(0), m_height(0) {}
    ~DIBPixels()
    {
        if (m_bits)
            ::operator delete[](m_bits);
    }

    bool create(int unused, int width, int height);
    bool load(HBITMAP bitmap);

    BYTE* pixel(int x, int y)
    {
        if (x < 0 || x >= m_width || y < 0 || y >= m_height)
            return g_nullPixel;
        return (BYTE*)(m_lastRow + (x - y * m_width));
    }

    DWORD* m_bits;
    DWORD* m_lastRow;
    int    m_width;
    int    m_height;
};
static_assert(sizeof(DIBPixels) == 0x10, "DIBPixels size");
