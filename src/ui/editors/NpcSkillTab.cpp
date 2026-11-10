#include "NpcSkillTab.h"
#include "MobClrSkillList.h"
#include "MobRawSkillList.h"
#include "MobsSkills.h"
#include "Exception.h"

DWORD NpcSkillTab::getTabStyle()
{
    return 1024;
}

bool NpcSkillTab::createPages(RECT* rc)
{
    if (!m_skills)
        return false;
    bool result = addPage(new MobClrSkillList(m_view, m_skills), 'mobs', L"Mobs Skills", -1);
    if (result)
        return addPage(new MobRawSkillList(m_view, m_skills), 'raw ', L"Raw Skills", -1);
    return result;
}

void NpcSkillTab::refresh()
{
    if (!m_skills)
        return;
    for (LRESULT i = 0; i < SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0); ++i)
    {
        if (i < 0 || i >= m_pages.m_capacity)
            throw ArrayIndexOutOfBoundsException((long)i, (long)m_pages.m_capacity);
        NpcSkillList* list = m_pages.m_table[i].value;
        if (list)
            list->setItemCount(list->m_set.MobsSkillSet::sub_44D4A0(m_skills));
    }
    HWND hWnd = m_hWnd;
    if (hWnd && SendMessageW(hWnd, TCM_SETCURSEL, 0, 0) != -1)
    {
        if (m_currentPage)
            ShowWindow(m_currentPage, SW_HIDE);
        m_currentPage = static_cast<TabControl*>(this)->getPage(0);
        if (m_currentPage)
            ShowWindow(m_currentPage, SW_SHOW);
    }
}

LRESULT NpcSkillTab::clearAll()
{
    LRESULT i = 0;
    LRESULT result = SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0);
    if (result > 0)
    {
        do
        {
            pageAt(i)->clearRows();
            ++i;
            result = SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0);
        } while (i < result);
    }
    return result;
}
