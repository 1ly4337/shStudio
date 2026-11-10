#include "p05_common.h"
#include "ModelView.h"

ModelView::~ModelView()
{
}

LPCWSTR ModelView::getClassName()
{
    return L"ModelView";
}

void ModelView::onRegisterClass(WNDCLASSEXW* wc)
{
    int cy = GetSystemMetrics(SM_CYICON);
    int cx = GetSystemMetrics(SM_CXICON);
    wc->hIcon = (HICON)LoadImageW(hModule, (LPCWSTR)0x86, IMAGE_ICON, cx, cy, LR_SHARED);
    int cySm = GetSystemMetrics(SM_CYSMICON);
    int cxSm = GetSystemMetrics(SM_CXSMICON);
    wc->hIconSm = (HICON)LoadImageW(hModule, (LPCWSTR)0x86, IMAGE_ICON, cxSm, cySm, LR_SHARED);
}
