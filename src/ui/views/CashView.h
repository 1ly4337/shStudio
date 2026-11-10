#pragma once
#include <windows.h>
#include "SDataView.h"
#include "ProductTab.h"

class String;
class RecordSet;
class Serializable;
class CashProducts;

class CashView : public SDataView
{
public:
    CashView(void* file, Serializable* data);
    virtual ~CashView();

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onInitMenu(HMENU menu) override;
    void updateMenu(HMENU menu) override;
    int getMenuId() override;
    HRESULT load() override;
    String getMergeFilter() override;
    void onDataUpdated() override;
    bool exportData(String* out, DWORD format) override;
    const wchar_t* getSQLTableName() override;
    void importSQLRow(RecordSet* rs, bool captions) override;
    int getViewMode() override;

public:
    CashProducts* m_products;
    ProductTab    m_tab;
};
static_assert(sizeof(CashView) == 0x124, "CashView size");
