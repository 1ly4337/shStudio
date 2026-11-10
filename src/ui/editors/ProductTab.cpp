#include "ProductTab.h"
#include "ProductTile.h"
#include "CashView.h"
#include "CashProducts.h"
#include "Exception.h"
#include "cash_g3.h"
#include "_imports_p07.h"

ProductTile** ProductTab_pageAt(HTablePtr<long, ProductTile*>& pages, int index)
{
    if (index < 0 || index >= pages.m_capacity)
        throw ArrayIndexOutOfBoundsException(index, pages.m_capacity);
    return &pages.m_table[index].value;
}

ProductTab::ProductTab(CashView* view)
    : TabCtrl<ProductTile*>()
    , m_icons()
{
    m_owner = view;
    m_products = 0;
    m_tileView = 4;
}

ProductTab::~ProductTab()
{
}

DWORD ProductTab::getTabStyle()
{
    return 0x8400;
}

bool ProductTab::onCommand(int code, LPARAM ctrl, int id)
{
    int current = (int)SendMessageW(m_hWnd, TCM_GETCURSEL, 0, 0);
    if (current < 0 || current >= (int)SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0))
        return false;
    int cmd = (int)(DWORD)id;
    if ((unsigned)(cmd - 0xE2A0) > 4)
        return (*ProductTab_pageAt(m_pages, current))->onCommand((int)code, ctrl, cmd);
    m_tileView = cmd - 0xE2A0;
    ProductTile** page = ProductTab_pageAt(m_pages, current);
    if (cmd - 0xE2A0 >= 0)
        (*page)->setView(cmd - 0xE2A0);
    return true;
}

bool ProductTab::onInitMenuPopup(HMENU menu)
{
    UINT item = m_tileView + 0xE2A0;
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, 0x2Cu);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    GetMenuItemInfoW(menu, item, FALSE, &mii);
    if ((mii.fState & 3) == 0) {
        mii.fState = MFS_CHECKED;
        SetMenuItemInfoW(menu, item, FALSE, &mii);
    }
    HWND page = getPage(-1);
    if (page)
        SendMessageW(page, WM_INITMENU, (WPARAM)menu, 0);
    return true;
}

LRESULT ProductTab::subclassProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == 0x407)
        return 130;
    return TabControl::subclassProc(msg, wParam, lParam);
}

bool ProductTab::createPages(RECT* rc)
{
    if (!m_products)
        return false;
    int n = m_products->sub_420770();
    for (int i = 0; i < n; ++i) {
        vLong& bags = m_products->m_bags;
        int bag = (i < 0 || i >= bags.m_capacity) ? 0 : bags.m_items[i];
        ProductTile* tile = new ProductTile(m_owner, this, bag, m_products->serialV5());
        if (!addPage(tile, bag, g3_bagName(g_bagTabNames, bag), -1))
            return false;
    }
    return true;
}

void ProductTab::onPageChanged(int index)
{
    LVITEMW lvi;
    memset(&lvi, 0, sizeof(lvi));
    lvi.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
    lvi.state = 0;
    SendMessageW((*ProductTab_pageAt(m_pages, index))->m_hWnd, LVM_SETITEMSTATE, 0xFFFFFFFF, (LPARAM)&lvi);
    ProductTile* tile = *ProductTab_pageAt(m_pages, index);
    int view = m_tileView;
    if (view <= 4) {
        tile->m_view = view;
        if (IsWindow(tile->m_hWnd))
            SendMessageW(tile->m_hWnd, LVM_SETVIEW, view, 0);
    }
    SetFocus((*ProductTab_pageAt(m_pages, index))->m_hWnd);
}

void ProductTab::setIcons(ImageStrip* icons)
{
    m_icons.ImageStrip::operator=(*icons);
    for (int i = 0; i < (int)SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0); ++i) {
        ProductTile* tile = *ProductTab_pageAt(m_pages, i);
        if (tile)
            tile->setImages(&m_icons);
    }
}

void ProductTab::clear()
{
    while ((int)SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0) > 4)
        destroyPage((int)SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0) - 1);
    for (int i = 0; i < (int)SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0); ++i) {
        ProductTile* tile = pageAt(i);
        SendMessageW(tile->m_hWnd, LVM_DELETEALLITEMS, 0, 0);
        for (int k = 0; k < tile->m_products.m_count; ++k)
            tile->m_products.m_items[k] = 0;
        tile->m_products.m_count = 0;
        tile->invalidate(true);
        UpdateWindow(tile->m_hWnd);
    }
}

void ProductTab::rebuild()
{
    int pages = (int)SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0);
    for (int i = 0; i < pages; ++i) {
        ProductTile* tile = *ProductTab_pageAt(m_pages, i);
        SendMessageW(tile->m_hWnd, LVM_DELETEALLITEMS, 0, 0);
        for (int k = 0; k < tile->m_products.m_count; ++k)
            tile->m_products.m_items[k] = 0;
        tile->m_products.m_count = 0;
    }
    int n = m_products->sub_420770();
    for (int i = 0; i < n; ++i) {
        vLong& bags = m_products->m_bags;
        int bag = (i < 0 || i >= bags.m_capacity) ? 0 : bags.m_items[i];
        if (!getPageByKey(bag)) {
            ProductTile* tile = new ProductTile(m_owner, this, bag, m_products->serialV5());
            if (!addPage(tile, bag, g3_bagName(g_bagTabNames, i + 1), -1))
                return;
        }
    }
    int count = (int)SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0);
    for (int j = 0; j < count; ++j)
        (*ProductTab_pageAt(m_pages, j))->fill(m_products);
}

ProductTile* ProductTab::getTile(long bag)
{
    int n = m_pages.m_count;
    int i = 0;
    if (n > 0) {
        HTable<long, ProductTile*>::Entry* table = m_pages.m_table;
        for (HTable<long, ProductTile*>::Entry* e = table; e->key != bag; ++e) {
            if (++i >= n)
                goto create;
        }
        if (i != -1 && table[i].value)
            return table[i].value;
    }
create:
    ProductTile* tile = new ProductTile(m_owner, this, bag, m_products->serialV5());
    int count = (int)SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0);
    if (!addPage(tile, bag, g3_bagName(g_bagTabNames, count), -1))
        return 0;
    n = m_pages.m_count;
    if (n <= 0)
        return 0;
    HTable<long, ProductTile*>::Entry* table = m_pages.m_table;
    i = 0;
    for (HTable<long, ProductTile*>::Entry* e = table; e->key != bag; ++e) {
        if (++i >= n)
            return 0;
    }
    if (i == -1)
        return 0;
    return table[i].value;
}
