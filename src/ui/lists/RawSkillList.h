#pragma once
#include <windows.h>
#include "ToonSkillList.h"

class RawSkillList : public ToonSkillList
{
public:
    RawSkillList(ToonSkillView* owner, ToonSkills* skills) : ToonSkillList(owner, skills) {}
    virtual ~RawSkillList() {}

    DWORD getStyle() override;
    bool onCreate() override;
    DWORD getExStyle() override;
    void initNewSkill(ToonSkill* skill) override {}
};
static_assert(sizeof(RawSkillList) == 0x124, "RawSkillList size");
