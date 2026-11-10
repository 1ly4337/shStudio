#include "p05_common.h"
#include "PopupView3D.h"

PopupView3D::PopupView3D()
    : Popup(), glControls(), m_alpha(), m_viewer(), m_angles()
{
    m_viewer.m_controls = static_cast<glControls*>(this);
    m_updating = false;
}

PopupView3D::~PopupView3D()
{
}

void PopupView3D::clearModel()
{
    m_viewer.setDrawable(0);
}

void PopupView3D::setModel(Drawable* model)
{
    m_viewer.setDrawable(model);
}

LPCWSTR PopupView3D::getClassName()
{
    return L"3DView";
}

void PopupView3D::onRegisterClass(WNDCLASSEXW* wc)
{
    int cy = GetSystemMetrics(SM_CYICON);
    int cx = GetSystemMetrics(SM_CXICON);
    wc->hIcon = (HICON)LoadImageW(hModule, (LPCWSTR)0x8C, IMAGE_ICON, cx, cy, LR_SHARED);
    int cySm = GetSystemMetrics(SM_CYSMICON);
    int cxSm = GetSystemMetrics(SM_CXSMICON);
    wc->hIconSm = (HICON)LoadImageW(hModule, (LPCWSTR)0x8C, IMAGE_ICON, cxSm, cySm, LR_SHARED);
}

DWORD PopupView3D::getStyle()
{
    return 0x80CC0000;
}

bool PopupView3D::onClose()
{
    show(false);
    return false;
}
