#pragma once
#include <windows.h>
#include "NpcSkillList.h"

class MobRawSkillList : public NpcSkillList
{
public:
    MobRawSkillList(NpcSkillView* owner, MobsSkills* skills);

    DWORD getStyle() override;
    bool onCreate() override;
    DWORD getExStyle() override;

};
static_assert(sizeof(MobRawSkillList) == 0x114, "MobRawSkillList size");
