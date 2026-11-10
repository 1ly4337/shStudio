#include "ToonSkillView.h"
#include "ToonSkills.h"
#include "ToonSkill.h"
#include "SkillData.h"
#include "Serializable.h"
#include "Streamable.h"
#include "Bytes.h"
#include "ImportSQLEditor.h"
#include "StudioFrame.h"
#include "SqlServer.h"
#include "RecordSet.h"
#include "AlertDial.h"
#include "FileDialogs.h"
#include "ida_compat.h"
#include <cstdio>
#include <cstring>
#include <typeinfo>
#include "_imports_p09.h"
#include "_imports_g1b.h"

ToonSkillView::ToonSkillView(void* file, Serializable* data)
    : SDataView(file, data), m_tabs(this)
{
    if (data)
    {
        m_skills = dynamic_cast<ToonSkills*>(data);
    }
    else
    {
        m_skills = new ToonSkills();
        Serializable* doc = m_skills ? static_cast<Serializable*>(m_skills) : 0;
        FIELD(Serializable*, this, 0x9C) = doc;
        FIELD(Serializable*, this, 0x50) = doc;
    }
    m_tabs.m_skills = m_skills;
}

ToonSkillView::~ToonSkillView()
{
}

bool ToonSkillView::onCreate()
{
    if (!SDataView::onCreate())
        return false;
    HWND parent = m_hWnd;
    RECT area;
    RECT* rc = getClientRect(&area);
    if (!m_tabs.create(parent, (HMENU)0x65, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
                       WS_VISIBLE, 0, rc->bottom - rc->top))
        return false;
    m_tabs.refresh();
    enableMenuItem(0xE410, true);
    return true;
}

bool ToonSkillView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id != 0xE413)
        return SDataView::onCommand(code, ctrl, id);
    dumpSkills();
    return true;
}

bool ToonSkillView::onInitMenu(HMENU menu)
{
    SDataView::onInitMenu(menu);
    m_tabs.onInitMenu((WPARAM)menu);
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, sizeof(mii) - sizeof(mii.cbSize));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    mii.fState = MFS_DISABLED;
    SetMenuItemInfoW(menu, 0xE12B, FALSE, &mii);
    return true;
}

static BOOL setMenuItem(HMENU menu, UINT id, UINT mask, UINT state, LPWSTR text, UINT cch)
{
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, sizeof(mii) - sizeof(mii.cbSize));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = mask;
    mii.fState = state;
    if (mask & MIIM_STRING)
    {
        mii.dwTypeData = text;
        mii.cch = cch;
    }
    return SetMenuItemInfoW(menu, id, FALSE, &mii);
}

void ToonSkillView::updateMenu(HMENU menu)
{
    SDataView::updateMenu(menu);
    setMenuItem(menu, 0xE12A, MIIM_STATE, 0, 0, 0);
    setMenuItem(menu, 0xE12B, MIIM_STATE, MFS_DISABLED, 0, 0);
    setMenuItem(menu, 0xE12C, MIIM_STATE, 0, 0, 0);
    setMenuItem(menu, 0xE401, MIIM_STATE | MIIM_STRING, 0,
                const_cast<LPWSTR>(L"Episo 4 format (3 levels per skill, 3 abilities per level)"), 58);
    setMenuItem(menu, 0xE402, MIIM_STATE | MIIM_STRING, 0,
                const_cast<LPWSTR>(L"Episo 5 format (9 levels per skill, 3 abilities per level)"), 58);
    setMenuItem(menu, 0xE403, MIIM_STATE | MIIM_STRING, 0,
                       const_cast<LPWSTR>(L"Episo 6 format (15 levels per skill, 10 abilities per level)"), 60);
}

bool ToonSkillView::save()
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
        FILE* stream = 0;
        _wfopen_s(&stream, path.m_buffer, L"wb");
        if (stream)
        {
            if (script.m_length)
            {
                int bom = 0xFEFF;
                fwrite(&bom, 2, 1, stream);
                fwrite(script.m_buffer, 2 * script.m_length, 1, stream);
            }
            fflush(stream);
            fclose(stream);
        }
        setStatusText(L"File saved and SQL script generated.", 0);
    }
    return true;
}

