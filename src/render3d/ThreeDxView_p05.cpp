#include "p05_common.h"
#include "ThreeDxView.h"

ThreeDxView::ThreeDxView(FileTree* node, DWORD type)
    : Document(node), m_viewer(), m_textureDir()
{
    m_drawable = 0;
    m_type = type;
}

void ThreeDxView::setModel(Drawable* model)
{
    m_viewer.setDrawable(model);
}

LPCWSTR ThreeDxView::getClassName()
{
    return L"3DX View";
}

void ThreeDxView::onRegisterClass(WNDCLASSEXW* wc)
{
    int cy = GetSystemMetrics(SM_CYICON);
    int cx = GetSystemMetrics(SM_CXICON);
    wc->hIcon = (HICON)LoadImageW(hModule, (LPCWSTR)0x88, IMAGE_ICON, cx, cy, LR_SHARED);
    int cySm = GetSystemMetrics(SM_CYSMICON);
    int cxSm = GetSystemMetrics(SM_CXSMICON);
    wc->hIconSm = (HICON)LoadImageW(hModule, (LPCWSTR)0x88, IMAGE_ICON, cxSm, cySm, LR_SHARED);
}

SIZE* ThreeDxView::getDefaultSize(SIZE* out, SIZE def)
{
    out->cx = def.cx;
    out->cy = def.cy + 30;
    return out;
}

int ThreeDxView::getMenuId()
{
    return 167;
}
