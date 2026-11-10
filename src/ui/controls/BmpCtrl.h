#pragma once
#include <windows.h>
#include "Window.h"
#include "bitMask.h"
#include "ImageList.h"

class BmpCtrl : public Window
{
public:
    BmpCtrl(COLORREF bkColor, COLORREF fillColor);
    ~BmpCtrl() override;

    BOOL setBitmap(HBITMAP bitmap);

    bool onLButton(WPARAM keys, POINT* pt, bool down) override;
    LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

public:
    bool     m_ownsBitmap;
    COLORREF m_fillColor;
    SIZE     m_bitmapSize;
    bitMask<unsigned char> m_style;
    HBITMAP  m_bitmap;
    ImageList m_images;
    int      m_imageIndex;
    DWORD    m_drawFlags;
};
static_assert(sizeof(BmpCtrl) == 0x78, "BmpCtrl size");
