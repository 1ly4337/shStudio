#include "p04_common.h"
#include "ToonSkillSet.h"
#include "ToonSkills.h"
#include "ToonSkill.h"

int ToonSkillSet::fill(ToonSkills* skills, int mode, int classIndex)
{
    for (int i = 0; i < m_count; ++i)
        m_items[i] = 0;
    m_count = 0;

    int count = skills->size();
    for (int i = 0; i < count; ++i)
    {
        ToonSkill* skill = skills->get(i);
        if (!skill)
            continue;
        if (mode)
        {
            const SkillData& level1 = skill->m_levels[0];
            if (!FIELD(short, &level1, 0x22))
                continue;
            if (mode == 1)
            {
                if (!FIELD(BYTE, &level1, 0x1F) || (classIndex && !FIELD(BYTE, &level1, 0x16 + classIndex)))
                    continue;
            }
            else if (mode != 2 || FIELD(BYTE, &level1, 0x1F))
            {
                continue;
            }
        }
        add(skill);
    }
    return m_count;
}
