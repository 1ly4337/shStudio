#include "MobClrSkillList.h"
#include "_imports_G.h"
#include "StudioFrame.h"

MobClrSkillList::MobClrSkillList(NpcSkillView* owner, MobsSkills* skills)
    : NpcSkillList(owner, skills)
{
}

bool MobClrSkillList::onCreate()
{
    bool result = ListCtrlOD::onCreate();
    if (!result)
        return result;
    ImageStrip* icons = g_studioFrame->m_client.m_clientData.getIcons(L"icon_skills");
    if (icons)
        setImages(icons);
    addColumn(L"ID", 50, 16, 'skid');
    addColumn(L"Name", 120, 4096, 'name');
    addColumn(L"State", 50, 18, 'sttp');
    addColumn(L"Type", 130, 0, 'tpdt');
    addColumn(L"Dmg Type", 50, 18, 'dmtp');
    addColumn(L"Desc", 250, 4096, 'desc');
    return true;
}
