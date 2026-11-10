#pragma once
#include <windows.h>
#include "ToonSkillList.h"

class PersoSkillList : public ToonSkillList
{
public:
    PersoSkillList(ToonSkillView* owner, ToonSkills* skills) : ToonSkillList(owner, skills)
    {
        m_allClasses = true;
        m_listKind = 1;
        m_useRawEditor = false;
    }
    virtual ~PersoSkillList() {}

    bool onCommand(int code, LPARAM ctrl, int id) override;
    int getHeaderMenu() override;
    void onContextMenu(HMENU menu) override;
    void initNewSkill(ToonSkill* skill) override;

public:
    bool m_allClasses;
};
static_assert(sizeof(PersoSkillList) == 0x128, "PersoSkillList size");
