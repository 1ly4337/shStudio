#pragma once
#include <windows.h>
#include "SDataView.h"
#include "ItemSetList.h"

class String;
class RecordSet;
class Serializable;
namespace Data { class ItemSets; }

class ItemSetView : public SDataView
{
public:
    ItemSetView(void* file, Serializable* data);
    virtual ~ItemSetView();

    bool onCreate() override;
    bool onInitMenu(HMENU menu) override { return true; }
    COMDLG_FILTERSPEC* getFilters(int* count) override;
    void onDataUpdated() override {}
    bool exportData(String* out, DWORD format) override;
    void importSQLRow(RecordSet* rs, bool captions) override {}

public:
    Data::ItemSets* m_sets;
    ItemSetList     m_list;
};
static_assert(sizeof(ItemSetView) == 0x1A8, "ItemSetView size");
