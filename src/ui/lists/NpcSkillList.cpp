#include "NpcSkillList.h"
#include "NpcSkillView.h"
#include "MobsSkills.h"
#include "MobsSkill.h"
#include "MobSkillEditor.h"
#include "AlertDial.h"
#include "StudioFrame.h"
#include "SqlServer.h"
#include "String.hpp"
#include "_imports_G.h"
#include "Collection.hpp"

NpcSkillList::~NpcSkillList()
{
}

DWORD NpcSkillList::getStyle()
{
    return 5;
}

void NpcSkillList::onChanged()
{
    m_owner->setModified(true);
}

int NpcSkillList::insertItem(Node* node, int index)
{
    return (m_owner->addSkill(dynamic_cast<MobsSkill*>(node)) != 0) - 1;
}

Node* NpcSkillList::getNode(int index)
{
    return m_set.get(index);
}

int NpcSkillList::listV69()
{
    return 'name';
}

int NpcSkillList::getHeaderMenu()
{
    return 190;
}

bool NpcSkillList::sort()
{
    int count = m_set.m_count;
    if (count < 2)
        return false;
    m_set.m_changes = 0;
    quickSort(m_set, 0, count - 1, m_sortColumn, m_prevSortColumn, m_sortAscending);
    return m_set.m_changes > 0;
}

bool NpcSkillList::onActivateItem(int index, int keys)
{
    MobsSkill* skill = m_set.get(index);
    if (!skill)
        return false;
    MobSkillEditor editor(skill);
    if (editor.createEditor(m_hWnd) && editor.doModal() == IDOK)
    {
        onChanged();
        SqlServer* sql = (SqlServer*)g_studioFrame->getSqlConnection();
        if (sql && sql->m_connected)
        {
            String script;
            script << L"DELETE FROM [PS_GameDefs].[dbo].[Skills] WHERE SkillID=" << (long)skill->m_skill.skillId
                   << L" AND SkillLevel=100" << L"\r\n";
            skill->toText(script, 'sql ');
            if (!sql->execute(script.m_buffer))
            {
                script.clear();
                script << L"The skill " << (long)skill->m_skill.skillId
                       << L" of the table PS_GameDefs.dbo.Skills is updated.";
                m_owner->setStatusText(script.m_buffer, 0);
            }
        }
    }
    return true;
}

bool NpcSkillList::removeItem(int index)
{
    if (AlertDial::alert(m_hWnd,
                         L"Are you sure you want to delete this skill?\n"
                         L"The indexes of all subsequent skills will be changed.",
                         0, MB_ICONQUESTION | MB_YESNO) != IDYES)
        return false;
    return m_owner->removeSkill(m_set.get(index));
}

Node* NpcSkillList::createNode()
{
    return new MobsSkill(m_skills->m_format, (short)(m_skills->size() + 1), (int)(INT_PTR)m_skills);
}

bool NpcSkillList::clearItem(int index)
{
    MobsSkill* skill = m_set.get(index);
    if (!skill)
        return false;
    skill->reset();
    return true;
}

int NpcSkillList::indexOfNode(Node* node)
{
    return m_set.indexOf(dynamic_cast<MobsSkill*>(node));
}

Node* NpcSkillList::findNode(Node* like)
{
    return 0;
}

UINT NpcSkillList::getClipboardFormat()
{
    return 523;
}

LRESULT NpcSkillList::clearRows()
{
    for (int i = 0; i < m_set.m_count; ++i)
        m_set.m_items[i] = 0;
    m_set.m_count = 0;
    return setItemCount(0);
}
