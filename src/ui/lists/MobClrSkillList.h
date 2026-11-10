#pragma once
#include <windows.h>
#include "NpcSkillList.h"

class MobClrSkillList : public NpcSkillList
{
public:
    MobClrSkillList(NpcSkillView* owner, MobsSkills* skills);

    bool onCreate() override;

};
static_assert(sizeof(MobClrSkillList) == 0x114, "MobClrSkillList size");
