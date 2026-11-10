#pragma once
#include <windows.h>
#include "ToonSkillList.h"

class NoosSkillList : public ToonSkillList
{
public:
    NoosSkillList(ToonSkillView* owner, ToonSkills* skills) : ToonSkillList(owner, skills) {}
    virtual ~NoosSkillList() {}

    void initNewSkill(ToonSkill* skill) override;
};
static_assert(sizeof(NoosSkillList) == 0x124, "NoosSkillList size");
