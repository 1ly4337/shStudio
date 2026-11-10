#include "ToonSkillTab.h"
#include "PersoSkillList.h"
#include "NoosSkillList.h"
#include "RawSkillList.h"
#include "ToonSkills.h"
#include "_imports_p09.h"
#include "_imports_g1b.h"

DWORD ToonSkillTab::getTabStyle()
{
    return 1024;
}

bool ToonSkillTab::createPages(RECT* rc)
{
    if (!m_skills)
        return false;
    bool result = addPage(new PersoSkillList(m_view, m_skills), 'pers', L"Toon Skills", -1);
    if (result)
    {
        result = addPage(new NoosSkillList(m_view, m_skills), 'noos', L"Noos Skills", -1);
        if (result)
        {
            result = addPage(new RawSkillList(m_view, m_skills), 'raws', L"Raw Skills", -1);
            if (result)
            {
                ToonSkillList* raw = pageValueAt(2);
                pageValueAt(0)->m_rawList = raw;
                pageValueAt(1)->m_rawList = raw;
            }
        }
    }
    return result;
}

void ToonSkillTab::refresh()
{
    if (!m_skills)
        return;
    int modes[3] = { 1, 2, 0 };
    for (LRESULT i = 0; i < SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0); ++i)
    {
        ToonSkillList* list = pageValueAt((int)i);
        if (list)
            list->setItemCount(list->m_skills.fill(m_skills, modes[i], list->m_classFilter));
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

LRESULT ToonSkillTab::clearAll()
{
    LRESULT i = 0;
    LRESULT result = SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0);
    if (result > 0)
    {
        do
        {
            pageAt((int)i)->clearRows();
            ++i;
            result = SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0);
        } while (i < result);
    }
    return result;
}
