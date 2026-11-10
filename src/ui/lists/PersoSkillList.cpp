#include "PersoSkillList.h"
#include "ToonSkill.h"
#include <cstring>
#include "_imports_p09.h"
#include "_imports_g1b.h"

bool PersoSkillList::onCommand(int code, LPARAM ctrl, int id)
{
    if ((code | 1) != 1 || ctrl || (unsigned int)(id - 1000) > 6)
        return ListCtrlOD::onCommand(code, ctrl, id);
    if (id == 1000)
    {
        m_allClasses = true;
        m_classFilter = 0;
    }
    else
    {
        m_allClasses = false;
        m_classFilter = id - 1000;
    }
    setItemCount(m_skills.fill(m_doc, 1, m_classFilter));
    return true;
}

int PersoSkillList::getHeaderMenu()
{
    return 192;
}

void PersoSkillList::onContextMenu(HMENU menu)
{
    UINT id = m_classFilter + 1000;
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, sizeof(mii) - sizeof(mii.cbSize));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    GetMenuItemInfoW(menu, id, FALSE, &mii);
    if ((mii.fState & 3) == 0)
    {
        mii.fState = MFS_CHECKED;
        SetMenuItemInfoW(menu, id, FALSE, &mii);
    }
}

void PersoSkillList::initNewSkill(ToonSkill* skill)
{
    skill->m_levels[0].typeShow = 1;
}
