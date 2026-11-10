#include <windows.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <cstdio>
#include <cstring>
#include <new>
#include "ida_compat.h"
#include "MonsterView.h"
#include "Monsters.h"
#include "Monster.h"
#include "Serializable.h"
#include "String.hpp"
#include "Bytes.h"
#include "RecordSet.h"
#include "FileDialogs.h"
#include "StudioFrame.h"
#include "_imports_C3.h"

class FileTree;

#define MOB_ID(m)    FIELD(int, (m), 0x14)
#define MOB_NAME(m)  FIELD(String, (m), 0x18)

#define FILE_PATH(f)  FIELD(String, (f), 0x10)
#define FILE_NAME(f)  FIELD(String, (f), 0x30)
#define FILE_STATE(f) FIELD(int, (f), 0xFC)

static int rsInt(RecordSet& rs, const wchar_t* column)
{
    return rs.m_rs ? rs.m_rs->getIterator()->getInt(column) : 0;
}
static int rsInt(RecordSet& rs, int column)
{
    return rs.m_rs ? rs.m_rs->getIterator()->getInt(column) : 0;
}
static const wchar_t* rsString(RecordSet& rs, const wchar_t* column)
{
    return rs.m_rs ? rs.m_rs->getIterator()->getString(column) : 0;
}

static bool readMonsterRecord(Monster* monster, RecordSet& rs)
{
    try
    {
        FIELD(short, monster, 0x3C) = (short)rsInt(rs, L"Level");
        FIELD(short, monster, 0x3E) = (short)rsInt(rs, L"Exp");
        FIELD(unsigned char, monster, 0x40) = (unsigned char)rsInt(rs, L"AI");
        FIELD(short, monster, 0x42) = (short)rsInt(rs, L"Money1");
        FIELD(short, monster, 0x44) = (short)rsInt(rs, L"Money2");
        FIELD(short, monster, 0x7C) = (short)rsInt(rs, L"QuestItemID");
        FIELD(int, monster, 0x80) = (int)rsInt(rs, L"HP");
        FIELD(short, monster, 0x84) = (short)rsInt(rs, L"SP");
        FIELD(short, monster, 0x86) = (short)rsInt(rs, L"MP");
        FIELD(short, monster, 0x88) = (short)rsInt(rs, L"Dex");
        FIELD(short, monster, 0x8A) = (short)rsInt(rs, L"Wis");
        FIELD(short, monster, 0x8C) = (short)rsInt(rs, L"Luc");
        FIELD(unsigned char, monster, 0x8E) = (unsigned char)rsInt(rs, L"Day");
        FIELD(unsigned char, monster, 0x8F) = (unsigned char)rsInt(rs, L"Size");
        FIELD(unsigned char, monster, 0x90) = (unsigned char)rsInt(rs, L"Attrib");
        FIELD(short, monster, 0x92) = (short)rsInt(rs, L"Defense");
        FIELD(short, monster, 0x94) = (short)rsInt(rs, L"Magic");
        FIELD(int, monster, 0xAC) = (int)rsInt(rs, L"NormalTime");
        FIELD(unsigned char, monster, 0xB0) = (unsigned char)rsInt(rs, L"NormalStep");
        FIELD(int, monster, 0xB4) = (int)rsInt(rs, L"ChaseTime");
        FIELD(unsigned char, monster, 0xB8) = (unsigned char)rsInt(rs, L"ChaseStep");
        FIELD(unsigned char, monster, 0xB9) = (unsigned char)rsInt(rs, L"ChaseRange");
        String column(L"ResistState");
        for (int i = 0; i < 15; ++i)
            FIELD(unsigned char, monster, 0x96 + i) = (unsigned char)rsInt(rs, sub_41E180(column, i + 1).m_buffer);
        column = L"ResistSkill";
        for (int i = 0; i < 6; ++i)
            FIELD(unsigned char, monster, 0xA5 + i) = (unsigned char)rsInt(rs, sub_41E180(column, i + 1).m_buffer);
        FIELD(short, monster, 0xBE) = (short)rsInt(rs, L"AttackType1");
        FIELD(int, monster, 0xC0) = (int)rsInt(rs, L"AttackTime1");
        FIELD(unsigned char, monster, 0xC4) = (unsigned char)rsInt(rs, L"AttackRange1");
        FIELD(short, monster, 0xC6) = (short)rsInt(rs, L"Attack1");
        FIELD(short, monster, 0xC8) = (short)rsInt(rs, L"AttackPlus1");
        FIELD(unsigned char, monster, 0xCA) = (unsigned char)rsInt(rs, L"AttackAttrib1");
        FIELD(unsigned char, monster, 0xCB) = (unsigned char)rsInt(rs, L"AttackSpecial1");
        FIELD(unsigned char, monster, 0xCC) = (unsigned char)rsInt(rs, L"AttackOK1");
        FIELD(short, monster, 0xD2) = (short)rsInt(rs, L"AttackType2");
        FIELD(int, monster, 0xD4) = (int)rsInt(rs, L"AttackTime2");
        FIELD(unsigned char, monster, 0xD8) = (unsigned char)rsInt(rs, L"AttackRange2");
        FIELD(short, monster, 0xDA) = (short)rsInt(rs, L"Attack2");
        FIELD(short, monster, 0xDC) = (short)rsInt(rs, L"AttackPlus2");
        FIELD(unsigned char, monster, 0xDE) = (unsigned char)rsInt(rs, L"AttackAttrib2");
        FIELD(unsigned char, monster, 0xDF) = (unsigned char)rsInt(rs, L"AttackSpecial2");
        FIELD(unsigned char, monster, 0xE0) = (unsigned char)rsInt(rs, L"AttackOK2");
        FIELD(short, monster, 0xE6) = (short)rsInt(rs, L"AttackType3");
        FIELD(int, monster, 0xE8) = (int)rsInt(rs, L"AttackTime3");
        FIELD(unsigned char, monster, 0xEC) = (unsigned char)rsInt(rs, L"AttackRange3");
        FIELD(short, monster, 0xEE) = (short)rsInt(rs, L"Attack3");
        FIELD(short, monster, 0xF0) = (short)rsInt(rs, L"AttackPlus3");
        FIELD(unsigned char, monster, 0xF2) = (unsigned char)rsInt(rs, L"AttackAttrib3");
        FIELD(unsigned char, monster, 0xF3) = (unsigned char)rsInt(rs, L"AttackSpecial3");
        FIELD(unsigned char, monster, 0xF4) = (unsigned char)rsInt(rs, L"AttackOK3");
    }
    catch (...)
    {
        return false;
    }
    return true;
}

