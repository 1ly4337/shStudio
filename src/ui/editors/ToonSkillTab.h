#pragma once
#include <windows.h>
#include "TabCtrl.h"
#include "Exception.h"
#include "ToonSkillList.h"

class ToonSkillView;
class ToonSkills;

class ToonSkillTab : public TabCtrl<ToonSkillList*>
{
public:
    explicit ToonSkillTab(ToonSkillView* view) : m_view(view), m_skills(0) {}

    DWORD getTabStyle() override;
    bool createPages(RECT* rc) override;

    void refresh();
    LRESULT clearAll();

    ToonSkillList* pageValueAt(int index)
    {
        if (index < 0 || index >= m_pages.m_capacity)
            throw ArrayIndexOutOfBoundsException((long)index, (long)m_pages.m_capacity);
        return m_pages.m_table[index].value;
    }

public:
    ToonSkillView* m_view;
    ToonSkills*    m_skills;
};
static_assert(sizeof(ToonSkillTab) == 0x40, "ToonSkillTab size");
