#include "LapisiaEditor.h"
#include "ctlhelpers_g5.h"
#include "ClientData.h"
#include "_imports_p07.h"

LapisiaEditor::LapisiaEditor(Window* owner, Data::Items* items, Data::Item* item)
    : GemEditor(owner, items, item)
{
}

LapisiaEditor::~LapisiaEditor()
{
}

bool LapisiaEditor::createEditor()
{
    HWND parent = m_owner->m_hWnd;
    m_icon.m_columns = 6;
    m_buttons = 1;
    return g4_createModal(this, parent, 390, 490, L"Lapisia Editor");
}

bool LapisiaEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id - 110 <= 6)
    {
        Button& check = m_usedOn[id - 110];
        check.setCheck(check.isChecked() == 0);
        return true;
    }
    if (id == 158)
    {
        m_button324.setCheck(m_button324.isChecked() == 0);
        return true;
    }
    if (id == 303)
    {
        bool on = m_levels.isChecked() == 0;
        m_levels.setCheck(on);
        m_levelMin.enable(on);
        m_levelMax.enable(on);
        return true;
    }
    return ItemEditor::onCommand(code, ctrl, id);
}

bool LapisiaEditor::onCreate()
{
    ImageStrip* icons = g4_clientData()->getItemIcons(FIELD(unsigned char, m_item, 0x54));
    if (icons)
    {
        m_icon.m_columns = 2 * (FIELD(int, icons, 0x20) > 56) + 8;
        m_icon.m_bitmaps = *icons;
        m_icon.m_cells = *icons;
    }
    m_icon.create(m_hWnd, (HMENU)101, 0, 23, 34, 34, 32, 0);

    createNameRow(0, 0, 180);
    createColorRow(90, 45, 40, 120);
    int row = createDescriptionRow(0, 70, 320, 74);

    g4_createLabel(L"Can be used on:", 20, row + 84, 150, 18, m_hWnd, 5);
    int y = row + 100;
    g4_createButton(m_usedOn[0], m_hWnd, 0x6E, L" Helmet", 0x50010002, 20, y, 60, 18);
    g4_createButton(m_usedOn[1], m_hWnd, 0x6F, L" Armor", 0x50000002, 85, y, 60, 18);
    g5_subclassCheck(m_usedOn[1]);
    g4_createButton(m_usedOn[2], m_hWnd, 0x70, L" Pants", 0x50000002, 150, y, 60, 18);
    g5_subclassCheck(m_usedOn[2]);
    g4_createButton(m_usedOn[3], m_hWnd, 0x71, L" Gantlet", 0x50000002, 215, y, 60, 18);
    g5_subclassCheck(m_usedOn[3]);
    g4_createButton(m_usedOn[4], m_hWnd, 0x72, L" Shoes", 0x50000002, 280, y, 60, 18);
    g5_subclassCheck(m_usedOn[4]);
    y += 20;
    g4_createButton(m_usedOn[5], m_hWnd, 0x73, L" Shield", 0x50000002, 20, y, 60, 18);
    g5_subclassCheck(m_usedOn[5]);
    g4_createButton(m_usedOn[6], m_hWnd, 0x74, L" Weapons", 0x50000002, 85, y, 70, 18);
    g5_subclassCheck(m_usedOn[6]);

    y += 30;
    g4_createLabel(L"Characteristics:", 20, y, 150, 18, m_hWnd, 5);
    g4_createLabel(L"Cost:", 20, y + 27, 30, 18, m_hWnd, 4);
    g5_createComboEx(m_cost, m_hWnd, 0x9D, 55, y + 21, 125, 450);
    m_cost.addItem(L"        1.000 golds", 30);
    m_cost.addItem(L"        4.095 golds", 31);
    m_cost.addItem(L"       11.250 golds", 32);
    m_cost.addItem(L"       22.965 golds", 33);
    m_cost.addItem(L"       41.280 golds", 34);
    m_cost.addItem(L"      137.900 golds", 35);
    m_cost.addItem(L"      365.000 golds", 36);
    m_cost.addItem(L"      480.000 golds", 37);
    m_cost.addItem(L"      627.000 golds", 38);
    m_cost.addItem(L"      814.000 golds", 39);
    m_cost.addItem(L"    1.040.000 golds", 40);
    m_cost.addItem(L"    2.500.000 golds", 96);
    m_cost.addItem(L"    3.500.000 golds", 97);
    m_cost.addItem(L"    5.500.000 golds", 98);
    m_cost.addItem(L"    7.500.000 golds", 99);
    m_cost.addItem(L"    9.800.000 golds", 100);

    y += 52;
    g4_createButton(m_levels, m_hWnd, 0x12F, L" Levels", 0x50010002, 20, y, 50, 18);
    g4_createLabel(L"min:", 80, y + 2, 25, 18, m_hWnd, 4);
    m_levelMin.createNumber(m_hWnd, (HMENU)304, 105, y, 30, 18, FIELD(short, m_item, 0x76), 0, 0);
    g4_createLabel(L"max:", 140, y + 2, 25, 18, m_hWnd, 4);
    m_levelMax.createNumber(m_hWnd, (HMENU)305, 170, y, 30, 18, FIELD(unsigned char, m_item, 0x78), 0, 0);

    y += 24;
    g4_createButton(m_button324, m_hWnd, 0x9E, L" Item is destroyed if enhancement fails", 0x50010002,
                    20, y, 210, 18);

    y += 28;
    g4_createLabel(L"Drop group:", 20, y - 2, 40, 26, m_hWnd, 4);
    m_dropGroup.createNumber(m_hWnd, (HMENU)148, 60, y, 50, 18, FIELD(short, m_item, 0xA4), 0, 0);
    g4_createLabel(L"Stack:", 130, y + 2, 40, 18, m_hWnd, 4);
    m_stack.createNumber(m_hWnd, (HMENU)149, 170, y, 50, 18, FIELD(unsigned char, m_item, 0xA9), 0, 0);

    createPriceRow(20, y + 28, this, 1);
    loadControls();
    return Modal::onCreate();
}

