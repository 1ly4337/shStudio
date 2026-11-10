#pragma once
#include <windows.h>
#include "TabCtrl.h"
#include "NpcSkillList.h"

class NpcSkillView;
class MobsSkills;

class NpcSkillTab : public TabCtrl<NpcSkillList*>
{
public:
    explicit NpcSkillTab(NpcSkillView* view) : m_view(view), m_skills(0) {}

    DWORD getTabStyle() override;
    bool createPages(RECT* rc) override;

    void refresh();
    LRESULT clearAll();

public:
    NpcSkillView* m_view;
    MobsSkills*   m_skills;
};
static_assert(sizeof(NpcSkillTab) == 0x40, "NpcSkillTab size");
