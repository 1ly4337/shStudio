#pragma once
#include <windows.h>
#include "ptrArray.h"
#include "ToonSkill.h"

class ToonSkills;

class ToonSkillSet : public ptrArray<ToonSkill*>
{
public:
    ToonSkillSet() : ptrArray<ToonSkill*>(128) {}
    virtual ~ToonSkillSet();

    void forgetAll()
    {
        for (int i = 0; i < m_count; ++i)
            m_items[i] = 0;
        m_count = 0;
    }

    int fill(ToonSkills* skills, int mode, int classIndex);
};
static_assert(sizeof(ToonSkillSet) == 0x18, "ToonSkillSet size");
