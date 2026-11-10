#include "CustomEditor.h"
#include "ctlhelpers_g4.h"
#include "_imports_p07.h"

CustomEditor::CustomEditor(Window* owner, Data::Items* items, Data::Item* item)
    : ItemEditor(owner, items, item)
{
}

CustomEditor::~CustomEditor()
{
}

bool CustomEditor::createEditor()
{
    HWND parent = m_owner->m_hWnd;
    m_icon.m_columns = 6;
    m_buttons = 1;
    return g4_createModal(this, parent, 370, 530, L"Custom Editor");
}

bool CustomEditor::onCreate()
{
    g4_setupIconPicker(this);
    createNameRow(0, 0, 210);
    createColorRow(90, 45, 40, 150);
    int y = createDescriptionRow(0, 70, 320, 74);
    y = createEquipRow(0, y + 76, true, true, true);

    g4_createLabel(L"Characteristics:", 20, y, 150, 18, m_hWnd, 1);
    int y2 = createDefenseRow(30, y + 20, this, (WPARAM)-1);
    y2 = createStatsRows(30, y2, this);
    y2 = createHpSpMpRow(30, y2, this);
    createStatCountRow(30, y2, this, 0);
    g4_createLabel(L"Drop group:", 230, y2 - 6, 35, 26, m_hWnd, 4);
    m_dropGroup.createNumber(m_hWnd, (HMENU)148, 265, y2, 45, 18, FIELD(short, m_item, 0xA4), 0, 0);
    int y3 = createPriceRow(30, y2 + 26, this, 0);
    g4_createLabel(L"Stack:", 30, y3 + 2, 30, 18, m_hWnd, 4);
    m_stack.createNumber(m_hWnd, (HMENU)149, 65, y3, 45, 18, FIELD(unsigned char, m_item, 0xA9), 0, 0);

    loadControls();
    return Modal::onCreate();
}

void CustomEditor::saveControls()
{
    ItemEditor::saveControls();
    FIELD(unsigned char, m_item, 0x78) = (unsigned char)SendMessageW(m_atkSpeed.m_hWnd, CB_GETCURSEL, 0, 0);
    FIELD(unsigned char, m_item, 0x98) = (unsigned char)SendMessageW(m_moveSpeed.m_hWnd, CB_GETCURSEL, 0, 0);
}
