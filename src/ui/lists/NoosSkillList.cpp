#include "NoosSkillList.h"
#include "ToonSkill.h"
#include "_imports_p09.h"
#include "_imports_g1b.h"

void NoosSkillList::initNewSkill(ToonSkill* skill)
{
    skill->m_levels[0].typeShow = 0;
    skill->m_levels[0].typeDetail = 82;
}