MonsterView::MonsterView(void* file, Serializable* data)
    : SDataView(file, data), m_list(this)
{
    if (data)
    {
        m_monsters = dynamic_cast<Monsters*>(data);
    }
    else
    {
        m_monsters = new Monsters;
        Serializable* serializable = m_monsters;
        m_content = serializable;
        FIELD(Serializable*, this, 0x50) = serializable;
    }
    m_list.m_monsters = m_monsters;
}

MonsterView::~MonsterView()
{
}

LPCWSTR MonsterView::getCommandHelp(UINT id)
{
    switch (id)
    {
    case 58400:
        return L"Export captions (generate DBMonsterText_XXX.SData)";
    case 58417:
        return L"Import Server definitions (create monsterEx.SData)";
    case 58416:
        return L"Import text fields from a language file";
    }
    if (id != 58421)
        return 0;
    return L"Change codepage...";
}

void MonsterView::fillScriptMenu(HMENU menu)
{
    appendMenuItem(menu, 58400);
    appendMenuItem(menu, 58421);
    appendMenuItem(menu, 58416);
    appendMenuItem(menu, 58417);
}

void MonsterView::updateMenu(HMENU menu)
{
    SDataView::updateMenu(menu);
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, sizeof(mii) - sizeof(mii.cbSize));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    mii.fState = MFS_ENABLED;
    SetMenuItemInfoW(menu, 0xE12A, FALSE, &mii);
    memset(&mii.fMask, 0, sizeof(mii) - sizeof(mii.cbSize));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    mii.fState = MFS_ENABLED;
    SetMenuItemInfoW(menu, 0xE402, FALSE, &mii);
    memset(&mii.fMask, 0, sizeof(mii) - sizeof(mii.cbSize));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    mii.fState = MFS_ENABLED;
    SetMenuItemInfoW(menu, 0xE404, FALSE, &mii);
}

int MonsterView::getViewMode()
{
    return m_monsters->m_format;
}

void MonsterView::setViewMode(int mode)
{
    if (mode)
        m_monsters->setCodePage(mode);
}

const wchar_t* MonsterView::getSQLTableName()
{
    return L"PS_GameDefs.dbo.Mobs";
}

