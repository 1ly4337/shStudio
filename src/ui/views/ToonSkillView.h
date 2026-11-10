#pragma once
#include <windows.h>
#include "SDataView.h"
#include "ToonSkillTab.h"
#include "String.hpp"

class Serializable;
class RecordSet;
class ToonSkills;
class ToonSkill;

class ToonSkillView : public SDataView
{
public:
    ToonSkillView(void* file, Serializable* data);
    virtual ~ToonSkillView();

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onInitMenu(HMENU menu) override;

    void updateMenu(HMENU menu) override;
    bool save() override;

    String getMergeFilter() override;
    void importSQLDefinition() override;
    void onDataUpdated() override;
    bool exportData(String* out, DWORD format) override;
    const wchar_t* getSQLTableName() override;
    void importSQLRow(RecordSet* rs, bool captions) override;
    void setViewMode(int mode) override;
    int getViewMode() override;

    virtual LRESULT clearLists();
    virtual bool removeSkill(ToonSkill* skill);

    void dumpSkills();

public:
    ToonSkills*  m_skills;
    ToonSkillTab m_tabs;
};
static_assert(sizeof(ToonSkillView) == 0xEC, "ToonSkillView size");
