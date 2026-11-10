#include "p05_common.h"
#include "MonsterEditor.h"
#include "Data_Item.h"

MonsterEditor::ItemsList::~ItemsList()
{
}

DWORD MonsterEditor::ItemsList::getStyle()
{
    return 5;
}

bool MonsterEditor::ItemsList::onCreate()
{
    bool result = ListCtrl::onCreate();
    if (result) {
        addColumn(L"Cntr", 50, 0, 'cnts');
        addColumn(L"Name", 150, 4096, 'name');
        return true;
    }
    return result;
}

Node* MonsterEditor::ItemsList::getNode(int index)
{
    return (Node*)m_items.get(index);
}

int MonsterEditor::ItemsList::indexOfNode(Node* node)
{
    return m_items.indexOf(dynamic_cast<Data::Item*>(node));
}

MonsterEditor::MonsterEditor(Monster* monster, void* data)
    : Modal(),
      m_monster(monster), m_data(data),
      m_icon(),
      m_edits(), m_combos(), m_debuffs(), m_stats(), m_edits2(), m_attackTypes(),
      m_attackA(), m_attackB(), m_attackC(), m_attackD(), m_attackSkills(), m_attackE(), m_attackF(),
      m_dropPanel(),
      m_btn6F0(),
      m_bags(this, monster),
      m_edit810(), m_edit81C(), m_btn828(), m_btn834(),
      m_items(),
      m_btn95C(),
      m_preview(panelBackgroundColor(), 0)
{
    m_6FC = false;
    m_9E0 = false;
}

MonsterEditor::~MonsterEditor()
{
}

void MonsterEditor::reserved73()
{
    if (m_icon.m_hWnd)
        ((Window*)((uint8_t*)&m_icon + 0xDC))->show(false);
}