HRESULT MonsterView::load()
{
    if (!m_monsters || !g_studioFrame)
        return E_POINTER;
    FileTree* file = m_data;
    if (!FILE_NAME(file).equalsNoCase(L"monster.sdata"))
        return SDataView::load();
    bool loaded = false;
    {
        String path;
        void* exFile = 0;
        if (m_ownedData)
        {
            path = FILE_PATH(file);
            path.removeExtension();
            path << L"ex.sdata";
            if (!PathFileExistsW(path.m_buffer))
                path.clear();
        }
        else
        {
            exFile = g_studioFrame->findFile(L"monster\\MonsterEx.sdata");
            if (exFile)
                path = FILE_PATH(exFile);
        }
        if (path.endsWithNoCase(L"ex.sdata"))
        {
            Bytes bytes;
            if (!g_studioFrame->loadFromFile(&bytes, &path, false))
            {
                Serializable* data = m_content;
                try
                {
                    m_monsters->m_extended = true;
                    data->read(bytes, 'file');
                    FILE_STATE(m_data) = 1;
                    if (exFile)
                        FILE_STATE(exFile) = 1;
                    loaded = true;
                }
                catch (...)
                {
                    m_monsters->m_extended = false;
                    data->clear();
                }
            }
        }
    }
    if (!loaded)
        return SDataView::load();
    return 0;
}

HRESULT MonsterView::store()
{
    bool readOnly = g_contentProvider && C3_VCALL((void*)g_contentProvider, 0x04, bool)((void*)g_contentProvider);
    if (!m_monsters->m_extended || readOnly)
        return SDataView::store();
    try
    {
        Bytes bytes;
        try
        {
            m_content->write(bytes, 'file');
        }
        catch (...)
        {
            return E_FAIL;
        }
        String path(FILE_PATH(m_data));
        path.removeExtension();
        path << L"Ex.sdata";
        bytes.aStream::save(path.m_buffer);
        bytes.wipe();
        m_monsters->setExtended(false);
        try
        {
            m_content->write(bytes, 'file');
        }
        catch (...)
        {
            return E_FAIL;
        }
        g_studioFrame->saveToFile(&bytes, &FILE_PATH(m_data), true);
        m_monsters->setExtended(true);
        path.clear();
        if (getScriptPath(&path, L"monster.sql"))
        {
            String sql;
            m_monsters->toText(sql, 'mbdp');
            FILE* stream = 0;
            _wfopen_s(&stream, path.m_buffer, L"wb");
            if (stream)
            {
                if (sql.m_length)
                {
                    int bom = 0xFEFF;
                    fwrite(&bom, 2, 1, stream);
                    fwrite(sql.m_buffer, 2 * sql.m_length, 1, stream);
                }
                fflush(stream);
                fclose(stream);
            }
            setStatusText(L"File monsterEx.sdata saved, script monster.sql created", 0);
        }
        else
        {
            setStatusText(L"File monsterEx.sdata saved", 0);
        }
    }
    catch (...)
    {
        return E_FAIL;
    }
    return 0;
}

bool MonsterView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) != 1 || id != 58400)
        return SDataView::onCommand(code, ctrl, id);
    Bytes bytes;
    oStream& out = bytes;
    out << (unsigned long)0x1000000 << (unsigned long)0 << (unsigned long)0x3000000 << (unsigned long)0xF000000
        << (unsigned long)0x84734CC << (unsigned long)0x8CF8DD02 << (unsigned long)0x780BCC00 << (unsigned long)0
        << (unsigned long)0x6A67B600 << (unsigned long)0x493D200 << (unsigned long)0xD000000 << (unsigned long)0xD84834CC
        << (unsigned long)0x5D02 << (unsigned long)0xC785F02 << (unsigned long)0xD467B600 << (unsigned long)0xD4F7DD02
        << (unsigned long)0 << (unsigned long)0x7000000 << (unsigned long)0x9600 << (unsigned long)0xE0286102
        << (unsigned long)0x5FDEC800 << (unsigned long)0x4494D200 << (unsigned long)0 << (unsigned long)0x32000000
        << (unsigned long)0x37000000 << (unsigned long)0x49544100 << (unsigned long)0xD200 << (unsigned long)0xE860B400
        << (unsigned long)0x4494D200 << (unsigned long)0x3000000 << (unsigned long)0 << (unsigned long)0x7000000
        << (unsigned long)2
        << (char)2 << (short)'i' << (short)'d'
        << (char)4 << (short)'n' << (short)'a' << (short)'m' << (short)'e';
    int first = 0;
    int count = m_monsters->size();
    unsigned long written;
    if (MOB_ID(m_monsters->get(0)) >= 1)
    {
        written = count;
    }
    else
    {
        first = 1;
        written = count - 1;
    }
    out << written;
    for (int i = first; i < count; ++i)
    {
        Monster* monster = m_monsters->get(i);
        oStream& o = out << (unsigned long)MOB_ID(monster) << (unsigned long)0;
        MOB_NAME(monster).write(o, 4, o.m_codePage);
    }
    String fileName(L"DBMonsterText_XXX.SData");
    DWORD typeIndex = 1;
    if (!FileDialogs_save(m_hWnd, &fileName, L".sdata", (const COMDLG_FILTERSPEC*)off_59A770, 1, &typeIndex,
                          L"Save language definition as"))
        bytes.aStream::save(fileName.m_buffer);
    return true;
}

