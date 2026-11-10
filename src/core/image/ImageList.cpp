#include "ImageList.h"
#include "_imports_p10.h"

ImageList::~ImageList()
{
    destroy();
}

void ImageList::destroy()
{
    if (m_hList)
        ImageList_Destroy(m_hList);
    m_hList = 0;
    for (int i = 0; i < m_ids.m_capacity; ++i)
        m_ids.resetItem(&m_ids.m_items[i]);
    m_ids.m_count = 0;
}

void ImageList::removeAll()
{
    if (m_hList)
        ImageList_Remove(m_hList, -1);
    for (int i = 0; i < m_ids.m_capacity; ++i)
        m_ids.resetItem(&m_ids.m_items[i]);
    m_ids.m_count = 0;
}

ImageList* ImageList::load(WORD resourceId, UINT flags)
{
    destroy();
    int cx = m_cx;
    m_hList = ImageList_LoadImageW(GetModuleHandleW(0), MAKEINTRESOURCEW(resourceId), cx, 64, CLR_DEFAULT,
                                   IMAGE_BITMAP, flags | LR_CREATEDIBSECTION);
    int width, height;
    ImageList_GetIconSize(m_hList, &width, &height);
    m_cx = width;
    m_cy = height;
    int count = ImageList_GetImageCount(m_hList);
    for (int i = 0; i < count; ++i)
        m_ids.add(i);
    return this;
}

int ImageList::addBitmap(HBITMAP bitmap)
{
    int before = ImageList_GetImageCount(m_hList);
    int index = ImageList_Add(m_hList, bitmap, 0);
    if (index != -1)
    {
        int added = ImageList_GetImageCount(m_hList) - before;
        int base = m_ids.m_count;
        for (int i = 0; i < added; ++i)
            m_ids.add(i + base);
    }
    return index;
}

bool ImageList::addBitmapAt(HBITMAP bitmap, int firstId)
{
    int before = ImageList_GetImageCount(m_hList);
    if (ImageList_Add(m_hList, bitmap, 0) == -1)
        return false;
    int added = ImageList_GetImageCount(m_hList) - before;
    for (int i = 0; i < added; ++i)
        m_ids.add(i + firstId);
    return true;
}
