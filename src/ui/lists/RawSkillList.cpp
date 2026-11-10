#include "RawSkillList.h"
#include "ToonSkills.h"
#include "_imports_p09.h"
#include "_imports_g1b.h"

DWORD RawSkillList::getStyle()
{
    return 517;
}

bool RawSkillList::onCreate()
{
    bool result = ListCtrl::onCreate();
    if (!result)
        return result;
    addColumn(L"ID", 50, 16, 'skid');
    addColumn(L"Name", 120, 4096, 'name');
    addColumn(L"Type", 50, 18, 'tpdt');
    addColumn(L"Icon", 50, 18, 'icon');
    addColumn(L"Anim", 50, 18, 'anim');
    for (int i = 0; i < 60; ++i)
    {
        DWORD tag = g_skillColumnTags[8 + i];
        if (tag == 'tpdt')
            continue;
        wchar_t* name = g_skillColumnNames[8 + i];
        if (!tag || !name)
            break;
        addColumn(name, 50, 18, tag);
    }
    for (int i = 0; i < 6; ++i)
        addColumn(g_skillColumnNames[88 + i], 50, 18, g_skillColumnTags[88 + i]);
    for (int i = 0; i < 6; ++i)
        addColumn(g_skillColumnNames[68 + i], 50, 18, g_skillColumnTags[68 + i]);
    if ((m_doc->m_format & 0xFFF) >= 0x600)
    {
        for (int i = 0; i < 14; ++i)
            addColumn(g_skillColumnNames[74 + i], 50, 18, g_skillColumnTags[74 + i]);
    }
    for (int i = 0; i < 6; ++i)
        addColumn(g_skillColumnNames[94 + i], 50, 18, g_skillColumnTags[94 + i]);
    return true;
}

DWORD RawSkillList::getExStyle()
{
    return 48;
}
