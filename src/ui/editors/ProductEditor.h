#pragma once
#include <windows.h>
#include "Modal.h"
#include "IconPicker.h"
#include "EditText.h"
#include "ComboBox.h"

class CashProduct;
class CashProducts;
class ImageStrip;

class ProductEditor : public Modal
{
public:
    ProductEditor(CashProduct* product, CashProducts* products, ImageStrip* icons);
    virtual ~ProductEditor();

    bool onCreate() override;
    void reserved73() override;
    int mapCommandToResult(int id) override;

public:
    CashProducts* m_products;
    CashProduct*  m_product;
    ImageStrip*  m_icons;
    IconPicker    m_picker;
    EditText      m_name;
    EditText      m_desc;
    ComboBox      m_code;
    ComboBox      m_bag;
    EditText      m_cost;
    EditText      m_cost2;
    EditText      m_itemIds[24];
    EditText      m_counts[24];
};
static_assert(sizeof(ProductEditor) == 0x4C0, "ProductEditor size");
