#include "ImageStrip.h"
#include "gdiutil.h"
#include <cstring>

ImageStrip::ImageStrip()
{
    m_bitmap = 0;
    m_keyColor = 0xFFFFFF;
    m_cx = 0;
    m_cy = 0;
    m_count = 0;
    m_capacity = 0;
    HDC display = CreateDCW(L"DISPLAY", 0, 0, 0);
    m_srcDC = CreateCompatibleDC(display);
    m_dstDC = CreateCompatibleDC(display);
    DeleteDC(display);
}

ImageStrip::~ImageStrip()
{
    DeleteDC(m_dstDC);
    DeleteDC(m_srcDC);
    DeleteObject(m_bitmap);
}

ImageStrip& ImageStrip::operator=(const ImageStrip& src)
{
    if (m_bitmap)
    {
        DeleteObject(m_bitmap);
        m_bitmap = 0;
    }
    for (int i = 0; i < m_ids.m_capacity; ++i)
        m_ids.resetItem(&m_ids.m_items[i]);
    m_ids.m_count = 0;
    m_cx = src.m_cx;
    m_cy = src.m_cy;
    m_bitmap = copyBitmap((const HGDIOBJ*)&src.m_bitmap);
    if (m_bitmap)
    {
        m_count = src.m_count;
        m_capacity = src.m_capacity;
        m_ids = src.m_ids;
    }
    else
    {
        m_count = 0;
        m_capacity = 0;
    }
    return *this;
}

void ImageStrip::append(const ImageStrip* src)
{
    for (int i = 0; i < src->m_count; ++i)
    {
        HBITMAP icon = src->getIcon(i);
        add(icon, src->idAt(i));
        DeleteObject(icon);
    }
}

void ImageStrip::clear()
{
    for (int i = 0; i < m_ids.m_capacity; ++i)
        m_ids.resetItem(&m_ids.m_items[i]);
    m_ids.m_count = 0;
    if (m_bitmap)
        DeleteObject(m_bitmap);
    m_bitmap = 0;
    m_count = 0;
    m_capacity = 0;
}

bool ImageStrip::add(HBITMAP icon, long id)
{
    if (!m_cx)
        return false;
    if (!icon)
        return false;
    for (int i = 0; i < m_ids.m_count; ++i)
    {
        if (m_ids.m_items[i] == id)
            return false;
    }
    BITMAP bm;
    memset(&bm, 0, sizeof(bm));
    if (!GetObjectW(icon, sizeof(BITMAP), &bm))
        return false;
    if (bm.bmWidth != m_cx || bm.bmHeight != m_cy)
        return false;
    if (m_count == m_capacity)
    {
        int capacity = m_capacity + 64;
        HBITMAP grown = createSolidBitmap(capacity * m_cx, m_cy, 0xFFFFFF);
        if (!grown)
            return false;
        if (m_bitmap)
        {
            if (!copyColumns(grown, 0, m_bitmap, (int)(INT_PTR)grown, m_cx * m_count))
            {
                DeleteObject(grown);
                return false;
            }
            DeleteObject(m_bitmap);
        }
        m_bitmap = grown;
        m_capacity = capacity;
    }
    if (!copyColumns(m_bitmap, m_cx * m_count, icon, m_cx, m_cx))
        return false;
    ++m_count;
    m_ids.add(id);
    return true;
}

HBITMAP ImageStrip::getIcon(int index) const
{
    if (index < 0 || index >= m_count)
        return 0;
    HDC display = CreateDCW(L"DISPLAY", 0, 0, 0);
    HDC iconDC = CreateCompatibleDC(display);
    HBITMAP icon = CreateCompatibleBitmap(display, m_cx, m_cy);
    HGDIOBJ oldIcon = SelectObject(iconDC, icon);
    HGDIOBJ oldStrip = SelectObject(m_srcDC, m_bitmap);
    TransparentBlt(iconDC, 0, 0, m_cx, m_cy, m_srcDC, index * m_cx, 0, m_cx, m_cy, m_keyColor);
    SelectObject(m_srcDC, oldStrip);
    SelectObject(iconDC, oldIcon);
    DeleteDC(iconDC);
    DeleteDC(display);
    return icon;
}

bool ImageStrip::draw(HDC hdc, int x, int y, int index) const
{
    if (!hdc || index < 0 || index >= m_count)
        return false;
    HGDIOBJ old = SelectObject(m_srcDC, m_bitmap);
    BOOL done = BitBlt(hdc, x, y, m_cx, m_cy, m_srcDC, index * m_cx, 0, SRCCOPY);
    SelectObject(m_srcDC, old);
    return done != 0;
}

bool ImageStrip::copyColumns(HBITMAP dst, int x, HBITMAP src, int , int width) const
{
    if (!dst || !src)
        return false;
    HGDIOBJ oldSrc = SelectObject(m_srcDC, src);
    HGDIOBJ oldDst = SelectObject(m_dstDC, dst);
    BOOL done = BitBlt(m_dstDC, x, 0, width, m_cy, m_srcDC, 0, 0, SRCCOPY);
    SelectObject(m_srcDC, oldSrc);
    SelectObject(m_dstDC, oldDst);
    return done != 0;
}
