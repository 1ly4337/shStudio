#include "NpcSkillView.h"
#include "MobsSkills.h"
#include "MobsSkill.h"
#include "Serializable.h"
#include "ImportSQLEditor.h"
#include "StudioFrame.h"
#include "SqlServer.h"
#include "RecordSet.h"
#include "ida_compat.h"
#include "_imports_G.h"
#include <cstdio>
#include <cstring>

NpcSkillView::NpcSkillView(void* file, Serializable* data)
    : SDataView(file, data), m_tabs(this)
{
    if (data)
    {
        m_skills = dynamic_cast<MobsSkills*>(data);
    }
    else
    {
        m_skills = new MobsSkills();
        Serializable* doc = m_skills ? static_cast<Serializable*>(m_skills) : 0;
        FIELD(Serializable*, this, 0x9C) = doc;
        FIELD(Serializable*, this, 0x50) = doc;
    }
    m_tabs.m_skills = m_skills;
}

NpcSkillView::~NpcSkillView()
{
}

bool NpcSkillView::onCreate()
{
    if (!SDataView::onCreate())
        return false;
    HWND parent = m_hWnd;
    RECT rc;
    RECT* r = getClientRect(&rc);
    if (!m_tabs.Control::create(parent, (HMENU)0x65, r->left, r->top, r->right - r->left, r->bottom - r->top,
                           WS_VISIBLE, 0, r->bottom - r->top))
        return false;
    m_tabs.refresh();
    enableMenuItem(0xE410, true);
    return true;
}

bool NpcSkillView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code || ctrl)
        return SDataView::onCommand(code, ctrl, id);
    if (id == 0xE120)
    {
        m_skills->setCodePage(0x500);
        setModified(true);
        return true;
    }
    if (id == 0xE122)
    {
        m_skills->setCodePage(0x620);
        setModified(true);
        return true;
    }
    return SDataView::onCommand(0, 0, id);
}

bool NpcSkillView::onInitMenu(HMENU menu)
{
    SDataView::onInitMenu(menu);
    m_tabs.onInitMenu((WPARAM)menu);
    return true;
}

void NpcSkillView::updateMenu(HMENU menu)
{
    SDataView::updateMenu(menu);
    MENUITEMINFOW mii;

    memset(&mii, 0, sizeof(mii));
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_STATE;
    mii.fState = MFS_DISABLED;
    SetMenuItemInfoW(menu, 0xE12B, FALSE, &mii);

    memset(&mii, 0, sizeof(mii));
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_STATE;
    mii.fState = 0;
    SetMenuItemInfoW(menu, 0xE12C, FALSE, &mii);

    memset(&mii, 0, sizeof(mii));
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_STATE | MIIM_STRING;
    mii.fState = 0;
    mii.dwTypeData = (LPWSTR)L"Episo 4 format (3 levels per skill, 3 abilities per level)";
    mii.cch = 58;
    SetMenuItemInfoW(menu, 0xE401, FALSE, &mii);

    memset(&mii, 0, sizeof(mii));
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_STATE | MIIM_STRING;
    mii.fState = 0;
    mii.dwTypeData = (LPWSTR)L"Episo 5 format (9 levels per skill, 3 abilities per level)";
    mii.cch = 58;
    SetMenuItemInfoW(menu, 0xE402, FALSE, &mii);

    memset(&mii, 0, sizeof(mii));
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_STATE | MIIM_STRING;
    mii.fState = 0;
    mii.dwTypeData = (LPWSTR)L"Episo 6 format (15 levels per skill, 10 abilities per level)";
    mii.cch = 60;
    SetMenuItemInfoW(menu, 0xE403, FALSE, &mii);
}

bool NpcSkillView::save()
{
    bool result = SDataView::save();
    if (!result)
        return result;
    String path;
    void* file = FIELD(void*, this, 0x4C);
    if (getScriptPath(&path, FIELD(const wchar_t*, file, 0x34)))
    {
        String script;
        static_cast<Streamable*>(m_skills)->toText(script, 'sql ');
        UINT codePage = FIELD(UINT, file, 0x140);
        FILE* stream = 0;
        _wfopen_s(&stream, path.m_buffer, L"wb");
        script.writeTo(stream, codePage);
        setStatusText(L"File saved and SQL script generated.", 0);
    }
    return true;
}

HRESULT NpcSkillView::load()
{
    clearLists();
    HRESULT result = SDataView::load();
    m_tabs.refresh();
    return result;
}

