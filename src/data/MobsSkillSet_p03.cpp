#include "MobsSkillSet.h"
#include "MobsSkills.h"
#include "MobsSkill.h"
#include "_imports_w1.h"

int MobsSkillSet::MobsSkillSet::sub_44D4A0(MobsSkills* skills)
{
    for (int i = 0; i < m_count; ++i)
        m_items[i] = 0;
    m_count = 0;
    int n = skills->size();
    for (int i = 0; i < n; ++i)
    {
        MobsSkill* skill = skills->get(i);
        if (skill && skill->m_skill.typeDetail)
            add(skill);
    }
    return m_count;
}
