#include "AccessoryEditor.h"
#include "ctlhelpers_g4.h"
#include "_imports_p07.h"

bool AccessoryEditor::onCreate()
{
    g4_setupIconPicker(this);
    createNameRow(0, 0, 210);
    createColorRow(90, 45, 40, 150);
    int y = createDescriptionRow(0, 70, 320, 74);
    y = createEquipRow(0, y + 76, true, true, true);

    g4_createLabel(L"Characteristics:", 20, y, 150, 18, m_hWnd, 1);
    y += 20;
    g4_createLabel(L"Level:", 30, y + 4, 30, 18, m_hWnd, 4);
    m_level.createNumber(m_hWnd, (HMENU)122, 65, y + 1, 45, 19, FIELD(short, m_item, 0x58), 0, 0);
    g4_createLabel(L"Slot:", 130, y + 4, 30, 18, m_hWnd, 4);
    g4_createCombo(m_slots, m_hWnd, 0x7D, 165, y, 45, 18);
    g4_addComboString(m_slots, L" 0 ");
    g4_addComboString(m_slots, L" 1 ");
    g4_addComboString(m_slots, L" 2 ");
    g4_addComboString(m_slots, L" 3 ");
    g4_addComboString(m_slots, L" 4 ");
    g4_addComboString(m_slots, L" 5 ");
    g4_addComboString(m_slots, L" 6 ");

    g4_createLabel(L"Move Speed:", 230, y - 1, 30, 28, m_hWnd, 4);
    g4_createCombo(m_moveSpeed, m_hWnd, 0x88, 265, y, 45, 200);
    g4_addComboString(m_moveSpeed, L"--");
    g4_addComboString(m_moveSpeed, L"+1");
    g4_addComboString(m_moveSpeed, L"+2");
    g4_addComboString(m_moveSpeed, L"+3");
    g4_addComboString(m_moveSpeed, L"+4 GM");
    g4_addComboString(m_moveSpeed, L"+5 GM");
    g4_addComboString(m_moveSpeed, L"+6 GM");
    SendMessageW(m_moveSpeed.m_hWnd, CB_SETCURSEL, FIELD(unsigned char, m_item, 0x98), 0);

    int y2 = y + 30;
    g4_createLabel(L"Attack min:", 30, y + 25, 34, 24, m_hWnd, 4);
    m_attackMin.createNumber(m_hWnd, (HMENU)127, 65, y2, 45, 18, FIELD(unsigned short, m_item, 0x7E), 0, 0);
    g4_createLabel(L"Attack max:", 130, y2 - 5, 35, 24, m_hWnd, 4);
    m_attackMax.createNumber(m_hWnd, (HMENU)128, 165, y2, 45, 18, FIELD(unsigned short, m_item, 0x80), 0, 0);
    g4_createLabel(L"Atk speed:", 230, y2 - 5, 30, 28, m_hWnd, 4);
    g4_createCombo(m_atkSpeed, m_hWnd, 0x87, 265, y2 - 2, 45, 200);
    g4_addComboString(m_atkSpeed, L"--");
    g4_addComboString(m_atkSpeed, L"+1");
    g4_addComboString(m_atkSpeed, L"+2");
    g4_addComboString(m_atkSpeed, L"+3");
    g4_addComboString(m_atkSpeed, L"+4 GM");
    g4_addComboString(m_atkSpeed, L"+5 GM");
    g4_addComboString(m_atkSpeed, L"+6 GM");
    SendMessageW(m_atkSpeed.m_hWnd, CB_SETCURSEL, FIELD(unsigned char, m_item, 0x78), 0);

    int y3 = createDefenseRow(30, y2 + 25, this, (WPARAM)-1);
    y3 = createStatsRows(30, y3, this);
    y3 = createHpSpMpRow(30, y3, this);
    createStatCountRow(30, y3, this, 0);
    g4_createLabel(L"Drop group:", 230, y3 - 6, 35, 26, m_hWnd, 4);
    m_dropGroup.createNumber(m_hWnd, (HMENU)148, 265, y3, 45, 18, FIELD(short, m_item, 0xA4), 0, 0);
    int y4 = createPriceRow(30, y3 + 26, this, 0);
    g4_createLabel(L"Stack:", 30, y4 + 2, 30, 18, m_hWnd, 4);
    m_stack.createNumber(m_hWnd, (HMENU)149, 65, y4, 45, 18, FIELD(unsigned char, m_item, 0xA9), 0, 0);

    loadControls();
    return Modal::onCreate();
}

void AccessoryEditor::saveControls()
{
    ItemEditor::saveControls();
    FIELD(unsigned char, m_item, 0x78) = (unsigned char)SendMessageW(m_atkSpeed.m_hWnd, CB_GETCURSEL, 0, 0);
    FIELD(unsigned char, m_item, 0x98) = (unsigned char)SendMessageW(m_moveSpeed.m_hWnd, CB_GETCURSEL, 0, 0);
}

AccessoryEditor::AccessoryEditor(Window* owner, Data::Items* items, Data::Item* item)
    : ItemEditor(owner, items, item)
{
}

AccessoryEditor::~AccessoryEditor()
{
}

bool AccessoryEditor::createEditor()
{
    HWND parent = m_owner->m_hWnd;
    m_icon.m_columns = 6;
    m_buttons = 1;
    return g4_createModal(this, parent, 380, 580, L"Accessory Editor");
}