String ToonSkillView::getMergeFilter()
{
    return String(L"Skill SData File\tDBSkillText_*.SData\t");
}

void ToonSkillView::importSQLDefinition()
{
    SqlServer* sql = static_cast<SqlServer*>(g_studioFrame->getSqlConnection());
    if (!sql || !sql->m_connected)
    {
        setStatusText(L"The connection to server must be alive to import skills definition.", 0);
        MessageBeep(MB_ICONHAND);
    }

    ImportSQLEditor editor(L"PS_GameDefs.dbo.Skills");
    if (!editor.createDialog(m_hWnd, 460, 150, 4, L"Import SQL definition") || editor.doModal() != IDYES)
        return;

    bool importCaptions = editor.m_importCaptions;
    UpdateWindow(m_hWnd);
    try
    {
        RecordSet countRs = sql->query(L"SELECT Count(*),Max(SkillLevel) FROM PS_GameDefs.dbo.Skills WHERE SkillLevel < 100");
        if (!countRs.m_impl || !countRs.m_impl->getIterator()->next())
            throw 0;
        int format = 0;
        int count = 0;
        if (countRs.m_impl)
        {
            count = countRs.m_impl->getIterator()->getIntAt(0);
            if (countRs.m_impl)
            {
                switch (countRs.m_impl->getIterator()->getIntAt(1))
                {
                case 3:
                    format = 0x400;
                    break;
                case 9:
                    format = 0x500;
                    break;
                case 15:
                    format = 0x600;
                    break;
                }
            }
        }
        if (!format)
        {
            setStatusText(L"The number of levels per skill cannot be obtained from database.", 0);
            MessageBeep(MB_ICONHAND);
            return;
        }

        setProgress(0, count);
        RecordSet rs = sql->query(
            L"SELECT * FROM [PS_GameDefs].[dbo].[Skills] WHERE SkillLevel < 100 ORDER BY SkillID,SkillLevel");
        ToonSkill* skill = 0;
        int lastId = 0;
        for (int row = 0; row < count; ++row)
        {
            setProgress(row, count);
            if (!rs.m_impl || !rs.m_impl->getIterator()->next())
                throw 0;
            int skillId = rs.m_impl ? rs.m_impl->getIterator()->getInt(L"SkillID") : 0;
            if (lastId != skillId)
            {
                lastId = skillId;
                skill = 0;
            }
            int level;
            const wchar_t* name;
            if (rs.m_impl)
            {
                level = rs.m_impl->getIterator()->getInt(L"SkillLevel");
                name = rs.m_impl ? rs.m_impl->getIterator()->getString(L"SkillName") : 0;
            }
            else
            {
                level = 0;
                name = 0;
            }
            String caption(name);

            if (level == 1)
            {
                skill = m_skills->findById(skillId);
                if (!skill)
                {
                    skill = new ToonSkill(skillId - 1, format);
                    m_skills->add(skill);
                }
                ToonSkill::baseName(caption, skill->m_name);
                skill->setFormat(format);
            }
            else if ((unsigned int)(level - 1) > 14)
            {
                String message(L"The PS_GameDefs.dbo.Skills contains invalid data.");
                message << L"\nWhen performing:\n"
                        << L"SELECT * FROM [PS_GameDefs].[dbo].[Skills] WHERE SkillLevel < 100 ORDER BY SkillID,SkillLevel"
                        << L"\nThe row " << (unsigned long)(row + 1) << L" has 'SkillLevel' ";
                message << (level >= 1 ? "> 15" : "< 1");
                AlertDial::alert(m_hWnd, message.m_buffer, L"Error", MB_ICONHAND);
                MessageBeep(MB_ICONHAND);
                return;
            }
            else if (!skill)
            {
                String message(L"The PS_GameDefs.dbo.Skills contains invalid data.");
                message << L"\nWhen performing:\n"
                        << L"SELECT * FROM [PS_GameDefs].[dbo].[Skills] WHERE SkillLevel < 100 ORDER BY SkillID,SkillLevel";
                message << L"\nNo record with 'SkillLevel'=1 was found for 'SkillID'=" << (unsigned long)skillId;
                AlertDial::alert(m_hWnd, message.m_buffer, L"Error", MB_ICONHAND);
                MessageBeep(MB_ICONHAND);
                return;
            }

            SkillData* data = &skill->m_levels[level - 1];
            if (importCaptions)
            {
                String& levelName = skill->m_levelNames[level - 1];
                levelName.clear();
                levelName.append(skill->m_name.m_buffer, skill->m_name.m_length) << L" Lv" << (unsigned long)level;
            }
            data->loadFromRecord(&rs);
        }
        m_skills->m_format = format;
        onDataUpdated();
    }
    catch (...)
    {
        setStatusText(L"The SQL request failed to execute.", 0);
        MessageBeep(MB_ICONHAND);
    }
}