void LapisiaEditor::loadControls()
{
    ItemEditor::loadControls();
    g5_selectItemIcon(this);
    m_usedOn[0].setCheck(FIELD(BYTE, m_item, 0x5A) == 1);
    m_usedOn[1].setCheck(FIELD(BYTE, m_item, 0x5B) == 1);
    m_usedOn[2].setCheck(FIELD(BYTE, m_item, 0x5C) == 1);
    m_usedOn[3].setCheck(FIELD(BYTE, m_item, 0x5E) == 1);
    m_usedOn[4].setCheck(FIELD(BYTE, m_item, 0x5F) == 1);
    m_usedOn[5].setCheck(FIELD(BYTE, m_item, 0x5D) == 1);
    m_usedOn[6].setCheck(FIELD(WORD, m_item, 0x58) == 1);
    m_cost.selectData(FIELD(unsigned char, m_item, 0x75));
    m_button324.setCheck(FIELD(WORD, m_item, 0x70) == 1);
    bool levels = FIELD(WORD, m_item, 0x68) == 10000;
    m_levels.setCheck(levels);
    m_levelMin.enable(levels);
    m_levelMax.enable(levels);
    m_levelMin.setInt(FIELD(short, m_item, 0x76));
    m_levelMax.setInt(FIELD(unsigned char, m_item, 0x78));
}

void LapisiaEditor::saveControls()
{
    ItemEditor::saveControls();
    FIELD(BYTE, m_item, 0x5A) = m_usedOn[0].isChecked() != 0;
    FIELD(BYTE, m_item, 0x5B) = m_usedOn[1].isChecked() != 0;
    FIELD(BYTE, m_item, 0x5C) = m_usedOn[2].isChecked() != 0;
    FIELD(BYTE, m_item, 0x5E) = m_usedOn[3].isChecked() != 0;
    FIELD(BYTE, m_item, 0x5F) = m_usedOn[4].isChecked() != 0;
    FIELD(BYTE, m_item, 0x5D) = m_usedOn[5].isChecked() != 0;
    FIELD(WORD, m_item, 0x58) = m_usedOn[6].isChecked() != 0;
    FIELD(WORD, m_item, 0x70) = m_button324.isChecked() != 0;
    FIELD(BYTE, m_item, 0x75) = (BYTE)g5_comboSelData(m_cost);
    if (m_levels.isChecked() == 0)
    {
        FIELD(WORD, m_item, 0x76) = 0;
        FIELD(BYTE, m_item, 0x78) = 0;
        FIELD(WORD, m_item, 0x68) = 0;
    }
    else
    {
        FIELD(WORD, m_item, 0x76) = (WORD)m_levelMin.getInt(FIELD(short, m_item, 0x76));
        FIELD(BYTE, m_item, 0x78) = (BYTE)m_levelMax.getInt(FIELD(unsigned char, m_item, 0x78));
        FIELD(WORD, m_item, 0x68) = 10000;
    }
}
