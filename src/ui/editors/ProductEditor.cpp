#include "ProductEditor.h"
#include "CashProducts.h"
#include "CashProduct.h"
#include "AlertDial.h"
#include "gdi_cache.h"
#include "String.hpp"
#include "String_p02.h"
#include "cash_g3.h"
#include "_imports_p07.h"

static inline int*& g3_stripIds(ImageStrip& s)    { return FIELD(int*, &s, 0x08); }
static inline int&  g3_stripIdCap(ImageStrip& s)  { return FIELD(int, &s, 0x10); }

ProductEditor::ProductEditor(CashProduct* product, CashProducts* products, ImageStrip* icons)
    : Modal()
    , m_picker()
    , m_name()
    , m_desc()
    , m_code()
    , m_bag()
    , m_cost()
    , m_cost2()
{
    m_products = products;
    m_product  = product;
    m_icons    = icons;
    m_code.m_dropLines = 2;
    m_code.m_flags = 0;
    m_bag.m_dropLines = 3;
    m_bag.m_flags = 0;
}

ProductEditor::~ProductEditor()
{
}

bool ProductEditor::onCreate()
{
    ImageStrip* icons = m_icons;
    m_picker.m_bitmaps.ImageStrip::operator=(*icons);
    m_picker.m_cells.ImageStrip::operator=(*icons);
    m_picker.create(m_hWnd, (HMENU)110, 20, 23, 34, 34, 32, 0);

    int count = g3_stripIdCap(m_picker.m_bitmaps);
    int sel = 0;
    if (count <= 0) {
        sel = -1;
    } else {
        int* ids = g3_stripIds(m_picker.m_bitmaps);
        while (*ids != m_product->m_icon) {
            ++sel;
            ++ids;
            if (sel >= count) {
                sel = -1;
                break;
            }
        }
    }
    m_picker.m_selected = sel;
    m_picker.showIcon(sel);
    m_picker.m_dropDown.m_selected = sel;

    g3_createLabel(m_hWnd, L"Product Name:", 95, 22, 75, 18, 4);
    g3_createEdit(m_name, m_hWnd, L"", 0x50010080u, 170, 20, 200, 18, 0x65);
    SendMessageW(m_name.m_hWnd, EM_LIMITTEXT, 0x32u, 0);
    m_name.setFontId(6);
    m_name.setText(m_product->m_name.m_buffer);

    g3_createLabel(m_hWnd, L"Product Code:", 95, 45, 75, 18, 4);
    m_code.m_hWnd = CreateWindowExW(0x200u, L"ComboBox", 0, (m_code.m_dropLines & 3) | 0x50210900,
                                    170, 42, 200, 18, m_hWnd, (HMENU)0x66, 0, 0);
    if (m_code.m_hWnd)
        m_code.setFontId(m_code.m_fontId);
    else
        GetLastError();
    SendMessageW(m_code.m_hWnd, CB_LIMITTEXT, 0x14u, 0);
    int nProducts = m_products->size();
    for (int i = 0; i < nProducts; ++i) {
        const wchar_t* code = m_products->m_items[i]->m_code.m_buffer;
        if (SendMessageW(m_code.m_hWnd, CB_ADDSTRING, 0, (LPARAM)code) != -1) {
            int index = (int)SendMessageW(m_code.m_hWnd, CB_GETCOUNT, 0, 0);
            m_code.m_items.put(index, code);
        }
    }
    SendMessageW(m_code.m_hWnd, CB_SELECTSTRING, 0xFFFFFFFF, (LPARAM)m_product->m_code.m_buffer);

    g3_createLabel(m_hWnd, L"Product Description:", 20, 74, 100, 16, 4);
    g3_createEdit(m_desc, m_hWnd, L"", 0x50211044u, 20, 92, 365, 74, 0x67);
    SendMessageW(m_desc.m_hWnd, EM_LIMITTEXT, 0x200u, 0);
    m_desc.setFontId(6);
    m_desc.setText(m_product->m_desc.m_buffer);

    g3_createLabel(m_hWnd, L"Bag:", 20, 180, 30, 18, 4);
    m_bag.m_hWnd = CreateWindowExW(0, L"ComboBoxEx32", 0, (m_bag.m_dropLines & 3) | 0x50010000,
                                   45, 176, 125, 120, m_hWnd, (HMENU)0x68, 0, 0);
    if (m_bag.m_hWnd) {
        m_bag.setFontId(m_bag.m_fontId);
        SendMessageW(m_bag.m_hWnd, CB_SETEXTENDEDUI, 1u, 0);
        m_bag.m_flags = 1;
    } else {
        GetLastError();
    }
    int nBags = m_products->sub_420770();
    for (int i = 0; i < nBags; ++i) {
        vLong& bags = m_products->m_bags;
        int bag = (i < 0 || i >= bags.m_capacity) ? 0 : bags.m_items[i];
        m_bag.addItem(g3_bagName(g_bagNames, bag), bag);
    }
    m_bag.selectData(m_product->m_bag);

    g3_createLabel(m_hWnd, L"Cost:", 190, 180, 25, 18, 4);
    m_cost.createNumber(m_hWnd,(HMENU) (HMENU)105, 220, 177, 50, 19, m_product->m_cost, 0, 0);
    g3_createLabel(m_hWnd, L"Cost(2):", 280, 180, 40, 18, 4);
    m_cost2.createNumber(m_hWnd,(HMENU) (HMENU)106, 330, 177, 50, 19, m_product->m_cost2, 0, 0);
    m_cost2.enable((m_product->m_version & 0xFFF) >= 0x700);

    int columnX[3] = { 20, 150, 280 };
    int k = 0;
    for (int col = 0; col < 3; ++col) {
        int x = columnX[col];
        g3_createLabel(m_hWnd, L"Item ID", x + 25, 213, 50, 18, 4);
        g3_createLabel(m_hWnd, L"Count", x + 80, 213, 50, 18, 4);
        int y = 231;
        for (int row = 8; row; --row) {
            {
                String label = String(L"# ") + (k + 1);
                g3_createLabel(m_hWnd, label.m_buffer, x, y + 2, 25, 18, 4);
            }
            m_itemIds[k].createNumber(m_hWnd,(HMENU) (HMENU)(INT_PTR)(k + 201), x + 25, y, 50, 18,
                                    m_product->m_itemId[k], 0x10000000, 0);
            m_counts[k].createNumber(m_hWnd,(HMENU) (HMENU)(INT_PTR)(k + 301), x + 80, y, 30, 18,
                                   m_product->m_itemCount[k], 0x10000000, 0);
            y += 25;
            ++k;
        }
    }
    return Modal::onCreate();
}

