#include "MobRawSkillList.h"
#include "MobsSkills.h"
#include "_imports_G.h"

MobRawSkillList::MobRawSkillList(NpcSkillView* owner, MobsSkills* skills)
    : NpcSkillList(owner, skills)
{
}

DWORD MobRawSkillList::getStyle()
{
    return 517;
}

bool MobRawSkillList::onCreate()
{
    bool result = ListCtrlOD::onCreate();
    if (!result)
        return result;
    addColumn(L"ID", 50, 16, 'skid');
    addColumn(L"Name", 120, 4096, 'name');
    addColumn(L"Type", 50, 18, 'tpdt');
    addColumn(L"Icon", 50, 18, 'icon');
    for (int i = 0; i < 2; ++i)
        addColumn(g_skillColumnNames[19 + i], 50, 18, g_skillColumnTags[19 + i]);
    for (int i = 0; i < 5; ++i)
        addColumn(g_skillColumnNames[40 + i], 50, 18, g_skillColumnTags[40 + i]);
    for (int i = 0; i < 6; ++i)
        addColumn(g_skillColumnNames[46 + i], 50, 18, g_skillColumnTags[46 + i]);
    for (int i = 0; i < 11; ++i)
        addColumn(g_skillColumnNames[57 + i], 50, 18, g_skillColumnTags[57 + i]);
    for (int i = 0; i < 6; ++i)
        addColumn(g_skillColumnNames[88 + i], 50, 18, g_skillColumnTags[88 + i]);
    for (int i = 0; i < 6; ++i)
        addColumn(g_skillColumnNames[68 + i], 50, 18, g_skillColumnTags[68 + i]);
    if ((m_skills->m_format & 0xFFF) >= 0x600)
    {
        for (int i = 0; i < 14; ++i)
            addColumn(g_skillColumnNames[74 + i], 50, 18, g_skillColumnTags[74 + i]);
    }
    return true;
}

DWORD MobRawSkillList::getExStyle()
{
    return 48;
}
