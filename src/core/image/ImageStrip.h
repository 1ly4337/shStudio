#pragma once
#include <windows.h>
#include "vLong.h"

class ImageStrip
{
public:
    ImageStrip();
    ~ImageStrip();

    ImageStrip& operator=(const ImageStrip& src);
    ImageStrip(const ImageStrip&) = delete;

    void append(const ImageStrip* src);
    void clear();
    bool add(HBITMAP icon, long id);
    HBITMAP getIcon(int index) const;
    bool draw(HDC hdc, int x, int y, int index) const;
    bool copyColumns(HBITMAP dst, int x, HBITMAP src, int unused, int width) const;

    long idAt(int index) const
    {
        return (index < 0 || index >= m_ids.m_capacity) ? 0 : m_ids.m_items[index];
    }

public:
    HBITMAP  m_bitmap;
    vLong    m_ids;
    int      m_cx;
    int      m_cy;
    int      m_count;
    int      m_capacity;
    HDC      m_srcDC;
    HDC      m_dstDC;
    COLORREF m_keyColor;
};
static_assert(sizeof(ImageStrip) == 0x34, "ImageStrip size");
