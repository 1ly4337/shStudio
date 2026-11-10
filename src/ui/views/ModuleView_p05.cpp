#include "p05_common.h"
#include "ModuleView.h"

void ModuleView::setModel(Drawable* model)
{
    m_viewer.setDrawable(model);
}

void ModuleView::onRegisterClass(WNDCLASSEXW* wc)
{
    int cy = GetSystemMetrics(SM_CYICON);
    int cx = GetSystemMetrics(SM_CXICON);
    wc->hIcon = (HICON)LoadImageW(hModule, (LPCWSTR)0x88, IMAGE_ICON, cx, cy, LR_SHARED);
    int cySm = GetSystemMetrics(SM_CYSMICON);
    int cxSm = GetSystemMetrics(SM_CXSMICON);
    wc->hIconSm = (HICON)LoadImageW(hModule, (LPCWSTR)0x88, IMAGE_ICON, cxSm, cySm, LR_SHARED);
}

LPCWSTR ModuleView::getClassName()
{
    return L"Module View";
}

int ModuleView::getMenuId()
{
    return 168;
}

void ModuleView::updateMenu(HMENU menu)
{
    Document::updateMenu(menu);
    enableMenuItem(0xE102, true);
    enableMenuItem(0xE11E, true);
}

LPCWSTR ModuleView::getCommandHelp(UINT id)
{
    switch (id) {
    case 0xE413: return L"Scale...";
    case 0xE430: return L"Import text fields from a language file";
    case 0xE431: return L"Import definition from database";
    }
    const wchar_t* text = L"Change codepage...";
    if (id != 0xE435)
        return 0;
    return text;
}

void ModuleView::fillScriptMenu(HMENU menu)
{
    MENUITEMINFOW mi;
    memset(&mi, 0, sizeof(mi));
    mi.cbSize = sizeof(mi);
    mi.fMask = MIIM_FTYPE;
    mi.fType = MFT_SEPARATOR;
    UINT count = GetMenuItemCount(menu);
    InsertMenuItemW(menu, count, TRUE, &mi);
    Popup::appendMenuItem(menu, 0xE413);
}

COMDLG_FILTERSPEC g_moduleFilters[1] = { { L"Module File", L"*.smod" } };

COMDLG_FILTERSPEC* ModuleView::getFilters(int* count)
{
    *count = 1;
    return g_moduleFilters;
}
