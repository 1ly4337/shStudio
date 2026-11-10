#pragma once
#include <windows.h>
#include "SDataView.h"
#include "NpcSkillTab.h"
#include "String.hpp"

class Serializable;
class RecordSet;
class MobsSkills;
class MobsSkill;

class NpcSkillView : public SDataView
{
public:
    NpcSkillView(void* file, Serializable* data);
    virtual ~NpcSkillView();

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onInitMenu(HMENU menu) override;

    void updateMenu(HMENU menu) override;
    bool save() override;
    HRESULT load() override;

    String getMergeFilter() override;
    void importSQLDefinition() override;
    void onDataUpdated() override;
    bool exportData(String* out, DWORD format) override;
    const wchar_t* getSQLTableName() override;
    void importSQLRow(RecordSet* rs, bool captions) override;
    void setViewMode(int mode) override;
    int getViewMode() override;

    virtual LRESULT clearLists();
    virtual bool addSkill(MobsSkill* skill);
    virtual bool removeSkill(MobsSkill* skill);

public:
    MobsSkills* m_skills;
    NpcSkillTab m_tabs;
};
static_assert(sizeof(NpcSkillView) == 0xEC, "NpcSkillView size");
