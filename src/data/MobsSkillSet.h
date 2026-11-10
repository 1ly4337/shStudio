#pragma once
#include <windows.h>
#include "ptrArray.h"
#include "MobsSkill.h"

class MobsSkills;

class MobsSkillSet : public ptrArray<MobsSkill*>
{
public:
    MobsSkillSet() : ptrArray<MobsSkill*>(128) {}
    virtual ~MobsSkillSet();

    int MobsSkillSet::sub_44D4A0(MobsSkills* skills);

};
static_assert(sizeof(MobsSkillSet) == 0x18, "MobsSkillSet size");
