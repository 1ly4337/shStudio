#pragma once
#include <windows.h>
#include "SDataView.h"
#include "NpcTab.h"

class NpcData;
class Serializable;
class RecordSet;

class NpcView : public SDataView
{
public:
    NpcView(void* file, Serializable* data);
    virtual ~NpcView();

    bool onCreate() override;
    void onActivate(bool active) override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onInitMenu(HMENU menu) override;
    void updateMenu(HMENU menu) override;
    void fillScriptMenu(HMENU menu) override;
    HRESULT load() override;
    void onDataUpdated() override {}
    bool exportData(String* out, DWORD format) override;
    void importSQLRow(RecordSet* rs, bool captions) override {}
    void setViewMode(int mode) override;
    int getViewMode() override;

public:
    NpcData* m_npcData;
    NpcTab   m_tab;
};
