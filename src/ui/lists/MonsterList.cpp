#include <windows.h>
#include <cwchar>
#include "ida_compat.h"
#include "MonsterList.h"
#include "MonsterView.h"
#include "Monsters.h"
#include "Monster.h"
#include "MonsterEditor.h"
#include "AlertDial.h"
#include "Preferences.h"
#include "String.hpp"
#include "StudioFrame.h"
#include "_imports_C3.h"
#include "Collection.hpp"

#define MOB_ID(m)    FIELD(int, (m), 0x14)
#define MOB_NAME(m)  FIELD(String, (m), 0x18)
#define MOB_LEVEL(m) FIELD(short, (m), 0x3C)

static void setMenuCheck(HMENU menu, UINT id, bool checked)
{
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, sizeof(mii) - sizeof(mii.cbSize));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    GetMenuItemInfoW(menu, id, FALSE, &mii);
    if ((mii.fState & (MFS_GRAYED | MFS_DISABLED)) == 0)
    {
        mii.fState = checked ? MFS_CHECKED : 0;
        SetMenuItemInfoW(menu, id, FALSE, &mii);
    }
}

MonsterList::~MonsterList()
{
}

int MonsterList::getHeaderMenu()
{
    return 194;
}

int MonsterList::indexOfNode(Node* node)
{
    return m_items.indexOf(dynamic_cast<Monster*>(node));
}

Node* MonsterList::findNode(Node* like)
{
    Monster& monster = dynamic_cast<Monster&>(*like);
    for (int i = 0; i < m_items.m_count; ++i)
    {
        Monster* item = m_items.m_items[i];
        if (item && MOB_ID(item) == MOB_ID(&monster))
            return m_items.m_items[i];
    }
    return 0;
}

void MonsterList::clearRows()
{
    setItemCount(0);
    for (int i = 0; i < m_items.m_count; ++i)
        m_items.m_items[i] = 0;
    m_items.m_count = 0;
}

bool MonsterList::onCreate()
{
    bool created = ListCtrlOD::onCreate();
    if (!created)
        return created;
    addColumn(L"MobID", 60, 16, 'mbid');
    addColumn(L"Name", 200, 4096, 'name');
    addColumn(L"Level", 45, 18, 'lvl ');
    addColumn(L"Model", 45, 18, 'modl');
    addColumn(L"Exp", 45, 18, 'exp_');
    addColumn(L"AI", 45, 18, 'ai  ');
    m_columnGroups = 0;
    setupColumns();
    refresh();
    return true;
}

void MonsterList::onContextMenu(HMENU menu)
{
    setMenuCheck(menu, 1100, m_showAll == false);
    setMenuCheck(menu, 1101, m_showAll != false);
    setMenuCheck(menu, 1050, m_allColumns != false);
    setMenuCheck(menu, 1060, (m_columnGroups & 1) != 0);
    setMenuCheck(menu, 1061, (m_columnGroups & 2) != 0);
    setMenuCheck(menu, 1062, (m_columnGroups & 4) != 0);
}

void MonsterList::setupColumns()
{
    while (m_columnCount > 6)
    {
        if (!deleteColumn(m_columnCount - 1))
            break;
    }
    if (m_columnGroups & 1)
        addStatColumns();
    if (m_columnGroups & 2)
        addResistColumns();
    if (m_columnGroups & 4)
        addAttackColumns();
    if (!m_columnGroups)
    {
        addColumn(L"Height", 45, 18, 'day ');
        addColumn(L"Elem", 45, 2, 'attr');
        addColumn(L"Qst Itm", 50, 18, 'qust');
        addColumn(L"Atk Mode 1", 50, 18, '1mod');
        addColumn(L"Atk Mode 2", 50, 18, '2mod');
        addColumn(L"Atk Mode 3", 50, 18, '3mod');
    }
}