void ProductEditor::reserved73()
{
    m_picker.m_dropDown.show(false);
}

int ProductEditor::mapCommandToResult(int id)
{
    if (id != 1)
        return id;

    String name;
    m_name.getText(&name);
    String desc;
    m_desc.getText(&desc);
    String code;
    m_code.getText(&code);

    int bag;
    if (m_bag.m_hWnd) {
        LRESULT cur = SendMessageW(m_bag.m_hWnd, CB_GETCURSEL, 0, 0);
        bag = (int)SendMessageW(m_bag.m_hWnd, CB_GETITEMDATA, cur, 0);
    } else {
        bag = 0;
    }
    int iconIndex = m_picker.m_selected;
    int icon = (iconIndex < 0 || iconIndex >= g3_stripIdCap(m_picker.m_bitmaps)) ? 0
             : g3_stripIds(m_picker.m_bitmaps)[iconIndex];
    int cost = m_cost.getInt(0);

    const wchar_t* error;
    if (!name.m_length) {
        error = L"You must provide a name (up to 50 char.) for the product.";
    } else if (!code.m_length) {
        error = L"You must provide a code (up to 20 char.) for the product.";
    } else if (!desc.m_length) {
        error = L"You must provide a description for the product.";
    } else if (bag < 0) {
        error = L"You must select a bag to insert product into.";
    } else if (icon <= 0 && m_picker.m_hWnd) {
        error = L"You must select an icon for the product.";
    } else {
        error = nullptr;
        int n = m_products->size();
        for (int i = 0; i < n; ++i) {
            CashProduct* other = m_products->m_items[i];
            if (other->m_code.m_length == code.m_length
                && _wcsnicmp(other->m_code.m_buffer, code.m_buffer, other->m_code.m_length) == 0
                && other != m_product) {
                error = L"The given code is already used by another product.";
                break;
            }
        }
    }
    if (error) {
        AlertDial::alert(m_hWnd, error, L"Cash Product", MB_ICONERROR);
        return -1;
    }

    m_product->m_name = name;
    m_product->m_code = code;
    m_product->m_desc = desc;
    m_product->m_bag = bag;
    if (m_picker.m_hWnd)
        m_product->m_icon = icon;
    m_product->m_cost = cost;
    if ((m_product->m_version & 0xFFF) >= 0x700)
        m_product->m_cost2 = m_cost2.getInt(m_product->m_cost2);

    String& title = m_product->m_title;
    title.clear();
    title.append(name.m_buffer, name.m_length) << L" - " << (long)cost << L" Pts";

    int items = 0;
    for (int k = 0; k < 24; ++k) {
        m_product->m_itemId[k] = m_itemIds[k].getInt(0);
        m_product->m_itemCount[k] = (BYTE)m_counts[k].getInt(0);
        if (m_product->m_itemId[k])
            ++items;
    }
    m_product->m_itemTotal = items;
    return id;
}
