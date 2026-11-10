#include "ProductTile.h"
#include "ProductTab.h"
#include "CashView.h"
#include "ProductEditor.h"
#include "CashProduct.h"
#include "StudioFrame.h"
#include "AlertDial.h"
#include "Clipboard.h"
#include "menus.h"
#include "gdi_cache.h"
#include "primitiveArray.h"
#include "String.hpp"
#include "cash_g3.h"
#include "_imports_p07.h"

static inline int g3_sqlExecute(void* db, const wchar_t* sql)
{
    return ((int(__thiscall*)(void*, const wchar_t*))((*(void***)db)[10]))(db, sql);
}

ProductTile::ProductTile(CashView* view, ProductTab* tab, long bag, int version)
    : ListCtrl()
    , m_products(version)
{
    m_owner = view;
    m_tab = tab;
    m_bag = bag;
    m_view = 4;
}

ProductTile::~ProductTile()
{
    for (int i = 0; i < m_products.m_count; ++i)
        m_products.m_items[i] = 0;
    m_products.m_count = 0;
}

bool ProductTile::onCreate()
{
    bool ok = ListCtrl::onCreate();
    if (!ok)
        return ok;
    if (IsWindow(m_hWnd)) {
        LRESULT ex = SendMessageW(m_hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
        SendMessageW(m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, ex | LVS_EX_FULLROWSELECT);
    }
    addColumn(L"Name", 200, 0, 'name');
    addColumn(L"Cost", 50, 18, 'cost');
    addColumn(L"Code", 80, 0, 'code');
    addColumn(L"Description", 250, 0, 'desc');
    primitiveArray<unsigned int> columns(16);
    columns.add(0);
    columns.add(2);
    columns.add(3);
    setFont(getStdFont(6));
    SIZE tile;
    tile.cx = 320;
    tile.cy = 52;
    setTileView(0, &tile, &columns);
    return true;
}

bool ProductTile::onCommand(int code, LPARAM ctrl, int id)
{
    if (id == 0xE12B) {
        deleteSelected(L"to delete");
        return true;
    }
    if (id != 0xE12A)
        return ListCtrl::onCommand(code, ctrl, id);

    CashProduct* product = new CashProduct(m_products.serialV5(), 0, 0);
    CashProducts* all = m_tab->m_products;
    product->m_bag = m_bag;
    ProductEditor dlg(product, all, &m_tab->m_icons);
    if (dlg.createDialog(m_owner->m_hWnd, 420, 520, 1, L"Cash Product") && dlg.doModal() == 1) {
        m_owner->setModified(true);
        all->add(product);
        m_products.add(product);
        ProductTile* tile = m_tab->getPageByKey(product->m_bag);
        int index = tile->m_products.insert(product, -1);
        tile->insertItem(product, index);
    } else {
        delete product;
    }
    return true;
}

bool ProductTile::onInitMenuPopup(HMENU menu)
{
    BOOL selected = getCount(1) > 0;
    setMenuItemState(menu, 0xE122, selected);
    setMenuItemState(menu, 0xE123, selected);
    setMenuItemState(menu, 0xE12B, selected);
    setMenuItemState(menu, 0xE12C, selected);
    int format = (int)getClipboardFormat();
    int paste = 0;
    if (format > 512) {
        bool found = false;
        if (OpenClipboard(g_clipboard.m_hOwner)) {
            UINT f = EnumClipboardFormats(0);
            if (f) {
                while (f != (UINT)format) {
                    f = EnumClipboardFormats(f);
                    if (!f)
                        break;
                }
                if (f)
                    found = true;
            }
            CloseClipboard();
            paste = found ? 1 : 0;
        }
    }
    setMenuItemState(menu, 0xE124, paste);
    return true;
}

LRESULT ProductTile::windowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    return ListCtrl::windowProc(msg, wParam, lParam);
}

int ProductTile::compareItems(Node* a, Node* b)
{
    CashProduct* pa = dynamic_cast<CashProduct*>(a);
    CashProduct* pb = dynamic_cast<CashProduct*>(b);
    if (!pa)
        return 1;
    if (!pb)
        return -1;
    int r = pa->compare(pb, m_sortColumn);
    if (!m_sortAscending)
        return -r;
    return r;
}