void MonsterList::addStatColumns()
{
    m_columnGroups |= 1;
    addColumn(L"Money1", 50, 18, 'gld1');
    addColumn(L"Money2", 50, 18, 'gld2');
    addColumn(L"Qst Itm", 50, 18, 'qust');
    addColumn(L"HP", 45, 18, 'hp  ');
    addColumn(L"SP", 45, 18, 'sp  ');
    addColumn(L"MP", 45, 18, 'mp  ');
    addColumn(L"Dex", 45, 18, 'dex ');
    addColumn(L"Wis", 45, 18, 'wis ');
    addColumn(L"Luc", 45, 18, 'luc ');
    addColumn(L"Height", 45, 18, 'day ');
    addColumn(L"Size", 45, 18, 'size');
    addColumn(L"Elem", 45, 2, 'attr');
    addColumn(L"Defense", 45, 18, 'defs');
    addColumn(L"Magic", 45, 18, 'magc');
    addColumn(L"Normal Time", 45, 18, 'ntme');
    addColumn(L"Normal Step", 45, 18, 'nstp');
    addColumn(L"Chase Time", 45, 18, 'ctme');
    addColumn(L"Chase Step", 45, 18, 'cstp');
    addColumn(L"Chase Range", 45, 18, 'crng');
}

void MonsterList::addResistColumns()
{
    m_columnGroups |= 2;
    addColumn(L"Res.State 1", 45, 18, 'rs01');
    addColumn(L"Res.State 2", 45, 18, 'rs02');
    addColumn(L"Res.State 3", 45, 18, 'rs03');
    addColumn(L"Res.State 4", 45, 18, 'rs04');
    addColumn(L"Res.State 5", 45, 18, 'rs05');
    addColumn(L"Res.State 6", 45, 18, 'rs06');
    addColumn(L"Res.State 7", 45, 18, 'rs07');
    addColumn(L"Res.State 8", 45, 18, 'rs08');
    addColumn(L"Res.State 9", 45, 18, 'rs09');
    addColumn(L"Res.State 10", 45, 18, 'rs10');
    addColumn(L"Res.State 11", 45, 18, 'rs11');
    addColumn(L"Res.State 12", 45, 18, 'rs12');
    addColumn(L"Res.State 13", 45, 18, 'rs13');
    addColumn(L"Res.State 14", 45, 18, 'rs14');
    addColumn(L"Res.State 15", 45, 18, 'rs15');
    addColumn(L"Res.Skill 1", 45, 18, 'rk01');
    addColumn(L"Res.Skill 2", 45, 18, 'rk02');
    addColumn(L"Res.Skill 3", 45, 18, 'rk03');
    addColumn(L"Res.Skill 4", 45, 18, 'rk04');
    addColumn(L"Res.Skill 5", 45, 18, 'rk05');
    addColumn(L"Res.Skill 6", 45, 18, 'rk06');
}

void MonsterList::addAttackColumns()
{
    m_columnGroups |= 4;
    addColumn(L"Atk 1 Type", 45, 18, '1typ');
    addColumn(L"Atk 1 Time", 45, 18, '1tme');
    addColumn(L"Atk 1 Range", 45, 18, '1rng');
    addColumn(L"Atk 1 Dmg", 45, 18, '1dmg');
    addColumn(L"Atk 1 Plus", 45, 18, '1add');
    addColumn(L"Atk 1 Elem", 45, 18, '1atr');
    addColumn(L"Atk 1 Spec", 45, 18, '1spc');
    addColumn(L"Atk 1 Mode", 45, 18, '1mod');
    addColumn(L"Atk 2 Type", 45, 18, '2typ');
    addColumn(L"Atk 2 Time", 45, 18, '2tme');
    addColumn(L"Atk 2 Range", 45, 18, '2rng');
    addColumn(L"Atk 2 Dmg", 45, 18, '2dmg');
    addColumn(L"Atk 2 Plus", 45, 18, '2add');
    addColumn(L"Atk 2 Elem", 45, 18, '2atr');
    addColumn(L"Atk 2 Spec", 45, 18, '2spc');
    addColumn(L"Atk 2 Mode", 45, 18, '2mod');
    addColumn(L"Atk 3 Type", 45, 18, '3typ');
    addColumn(L"Atk 3 Time", 45, 18, '3tme');
    addColumn(L"Atk 3 Range", 45, 18, '3rng');
    addColumn(L"Atk 3 Dmg", 45, 18, '3dmg');
    addColumn(L"Atk 3 Plus", 45, 18, '3add');
    addColumn(L"Atk 3 Elem", 45, 18, '3atr');
    addColumn(L"Atk 3 Spec", 45, 18, '3spc');
    addColumn(L"Atk 3 Mode", 45, 18, '3mod');
}

