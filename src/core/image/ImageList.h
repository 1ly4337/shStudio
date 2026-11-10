#pragma once
#include <windows.h>
#include <commctrl.h>
#include "vLong.h"

struct ImageList
{
    ImageList() : m_hList(0), m_cx(0), m_cy(0) {}
    ~ImageList();

    void destroy();
    void removeAll();
    ImageList* load(WORD resourceId, UINT flags);
    int addBitmap(HBITMAP bitmap);
    bool addBitmapAt(HBITMAP bitmap, int firstId);

    HIMAGELIST m_hList;
    int        m_cx;
    int        m_cy;
    vLong      m_ids;
};
static_assert(sizeof(ImageList) == 0x20, "ImageList size");
