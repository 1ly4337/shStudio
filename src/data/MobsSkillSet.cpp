#include "MobsSkillSet.h"

MobsSkillSet::~MobsSkillSet()
{
    for (int i = 0; i < m_count; ++i)
        m_items[i] = 0;
    m_count = 0;
}
