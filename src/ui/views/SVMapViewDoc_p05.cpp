#include "p05_common.h"
#include "SVMapViewDoc.h"

void SVMapViewDoc::onRegisterClass(WNDCLASSEXW* wc)
{
    int cy = GetSystemMetrics(SM_CYICON);
    int cx = GetSystemMetrics(SM_CXICON);
    wc->hIcon = (HICON)LoadImageW(hModule, (LPCWSTR)0x89, IMAGE_ICON, cx, cy, LR_SHARED);
    int cySm = GetSystemMetrics(SM_CYSMICON);
    int cxSm = GetSystemMetrics(SM_CXSMICON);
    wc->hIconSm = (HICON)LoadImageW(hModule, (LPCWSTR)0x89, IMAGE_ICON, cxSm, cySm, LR_SHARED);
}

int SVMapViewDoc::getMenuId()
{
    return 164;
}