bool ProductTile::canMoveItem(int from, int to)
{
    CashProduct* product = m_products.detach(from);
    if (!product)
        return false;
    m_owner->setModified(true);
    deleteItem(from);
    int index;
    if (to < m_products.size()) {
        int at = to;
        if (from < to)
            at = to - 1;
        index = m_products.insert(product, at);
    } else {
        index = m_products.insert(product, -1);
    }
    insertItem(product, index);
    m_owner->setModified(true);
    return true;
}

bool ProductTile::onActivateItem(int index, int keys)
{
    if (index < 0 || index >= m_products.size())
        return false;
    CashProduct* product = m_products.m_items[index];
    if (!product)
        return false;

    ProductEditor dlg(product, m_tab->m_products, &m_tab->m_icons);
    if (dlg.createDialog(m_owner->m_hWnd, 420, 520, 1, L"Cash Product")) {
        int oldBag = product->m_bag;
        if (dlg.doModal() == 1) {
            m_owner->setModified(true);
            if (product->m_bag == oldBag) {
                updateItem(index);
            } else if (m_products.detachItem(product)) {
                deleteItem(index);
                invalidate(false);
                ProductTile* tile = m_tab->getTile(product->m_bag);
                if (tile) {
                    tile->addNode(product, -1);
                } else {
                    product->m_bag = m_bag;
                    addNode(product, -1);
                }
            }
            void* db = g_studioFrame->getSqlConnection();
            if (db && FIELD(bool, db, 0x18)) {
                String sql;
                {
                    String code = String::sqlQuote(product->m_code.m_buffer);
                    (sql << L"DELETE FROM [PS_GameDefs].[dbo].[ProductList] WHERE [ProductCode]=")
                        .append(code.m_buffer, code.m_length);
                }
                g3_sqlExecute(db, sql.m_buffer);
                sql.clear();
                product->toText(sql, 'sql ');
                int error = g3_sqlExecute(db, sql.m_buffer);
                if (!error) {
                    m_owner->setStatusText(L"Product updated in table PS_GameDefs.dbo.ProductList.", 0);
                } else {
                    String message;
                    String& lastError = FIELD(String, db, 0x1C);
                    if (lastError.m_length)
                        message.append(lastError.m_buffer, lastError.m_length);
                    else
                        message << L"Error: " << (long)error;
                    (message << L"\r\n" << L"During execution of:").append(sql.m_buffer, sql.m_length);
                    AlertDial::productListError(m_owner->m_hWnd, message.m_buffer);
                }
            }
        }
    }
    return false;
}

bool ProductTile::removeItem(int index)
{
    CashProduct* product = m_products.get(index);
    if (!product || !m_owner->m_products->detachItem(product))
        return false;
    m_products.detachItem(product);
    m_owner->setModified(true);
    delete product;
    return true;
}

Node* ProductTile::getNodeAt(int index)
{
    if (index < 0 || index >= m_products.size())
        return 0;
    return m_products.m_items[index];
}

int ProductTile::indexOfNode(Node* node)
{
    return m_products.indexOf(dynamic_cast<CashProduct*>(node));
}

Node* ProductTile::createNode()
{
    return new CashProduct(m_products.serialV5(), 0, 0);
}

CashProduct* ProductTile::findNode(CashProduct* like)
{
    for (int i = 0; i < m_products.m_count; ++i) {
        CashProduct* p = m_products.m_items[i];
        if (p->m_code.m_length == like->m_code.m_length
            && _wcsnicmp(p->m_code.m_buffer, like->m_code.m_buffer, p->m_code.m_length) == 0)
            return m_products.m_items[i];
    }
    return 0;
}

int ProductTile::addNode(Node* node, int index)
{
    CashProduct* product = dynamic_cast<CashProduct*>(node);
    if (product) {
        if (product->m_bag == m_bag) {
            if (m_products.add(product))
                return insertItem(node, index);
        } else {
            ProductTile* tile = m_tab->getTile(product->m_bag);
            if (tile)
                return tile->addNode(node, -1);
        }
    }
    return -1;
}

BOOL ProductTile::fill(CashProducts* all)
{
    setRedraw(false);
    int n = all->size();
    for (int i = 0; i < n; ++i) {
        CashProduct* p = all->m_items[i];
        if (p && p->m_bag == m_bag)
            addNode(p, -1);
    }
    setRedraw(true);
    updateAll();
    return invalidate(true);
}
