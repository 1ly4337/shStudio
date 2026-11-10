#pragma once
#include <windows.h>
#include "SDataView.h"
#include "MonsterList.h"

class Monsters;
class Serializable;
class String;
class RecordSet;

class MonsterView : public SDataView
{
public:
    MonsterView(void* file, Serializable* data);
    virtual ~MonsterView();

    bool onCreate() override;
    void onActivate(bool active) override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void updateMenu(HMENU menu) override;
    void fillScriptMenu(HMENU menu) override;
    LPCWSTR getCommandHelp(UINT id) override;
    HRESULT load() override;
    HRESULT store() override;
    void onDataUpdated() override;
    bool exportData(String* out, DWORD format) override;
    const wchar_t* getSQLTableName() override;
    void importSQLRow(RecordSet* rs, bool captions) override;
    void setViewMode(int mode) override;
    int getViewMode() override;

    MonsterList m_list;
    Monsters*   m_monsters;
};
static_assert(sizeof(MonsterView) == 0x1C8, "MonsterView size");
