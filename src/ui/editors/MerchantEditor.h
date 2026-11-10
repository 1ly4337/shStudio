#pragma once
#include <windows.h>
#include "Modal.h"
#include "IconPicker.h"
#include "EditText.h"
#include "ComboBox.h"
#include "ItemsList.h"
#include "StockList.h"

class Merchant;
class Window;

class MerchantEditor : public Modal
{
public:
    MerchantEditor(Window* owner, Merchant* merchant);
    virtual ~MerchantEditor();

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void reserved73() override;
    int mapCommandToResult(int id) override;

    bool create(HWND parent);
    bool updateStock(bool refillFilters);
    static bool edit(Window* owner, Merchant* merchant);
    static bool fillItemTypes(ComboBox* combo, char merchantType, char faction);

public:
    Merchant*    m_merchant;
    IconPicker   m_icon;
    EditText     m_name;
    EditText     m_welcome;
    ComboBox     m_sellTo;
    ComboBox     m_type;
    ItemsList    m_sold;
    HWND         m_soldInfo;
    HWND         m_addButton;
    ComboBox     m_itemType;
    ComboBox     m_itemClass;
    StockList    m_stock;
    HWND         m_stockInfo;
};

extern int g_merchantTypeIds[18];
