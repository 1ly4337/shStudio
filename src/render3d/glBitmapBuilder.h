#pragma once
#include <windows.h>

class glBitmapBuilder
{
public:
    glBitmapBuilder();
    virtual ~glBitmapBuilder();

    DWORD begin(const SIZE* size, COLORREF background);
    DWORD end(HBITMAP* bitmap);

    HGLRC   m_hglrc;
    HDC     m_hdc;
    SIZE    m_size;
    HBITMAP m_hDib;
    HGDIOBJ m_hOld;
    void*   m_bits;
    PIXELFORMATDESCRIPTOR m_pfd;
};
static_assert(sizeof(glBitmapBuilder) == 0x48, "glBitmapBuilder size");
