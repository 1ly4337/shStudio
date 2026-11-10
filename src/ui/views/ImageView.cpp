#include "ImageView.h"
#include "_imports_p10.h"

ImageView::ImageView(FileTree* node)
    : SimpleView(node),
      m_bytes(0, 0u),
      m_bitmap((Window_isClassicAppearance(), GetSysColor(COLOR_WINDOW)), 0)
{
    m_field58 = 0;
}

LPCWSTR ImageView::getClassName()
{
    return L"PictView";
}

void ImageView::onRegisterClass(WNDCLASSEXW* wcx)
{
    int cy = GetSystemMetrics(SM_CYICON);
    int cx = GetSystemMetrics(SM_CXICON);
    wcx->hIcon = (HICON)LoadImageW(hModule, MAKEINTRESOURCEW(0x82), IMAGE_ICON, cx, cy, LR_SHARED);
    int cySmall = GetSystemMetrics(SM_CYSMICON);
    int cxSmall = GetSystemMetrics(SM_CXSMICON);
    wcx->hIconSm = (HICON)LoadImageW(hModule, MAKEINTRESOURCEW(0x82), IMAGE_ICON, cxSmall, cySmall, LR_SHARED);
}

int ImageView::getMenuId()
{
    return 165;
}

ImageView::~ImageView()
{
}
