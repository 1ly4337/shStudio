#include "CashView.h"
#include "CashProducts.h"
#include "CashProduct.h"
#include "ProductTile.h"
#include "StudioFrame.h"
#include "String.hpp"
#include "cash_g3.h"
#include "_imports_p07.h"

CashView::CashView(void* file, Serializable* data)
    : SDataView(file, data)
    , m_tab(this)
{
    m_margin = 6;
    if (data) {
        m_products = dynamic_cast<CashProducts*>(data);
    } else {
        CashProducts* products = new CashProducts(1024);
        m_products = products;
        Serializable* serial = products ? static_cast<Serializable*>(products) : 0;
        m_content = serial;
        FIELD(Serializable*, this, 0x50) = serial;
    }
    m_tab.m_products = m_products;
}

CashView::~CashView()
{
}

bool CashView::onCreate()
{
    if (!g_studioFrame)
        return false;
    if (!SDataView::onCreate())
        return false;
    StudioFrame* frame = g_studioFrame;
    m_bkColor = 0xEEEEEE;
    ImageStrip* icons = frame->m_client.m_clientData.getIcons(L"icon_somo2");
    RECT rc;
    getContentRect(&rc);
    if (!m_tab.create(m_hWnd, (HMENU)0x65, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
                               0x50000000u, 0, 0))
        return false;
    if (icons)
        m_tab.setIcons(icons);
    m_tab.rebuild();
    return true;
}

bool CashView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) != 1 || ctrl)
        return false;
    switch (id) {
    case 0xE12A:
    case 0xE12B:
        return m_tab.onCommand(code, 0, (WORD)id);
    case 0xE401:
        m_products->setCodePage(0x400);
        return true;
    case 0xE405: {
        short version = (short)m_products->serialV5();
        m_products->setCodePage((version & 0xF000) | 0x700);
        return true;
    }
    default:
        return SDataView::onCommand(code, 0, id);
    }
}

bool CashView::onInitMenu(HMENU menu)
{
    SDataView::onInitMenu(menu);
    m_tab.onInitMenuPopup(menu);
    return true;
}

void CashView::updateMenu(HMENU menu)
{
    SDataView::updateMenu(menu);
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, 0x2Cu);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    mii.fState = 0;
    SetMenuItemInfoW(menu, 0xE12A, FALSE, &mii);
    memset(&mii.fMask, 0, 0x2Cu);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE | MIIM_STRING;
    mii.fState = 0;
    mii.dwTypeData = (LPWSTR)L"Episo 4 format, 24 items per product, single price";
    mii.cch = 50;
    SetMenuItemInfoW(menu, 0xE401, FALSE, &mii);
    memset(&mii.fMask, 0, 0x2Cu);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE | MIIM_STRING;
    mii.fState = 0;
    mii.dwTypeData = (LPWSTR)L"Episo 7 format, 24 items per product, 2 prices";
    mii.cch = 46;
    SetMenuItemInfoW(menu, 0xE405, FALSE, &mii);
}

int CashView::getMenuId()
{
    return 173;
}

HRESULT CashView::load()
{
    m_tab.clear();
    HRESULT result = SDataView::load();
    m_tab.rebuild();
    return result;
}

String CashView::getMergeFilter()
{
    String filter;
    filter.append(L"ItemSell SData File\tDBItemSellText_*.SData\t", (unsigned)-1);
    return filter;
}

void CashView::onDataUpdated()
{
    setStatusText(0, 0);
    setModified(true);
    m_tab.rebuild();
}

bool CashView::exportData(String* out, DWORD format)
{
    m_products->toText(*out, format);
    return true;
}

const wchar_t* CashView::getSQLTableName()
{
    return L"PS_GameDefs.dbo.ProductList";
}

void CashView::importSQLRow(RecordSet* rs, bool captions)
{
    const wchar_t* code;
    void* source = FIELD(void*, rs, 0x04);
    if (source) {
        void* row = ((void* (__thiscall*)(void*))((*(void***)source)[1]))(source);
        code = ((const wchar_t* (__thiscall*)(void*, const wchar_t*))((*(void***)row)[4]))(row, L"ProductCode");
    } else {
        code = 0;
    }
    String key;
    key.append(code, (unsigned)-1);
    CashProduct* product = m_products->sub_420800(key.m_buffer);
    if (!product) {
        int row = m_products->size();
        product = new CashProduct(m_products->serialV5(), row, 0);
        m_products->add(product);
    }
    product->loadRecord(*rs, captions);
}

int CashView::getViewMode()
{
    if (m_products)
        return m_products->serialV5();
    return 0;
}
