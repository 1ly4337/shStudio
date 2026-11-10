#pragma once
#include <windows.h>
#include "TabCtrl.h"
#include "ImageStrip.h"

#include "ProductTile.h"
class CashView;
class CashProducts;

class ProductTab : public TabCtrl<ProductTile*>
{
public:
    explicit ProductTab(CashView* view);
    virtual ~ProductTab();

    bool onCommand(int code, LPARAM ctrl, int id) override;
    bool onInitMenuPopup(HMENU menu) override;
    LRESULT subclassProc(UINT msg, WPARAM wParam, LPARAM lParam) override;
    DWORD getTabStyle() override;
    bool createPages(RECT* rc) override;
    void onPageChanged(int index) override;

    void setIcons(ImageStrip* icons);
    void clear();
    void rebuild();
    ProductTile* getTile(long bag);

public:
    CashView*     m_owner;
    CashProducts* m_products;
    int           m_tileView;
    ImageStrip   m_icons;
};
static_assert(sizeof(ProductTab) == 0x78, "ProductTab size");

ProductTile** ProductTab_pageAt(HTablePtr<long, ProductTile*>& pages, int index);