void MonsterList::onItemChanged(int index, UINT newState)
{
    int count = getCount(0);
    m_view->enableMenuItem(0xE123, count > 0);
}

bool MonsterList::onActivateItem(int index, int keys)
{
    Monster* monster = m_items.get(index);
    MonsterModels* models = g_studioFrame->m_client.m_clientData.getMonsterModels(L"monsters");
    if (monster && models)
    {
        MonsterEditor editor(monster, models);
        if (editor.MonsterEditor::open(m_hWnd))
        {
            if (g_preferences.getInt(L"3D View", L"show monster", 0) == 1)
                editor.onCommand(0, 0, 150);
            if (editor.doModal() == 1)
                m_view->setModified(true);
        }
        return true;
    }
    m_view->setStatusText(L"No models are available (no access to file \"monster.mon\"), the edition is not possible.", 0);
    return false;
}

void MonsterList::refresh()
{
    clearRows();
    if (!m_monsters)
        return;
    int count = m_monsters->size();
    if (m_showAll)
    {
        for (int i = 0; i < count; ++i)
            m_items.add(m_monsters->get(i));
    }
    else
    {
        for (int i = 0; i < count; ++i)
        {
            Monster* monster = m_monsters->get(i);
            if (monster && MOB_LEVEL(monster) > 0)
            {
                String& name = MOB_NAME(monster);
                if (name.m_length >= 2 && !wcsncmp(name.m_buffer, L"??", 2))
                    continue;
                if (!name.startsWith(L"Error"))
                    m_items.add(monster);
            }
        }
    }
    setItemCount(m_items.size());
}

Node* MonsterList::createNode()
{
    return new Monster(m_monsters->size(), m_monsters->m_format);
}

int MonsterList::insertItem(Node* node, int index)
{
    Monster* monster = dynamic_cast<Monster*>(node);
    if (m_monsters->add(monster))
        return m_items.insert(monster, index);
    return -1;
}

bool MonsterList::clearItem(int index)
{
    Monster* monster = m_items.get(index);
    if (!monster)
        return false;
    monster->reset();
    m_view->setModified(true);
    return true;
}

bool MonsterList::removeItem(int index)
{
    if (index < 0)
        return false;
    if (index >= m_items.size())
        return false;
    Monster* monster = m_items.get(index);
    if (!monster)
        return false;
    if (AlertDial::alert(m_hWnd,
                         L"Are you sure you want to delete the selected monster(s)?\n"
                         L"The indexes of all subsequent monsters will be changed.",
                         0, MB_YESNO | MB_ICONQUESTION) != IDYES)
        return false;
    Monsters* monsters = m_monsters;
    if (!monsters || !monsters->detachItem(monster))
        return false;
    monsters->destroyItem(monster);
    m_items.detach(index);
    m_view->setModified(true);
    return true;
}

bool MonsterList::onCommand(int code, LPARAM ctrl, int id)
{
    if (code || ctrl)
        return ListCtrlOD::onCommand(code, ctrl, id);
    switch (id)
    {
    case 1100:
        m_showAll = false;
        refresh();
        return true;
    case 1101:
        m_showAll = true;
        refresh();
        return true;
    case 1050:
        m_columnGroups = 7;
        setupColumns();
        refresh();
        return true;
    }
    if ((unsigned)(id - 1060) <= 2)
    {
        m_columnGroups ^= 1 << (id - 1060);
        setupColumns();
        refresh();
        return true;
    }
    return ListCtrlOD::onCommand(0, 0, id);
}

void MonsterList::onChanged()
{
    m_view->setModified(true);
}

Node* MonsterList::getNode(int index)
{
    return m_items.get(index);
}

bool MonsterList::sort()
{
    int previous = m_prevSortColumn;
    bool ascending = m_sortAscending;
    int count = m_items.m_count;
    int column = m_sortColumn;
    if (count < 2)
        return false;
    m_items.m_changes = 0;
    quickSort(m_items, 0, count - 1, column, previous, ascending);
    return m_items.m_changes > 0;
}