void MonsterView::importSQLRow(RecordSet* rs, bool captions)
{
    try
    {
        int id = rsInt(*rs, L"MobID");
        if (id <= 0)
            return;
        Monsters* monsters = m_monsters;
        Monster* monster = 0;
        int i = 0;
        for (; i < monsters->m_count; ++i)
            if (MOB_ID(monsters->m_items[i]) == id)
                break;
        if (i < monsters->m_count)
            monster = monsters->m_items[i];
        if (!monster)
        {
            monster = new Monster(id, m_monsters->m_format);
            m_monsters->add(monster);
        }
        if (captions || !MOB_NAME(monster).m_length)
            MOB_NAME(monster) = rsString(*rs, L"MobName");
        readMonsterRecord(monster, *rs);
    }
    catch (...)
    {
    }
}

void MonsterView::onDataUpdated()
{
    void* db = g_studioFrame->getSqlConnection();
    if (db && FIELD(bool, db, 0x18))
    {
        String query(L"SELECT Grade,DropRate FROM PS_GameDefs.dbo.MobItems WHERE MobID=");
        int count = m_monsters->size();
        setStatusText(L"Updating drops definition...", 0);
        setProgress(0, count);
        for (int i = 0; i < count; ++i)
        {
            setProgress(i, count);
            Monster* monster = m_monsters->m_items[i];
            String sql(query);
            sql << (long)MOB_ID(monster) << L" ORDER BY ItemOrder";
            alignas(RecordSet) unsigned char rsStorage[sizeof(RecordSet)];
            RecordSet& rs = *C3_VCALL(db, 0x24, RecordSet*, RecordSet*, const wchar_t*)(db, (RecordSet*)rsStorage,
                                                                                        sql.m_buffer);
            if (rs.m_rs && rs.m_rs->getIterator()->next())
            {
                short* grade = &FIELD(short, monster, 0x46);
                int* rate = &FIELD(int, monster, 0x58);
                for (int row = 0;;)
                {
                    *grade = (short)rsInt(rs, 0);
                    *rate = rsInt(rs, 1);
                    if (!rs.m_rs || !rs.m_rs->getIterator()->next())
                        break;
                    ++row;
                    ++grade;
                    ++rate;
                    if (row >= 9)
                        break;
                }
            }
            rs.~RecordSet();
        }
        setStatusText(0, 0);
    }
    String& path = FILE_PATH(m_data);
    if (path.m_buffer && path.m_length > 1 && path.m_buffer[1] == L':')
        m_monsters->setExtended(true);
    setStatusText(0, 0);
    setModified(true);
}

bool MonsterView::exportData(String* out, DWORD format)
{
    if (m_monsters->m_extended)
    {
        m_monsters->toText(*out, format);
        return true;
    }
    setStatusText(L"Cannot export definitions without extended definition.", 0);
    return false;
}

bool MonsterView::onCreate()
{
    bool created = SDataView::onCreate();
    if (!created)
        return created;
    HWND parent = m_hWnd;
    RECT rect;
    RECT* r = getClientRect(&rect);
    return m_list.create(parent, (HMENU)0x65, r->left, r->top, r->right - r->left, r->bottom - r->top,
                             0x50000000, 0, r->bottom - r->top) != 0;
}

void MonsterView::onActivate(bool active)
{
    m_list.onActivate(active);
}