void ToonSkillView::onDataUpdated()
{
    setStatusText(0, 0);
    setModified(true);
    m_tabs.refresh();
}

bool ToonSkillView::exportData(String* out, DWORD format)
{
    static_cast<Streamable*>(m_skills)->toText(*out, format);
    return true;
}

const wchar_t* ToonSkillView::getSQLTableName()
{
    return L"PS_GameDefs.dbo.Skills where SkillLevel < 100";
}

void ToonSkillView::importSQLRow(RecordSet* rs, bool captions)
{
}

void ToonSkillView::setViewMode(int mode)
{
    if (mode)
    {
        m_skills->setCodePage(mode);
        setModified(true);
    }
}

int ToonSkillView::getViewMode()
{
    return m_skills->m_format & 0xFFF;
}

LRESULT ToonSkillView::clearLists()
{
    return m_tabs.clearAll();
}

bool ToonSkillView::removeSkill(ToonSkill* skill)
{
    if (!skill || !m_skills)
        return false;
    ToonSkillList* list = m_tabs.getPageByKey('pers');
    if (list && list->m_skills.ptrArray<ToonSkill*>::detachItem(skill))
        list->setItemCount(list->getCount(0) - 1);
    list = m_tabs.getPageByKey('noos');
    if (list && list->m_skills.ptrArray<ToonSkill*>::detachItem(skill))
        list->setItemCount(list->getCount(0) - 1);
    list = m_tabs.getPageByKey('raws');
    if (list && list->m_skills.ptrArray<ToonSkill*>::detachItem(skill))
        list->setItemCount(list->getCount(0) - 1);
    ToonSkills* skills = m_skills;
    if (skills->ptrArray<ToonSkill*>::detachItem(skill))
        skills->destroyItem(skill);
    setModified(true);
    return true;
}

void ToonSkillView::dumpSkills()
{
    String path;
    path.append(L"Skill.sdata");
    if (!FileDialogs_open(m_hWnd, &path, L"Skill.sdata\0*.sdata\0", L"Select the skills file to dump", false))
        return;

    Bytes data;
    if (g_studioFrame->loadFromFile(&data, &path, 1))
    {
        setStatusText(L"The selected file can not be loaded.", 0);
        MessageBeep(MB_ICONHAND);
        return;
    }

    String text;
    ToonSkills skills;
    int formats[5] = { 0x500, 0x600, 0x620, 0x400, 0 };
    for (int f = 0;; ++f)
    {
        skills.deleteAll();
        data.m_pos = 0;
        text.clear();
        int format = formats[f];
        int levels;
        switch (format)
        {
        case 0x400:
            levels = 3;
            break;
        case 0x500:
            levels = 9;
            break;
        case 0x600:
        case 0x620:
            levels = 15;
            break;
        default:
            setStatusText(L"The selected file is invalid.", 0);
            MessageBeep(MB_ICONHAND);
            return;
        }
        try
        {
            SkillData record;
            record.version = format;
            int count = (int)data.readInt();
            text << (long)count << L" skills declared" << L"\r\n";
            for (int i = 1; i <= count; ++i)
            {
                for (int j = 1; j <= levels; ++j)
                {
                    record.read(data, 'file');
                    text << (long)i << ';' << (long)j << ';';
                    record.exportText(text, '----');
                }
            }
            break;
        }
        catch (...)
        {
        }
    }

    path = L"skills.dump.txt";
    DWORD typeIndex = 1;
    if (!FileDialogs_save(m_hWnd, &path, L".txt", g_textFileFilter, 1, &typeIndex, L"Choose the name of dump file"))
        text.saveAs(path.m_buffer, 0, 1200);
}
