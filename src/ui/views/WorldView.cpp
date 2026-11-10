#include "p05_common.h"
#include "WorldView.h"

WorldView::WorldView(FileTree* node, int worldId)
    : DocView(node, &m_world),
      m_list(this, &m_world),
      m_world(worldId),
      m_popup()
{
    m_558 = 0;
}

WorldView::~WorldView()
{
}

LPCWSTR WorldView::getClassName()
{
    return L"WorldView";
}

void WorldView::onRegisterClass(WNDCLASSEXW* wc)
{
    int cy = GetSystemMetrics(SM_CYICON);
    int cx = GetSystemMetrics(SM_CXICON);
    wc->hIcon = (HICON)LoadImageW(hModule, (LPCWSTR)0x89, IMAGE_ICON, cx, cy, LR_SHARED);
    int cySm = GetSystemMetrics(SM_CYSMICON);
    int cxSm = GetSystemMetrics(SM_CXSMICON);
    wc->hIconSm = (HICON)LoadImageW(hModule, (LPCWSTR)0x89, IMAGE_ICON, cxSm, cySm, LR_SHARED);
}

int WorldView::getMenuId()
{
    return 175;
}

void WorldView::onActivated()
{
    SetFocus(m_list.m_hWnd);
}

void WorldView::fillScriptMenu(HMENU menu)
{
    Popup::appendMenuItem(menu, 0xE411);
    Popup::appendMenuItem(menu, 0xE435);
    Popup::appendMenuItem(menu, 0xE430);
}

COMDLG_FILTERSPEC g_worldFilters[1] = { { L"World File", L"*.wld" } };

COMDLG_FILTERSPEC* WorldView::getFilters(int* count)
{
    *count = 1;
    return g_worldFilters;
}