String NpcSkillView::getMergeFilter()
{
    return String(L"Skill SData File\tDBNpcSkillText_*.SData\t");
}

void NpcSkillView::importSQLDefinition()
{
    SqlServer* sql = (SqlServer*)g_studioFrame->getSqlConnection();
    if (!sql || !sql->m_connected)
    {
        setStatusText(L"The connection to server must be alive to import skills definition.", 0);
        MessageBeep(MB_ICONHAND);
    }

    ImportSQLEditor editor(L"Skills");
    if (!editor.createDialog(m_hWnd, 460, 150, 4, L"Import SQL definition") || editor.doModal() != IDOK)
        return;

    bool importCaptions = editor.m_importCaptions;
    RecordSet countRs = sql->query(L"SELECT Count(*) FROM PS_GameDefs.dbo.Skills WHERE SkillLevel = 100");
    if (!countRs.m_impl || !countRs.m_impl->getIterator()->next())
        throw 0;
    int count = countRs.m_impl ? countRs.m_impl->getIterator()->getIntAt(0) : 0;
    setProgress(0, count);

    RecordSet rs = sql->query(
        L"SELECT * FROM [PS_GameDefs].[dbo].[Skills] WHERE SkillLevel = 100 ORDER BY SkillID,SkillLevel");
    for (int i = 0; i < count; ++i)
    {
        setProgress(i, count);
        if (!rs.m_impl || !rs.m_impl->getIterator()->next())
            throw 0;
        int skillId;
        const wchar_t* name;
        if (rs.m_impl)
        {
            skillId = rs.m_impl->getIterator()->getInt(L"SkillID");
            name = rs.m_impl ? rs.m_impl->getIterator()->getString(L"SkillName") : 0;
        }
        else
        {
            skillId = 0;
            name = 0;
        }
        String caption(name);

        MobsSkill* skill = 0;
        MobsSkills* skills = m_skills;
        int n = skills->m_count;
        for (int j = 0; j < n; ++j)
        {
            if (skills->m_items[j]->m_skill.skillId == skillId)
            {
                skill = skills->m_items[j];
                break;
            }
        }
        if (!skill)
        {
            skill = new MobsSkill(skillId - 1, (short)m_skills->m_format, 0);
            m_skills->add(skill);
        }
        if (importCaptions)
            skill->m_name = caption;
        skill->m_skill.loadFromRecord(&rs);
        skill->m_skill.level = 1;
    }
    setProgress(0, 0);
    m_tabs.refresh();
    setModified(true);
}

void NpcSkillView::onDataUpdated()
{
}

bool NpcSkillView::exportData(String* out, DWORD format)
{
    static_cast<Streamable*>(m_skills)->toText(*out, format);
    return true;
}

const wchar_t* NpcSkillView::getSQLTableName()
{
    return L"PS_GameDefs.dbo.Skills where SkillLevel = 100";
}

void NpcSkillView::importSQLRow(RecordSet* rs, bool captions)
{
}

void NpcSkillView::setViewMode(int mode)
{
    if (mode)
        m_skills->setCodePage(mode);
}

int NpcSkillView::getViewMode()
{
    return m_skills->m_format & 0xFFF;
}

LRESULT NpcSkillView::clearLists()
{
    return m_tabs.clearAll();
}

bool NpcSkillView::addSkill(MobsSkill* skill)
{
    if (!skill || !m_skills)
        return false;
    NpcSkillList* list = m_tabs.getPageByKey('mobs');
    if (list && list->m_set.add(skill))
        list->setItemCount(list->getCount(0) + 1);
    list = m_tabs.getPageByKey('raw ');
    if (list && list->m_set.add(skill))
        list->setItemCount(list->getCount(0) + 1);
    m_skills->add(skill);
    setModified(true);
    return true;
}

bool NpcSkillView::removeSkill(MobsSkill* skill)
{
    if (!skill || !m_skills)
        return false;
    NpcSkillList* list = m_tabs.getPageByKey('mobs');
    if (list && list->m_set.detachItem(skill))
        list->setItemCount(list->getCount(0) - 1);
    list = m_tabs.getPageByKey('raw ');
    if (list && list->m_set.detachItem(skill))
        list->setItemCount(list->getCount(0) - 1);
    MobsSkills* skills = m_skills;
    if (skills->detachItem(skill))
        skills->destroyItem(skill);
    setModified(true);
    return true;
}
