#pragma once
#include <windows.h>
#include "SDataView.h"
#include "ItemsTab.h"

class String;
class RecordSet;
class Serializable;
namespace Data { class Items; }

class ItemView : public SDataView
{
public:
    ItemView(void* file, Serializable* data);
    virtual ~ItemView();

    bool onCreate() override;
    void onDestroy() override;
    void onActivate(bool active) override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onInitMenu(HMENU menu) override;
    bool onNotify(NMHDR* hdr, LRESULT* result) override;

    void updateMenu(HMENU menu) override;
    void fillScriptMenu(HMENU menu) override;
    LPCWSTR getCommandHelp(UINT id) override;
    void setFilterIndex(int index) override;
    int getFilterIndex() override;
    COMDLG_FILTERSPEC* getFilters(int* count) override;

    String getMergeFilter() override;
    void onDataUpdated() override;
    bool exportData(String* out, DWORD format) override;
    const wchar_t* getSQLTableName() override;
    void importSQLRow(RecordSet* rs, bool captions) override;
    void setViewMode(int mode) override;
    int getViewMode() override;

    virtual HRESULT copyToClipboard();

public:
    Data::Items* m_items;
    ItemsTab     m_tabs;
};

extern COMDLG_FILTERSPEC g_itemFilters[3];
