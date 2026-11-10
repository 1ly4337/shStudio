#include "ToonSkillList.h"
#include "ToonSkillView.h"
#include "Collection.hpp"
#include "ToonSkills.h"
#include "ToonSkill.h"
#include "ToonSkillEditor.h"
#include "RawSkillEditor.h"
#include "AlertDial.h"
#include "ClientData.h"
#include <typeinfo>
#include "_imports_p09.h"
#include "_imports_g1b.h"

ToonSkillList::ToonSkillList(ToonSkillView* owner, ToonSkills* skills)
    : m_owner(owner), m_doc(skills), m_rawList(0), m_classFilter(0), m_listKind(0)
{
    m_sortColumn = 'skid';
    m_prevSortColumn = 'name';
    m_useRawEditor = true;
}

ToonSkillList::~ToonSkillList()
{
    m_skills.forgetAll();
}

DWORD ToonSkillList::getStyle()
{
    return 5;
}

bool ToonSkillList::onCreate()
{
    bool result = ListCtrl::onCreate();
    if (!result)
        return result;
    ImageStrip* icons = reinterpret_cast<ClientData*>(reinterpret_cast<BYTE*>(((void*)g_studioFrame)) + 0x230)->getIcons(L"icon_skills");
    if (icons)
        setImages(icons);
    addColumn(L"ID", 50, 16, 'skid');
    addColumn(L"Name", 120, 4096, 'name');
    addColumn(L"State", 50, 18, 'sttp');
    addColumn(L"Type", 130, 0, 'tpdt');
    addColumn(L"Dmg Type", 50, 18, 'dmtp');
    addColumn(L"Desc", 250, 4096, 'desc');
    return true;
}

void ToonSkillList::onChanged()
{
    m_owner->setModified(true);
}

int ToonSkillList::insertItem(Node* node, int index)
{
    ToonSkill* skill = dynamic_cast<ToonSkill*>(node);
    if (!m_doc->add(skill))
        return -1;
    initNewSkill(skill);
    int position = m_skills.insert(skill, -1);
    if (m_rawList)
    {
        m_rawList->m_skills.insert(skill, -1);
        m_rawList->sort();
        m_rawList->setItemCount(m_rawList->getCount(0) + 1);
    }
    return position;
}

Node* ToonSkillList::getNode(int index)
{
    return m_skills.get(index);
}

int ToonSkillList::listV69()
{
    return 'name';
}

bool ToonSkillList::sort()
{
    int count = m_skills.m_count;
    if (count < 2)
        return false;
    m_skills.m_changes = 0;
    quickSort(m_skills, 0, count - 1, m_sortColumn, m_prevSortColumn, m_sortAscending);
    return m_skills.m_changes > 0;
}

bool ToonSkillList::onActivateItem(int index, int keys)
{
    ToonSkill* skill = m_skills.get(index);
    if (!skill)
        return false;
    bool raw = m_useRawEditor;
    ToonSkillEditor* editor;
    if ((GetKeyState(VK_LCONTROL) & 0x8080) != 0 || raw)
        editor = new RawSkillEditor(skill);
    else
        editor = new ToonSkillEditor(skill);
    if (!editor->createEditor(m_owner->m_hWnd))
        delete editor;
    else
        editor->showModeless(m_owner);
    return true;
}

bool ToonSkillList::removeItem(int index)
{
    if (index + 1 != m_skills.size()
        || AlertDial::alert(m_hWnd, L"Are you sure you want to delete this skill?", 0, MB_ICONQUESTION | MB_YESNO) != IDYES)
    {
        return false;
    }
    return m_owner->removeSkill(m_skills.get(index));
}

Node* ToonSkillList::createNode()
{
    ToonSkills* doc = m_doc;
    return new ToonSkill(doc->size(), doc->m_format);
}

bool ToonSkillList::clearItem(int index)
{
    if (AlertDial::alert(m_hWnd, L"Are you sure you want to erase this skill?", 0, MB_ICONQUESTION | MB_YESNO) != IDYES)
        return false;
    m_skills.get(index)->reset();
    return true;
}

int ToonSkillList::indexOfNode(Node* node)
{
    return m_skills.indexOf(dynamic_cast<ToonSkill*>(node));
}

Node* ToonSkillList::findNode(Node* like)
{
    return 0;
}

UINT ToonSkillList::getClipboardFormat()
{
    return 527;
}

LRESULT ToonSkillList::clearRows()
{
    return setItemCount(0);
}
