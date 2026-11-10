#include "LapisEditor.h"
#include "ctlhelpers_g5.h"
#include "ClientData.h"
#include "ContentProvider.h"
#include "String.hpp"
#include "_imports_p07.h"

LapisEditor::LapisEditor(Window* owner, Data::Items* items, Data::Item* item)
    : GemEditor(owner, items, item)
{
}

LapisEditor::~LapisEditor()
{
}

bool LapisEditor::createEditor()
{
    HWND parent = m_owner->m_hWnd;
    m_icon.m_columns = 9;
    m_buttons = 1;
    return g4_createModal(this, parent, 440, 620, L"Lapis Editor");
}

bool LapisEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id - 110 <= 9)
    {
        Button& check = m_links[id - 110];
        check.setCheck(check.isChecked() == 0);
        return true;
    }
    if (id == 158)
    {
        m_button324.setCheck(m_button324.isChecked() == 0);
        return true;
    }
    if (ctrl == m_spell.m_hWnd && code == 1)
    {
        setSpellValue((int)m_spell.getSelectedData());
        return true;
    }
    return ItemEditor::onCommand(code, ctrl, id);
}

void LapisEditor::setSpellValue(int value)
{
    if (value == -1)
    {
        m_spellValue.enable(true);
        m_spellValue.setText(L"");
    }
    else
    {
        m_spellValue.enable(false);
        m_spellValue.setInt(value);
    }
}

bool LapisEditor::onCreate()
{
    ClientData* client = g4_clientData();
    {
        String folder = getContentFolder();
        if (!FIELD(int, client, 0x1CC + 0x20))
            client->loadIconStrip(folder.m_buffer, L"icon_rapis.dds",
                                  reinterpret_cast<ImageStrip*>(reinterpret_cast<BYTE*>(client) + 0x1CC), 1, 0);
    }
    ImageStrip* strip = reinterpret_cast<ImageStrip*>(reinterpret_cast<BYTE*>(client) + 0x1CC);
    if (strip)
    {
        m_icon.m_columns = 2 * (FIELD(int, strip, 0x20) > 56) + 8;
        m_icon.m_bitmaps = *strip;
        m_icon.m_cells = *strip;
    }
    m_icon.create(m_hWnd, (HMENU)101, 0, 22, 34, 34, 32, 0);

    createNameRow(0, 0, 230);
    createColorRow(90, 43, 40, 120);
    int row = createDescriptionRow(0, 75, 380, 56);

    g4_createLabel(L"Can be linked to:", 20, row + 83, 150, 18, m_hWnd, 5);
    int y = row + 101;
    g4_createButton(m_links[6], m_hWnd, 0x74, L"Helmet", 0x50010002, 20, y, 60, 18);
    g4_createButton(m_links[3], m_hWnd, 0x71, L"Gantlet", 0x50000002, 100, y, 60, 18);
    g5_subclassCheck(m_links[3]);
    g4_createButton(m_links[4], m_hWnd, 0x72, L"Shoes", 0x50000002, 180, y, 60, 18);
    g5_subclassCheck(m_links[4]);
    g4_createButton(m_links[1], m_hWnd, 0x6F, L"Pants", 0x50000002, 260, y, 60, 18);
    g5_subclassCheck(m_links[1]);
    g4_createButton(m_links[0], m_hWnd, 0x6E, L"Armor", 0x50000002, 340, y, 60, 18);
    g5_subclassCheck(m_links[0]);
    y += 20;
    g4_createButton(m_links[2], m_hWnd, 0x70, L"Shield", 0x50000002, 20, y, 60, 18);
    g5_subclassCheck(m_links[2]);
    g4_createButton(m_links[7], m_hWnd, 0x75, L"Weapon", 0x50000002, 100, y, 60, 18);
    g5_subclassCheck(m_links[7]);
    g4_createButton(m_links[8], m_hWnd, 0x76, L"Necklace", 0x50000002, 180, y, 60, 18);
    g5_subclassCheck(m_links[8]);
    g4_createButton(m_links[9], m_hWnd, 0x77, L"Loop", 0x50000002, 260, y, 60, 18);
    g5_subclassCheck(m_links[9]);
    g4_createButton(m_links[5], m_hWnd, 0x73, L"Ring", 0x50000002, 340, y, 60, 18);
    g5_subclassCheck(m_links[5]);

    y += 30;
    g4_createLabel(L"Level min.:", 20, y - 4, 35, 26, m_hWnd, 4);
    m_minLevel.createNumber(m_hWnd, (HMENU)122, 55, y, 50, 18, FIELD(unsigned short, m_item, 0x6A), 0, 0);
    g4_createLabel(L"Uses:", 120, y + 2, 35, 18, m_hWnd, 4);
    g4_createCombo(m_slots, m_hWnd, 0x7D, 155, y - 2, 60, 250);
    g4_addComboString(m_slots, L"- invalid");
    g4_addComboString(m_slots, L"1 slot");
    g4_addComboString(m_slots, L"2 slots");
    g4_addComboString(m_slots, L"3 slots");
    g4_createLabel(L"Cost:", 225, y + 2, 30, 18, m_hWnd, 4);
    g5_createComboEx(m_cost, m_hWnd, 0x9D, 255, y - 3, 150, 450);
    m_cost.addItem(L"50%,     1.000 golds", 30);
    m_cost.addItem(L"46%,     4.095 golds", 31);
    m_cost.addItem(L"40%,    11.250 golds", 32);
    m_cost.addItem(L"32%,    22.965 golds", 33);
    m_cost.addItem(L"24%,    41.280 golds", 34);
    m_cost.addItem(L"16%,   137.900 golds", 35);
    m_cost.addItem(L" 8%,   365.000 golds", 36);
    m_cost.addItem(L" 2%,   480.000 golds", 37);
    m_cost.addItem(L"~1%,   627.000 golds", 38);
    m_cost.addItem(L" 1%,   814.000 golds", 39);
    m_cost.addItem(L"<1%, 1.040.000 golds", 40);
    m_cost.addItem(L"<1%, 2.500.000 golds", 96);
    m_cost.addItem(L"<1%, 3.500.000 golds", 97);
    m_cost.addItem(L"<1%, 5.500.000 golds", 98);
    m_cost.addItem(L"<1%, 7.500.000 golds", 99);
    m_cost.addItem(L"<1%, 9.800.000 golds", 100);
    m_cost.selectData(FIELD(unsigned char, m_item, 0x75));

    y += 30;
    g4_createButton(m_button324, m_hWnd, 0x9E, L" Item is destroyed if link or extraction fails", 0x50000002,
                    55, y, 220, 18);
    g5_subclassCheck(m_button324);

    y += 25;
    g4_createLabel(L"Stats:", 20, y, 75, 18, m_hWnd, 5);
    y += 20;
    g4_createLabel(L"STR:", 20, y + 2, 30, 18, m_hWnd, 4);
    m_str.createNumber(m_hWnd, (HMENU)140, 55, y, 50, 18, FIELD(unsigned short, m_item, 0x8C), 0, 0);
    g4_createLabel(L"DEX:", 20, y + 27, 30, 18, m_hWnd, 4);
    m_dex.createNumber(m_hWnd, (HMENU)141, 55, y + 25, 50, 18, FIELD(unsigned short, m_item, 0x8E), 0, 0);
    g4_createLabel(L"REC:", 20, y + 52, 30, 18, m_hWnd, 4);
    m_rec.createNumber(m_hWnd, (HMENU)142, 55, y + 50, 50, 18, FIELD(unsigned short, m_item, 0x90), 0, 0);
    g4_createLabel(L"LUC:", 120, y + 2, 30, 18, m_hWnd, 4);
    m_luc.createNumber(m_hWnd, (HMENU)145, 155, y, 50, 18, FIELD(unsigned short, m_item, 0x96), 0, 0);
    g4_createLabel(L"INT:", 120, y + 27, 30, 18, m_hWnd, 4);
    m_int.createNumber(m_hWnd, (HMENU)143, 155, y + 25, 50, 18, FIELD(unsigned short, m_item, 0x92), 0, 0);
    g4_createLabel(L"WIS:", 120, y + 52, 30, 18, m_hWnd, 4);
    m_wis.createNumber(m_hWnd, (HMENU)144, 155, y + 50, 50, 18, FIELD(unsigned short, m_item, 0x94), 0, 0);
    g4_createLabel(L"HP:", 220, y + 2, 30, 18, m_hWnd, 4);
    m_hp.createNumber(m_hWnd, (HMENU)137, 255, y, 50, 18, FIELD(unsigned short, m_item, 0x86), 0, 0);
    g4_createLabel(L"SP:", 220, y + 27, 30, 18, m_hWnd, 4);
    m_sp.createNumber(m_hWnd, (HMENU)138, 255, y + 25, 50, 18, FIELD(unsigned short, m_item, 0x88), 0, 0);
    g4_createLabel(L"MP:", 220, y + 52, 30, 18, m_hWnd, 4);
    m_mp.createNumber(m_hWnd, (HMENU)139, 255, y + 50, 50, 18, FIELD(unsigned short, m_item, 0x8A), 0, 0);
    g4_createLabel(L"Def:", 320, y + 2, 30, 18, m_hWnd, 4);
    m_defense.createNumber(m_hWnd, (HMENU)129, 355, y, 50, 18, FIELD(unsigned short, m_item, 0x82), 0, 0);
    g4_createLabel(L"Resist:", 320, y + 27, 30, 18, m_hWnd, 4);
    m_resist.createNumber(m_hWnd, (HMENU)130, 355, y + 25, 50, 18, FIELD(unsigned short, m_item, 0x84), 0, 0);
    g4_createLabel(L"Abs°:", 320, y + 52, 34, 18, m_hWnd, 4);
    m_absorb.createNumber(m_hWnd, (HMENU)131, 355, y + 50, 50, 18, FIELD(unsigned char, m_item, 0x99), 0, 0);

    y += 75;
    g4_createLabel(L"Attack:", 20, y, 75, 18, m_hWnd, 5);
    y += 20;
    g4_createLabel(L"Pow min:", 20, y - 4, 35, 26, m_hWnd, 4);
    m_attackMin.createNumber(m_hWnd, (HMENU)127, 55, y, 50, 18, FIELD(unsigned short, m_item, 0x7E), 0, 0);
    g4_createLabel(L"Pow max:", 120, y - 4, 35, 26, m_hWnd, 4);
    m_attackMax.createNumber(m_hWnd, (HMENU)128, 155, y, 50, 18, FIELD(unsigned short, m_item, 0x80), 0, 0);
    g4_createLabel(L"Atk range:", 220, y - 4, 30, 26, m_hWnd, 4);
    g4_createCombo(m_atkRange, m_hWnd, 0x84, 255, y - 2, 50, 200);
    g4_addComboString(m_atkRange, L"--");
    g4_addComboString(m_atkRange, L"+1m");
    g4_addComboString(m_atkRange, L"+2m");
    g4_addComboString(m_atkRange, L"+3m");
    SendMessageW(m_atkRange.m_hWnd, CB_SETCURSEL, FIELD(short, m_item, 0x76), 0);
    g4_createLabel(L"Atk speed:", 320, y - 4, 30, 26, m_hWnd, 4);
    g4_createCombo(m_atkSpeed, m_hWnd, 0x87, 355, y - 2, 50, 200);
    g4_addComboString(m_atkSpeed, L"--");
    g4_addComboString(m_atkSpeed, L"+1");
    g4_addComboString(m_atkSpeed, L"+2");
    g4_addComboString(m_atkSpeed, L"+3");

    y += 30;
    g4_createLabel(L"Special:", 20, y, 75, 18, m_hWnd, 5);
    g4_createLabel(L"Endu:", 20, y + 22, 30, 18, m_hWnd, 4);
    m_endurance.createNumber(m_hWnd, (HMENU)126, 55, y + 19, 50, 19, FIELD(unsigned short, m_item, 0x7C), 0, 0);
    g4_createLabel(L"Move speed:", 120, y + 16, 30, 26, m_hWnd, 4);
    g4_createCombo(m_moveSpeed, m_hWnd, 0x88, 155, y + 18, 50, 200);
    g4_addComboString(m_moveSpeed, L"--");
    g4_addComboString(m_moveSpeed, L"+1");
    g4_addComboString(m_moveSpeed, L"+2");
    g4_addComboString(m_moveSpeed, L"+3");
    g4_createLabel(L"Spell:", 220, y + 22, 30, 18, m_hWnd, 4);
    g5_createComboEx(m_spell, m_hWnd, 0x9B, 255, y + 17, 95, 200);
    m_spell.addItem(L"Other", -1);
    m_spell.addItem(L"None", 0);
    m_spell.addItem(L"Sleep", 1);
    m_spell.addItem(L"Darkness", 4);
    m_spell.addItem(L"Dying", 7);
    m_spell.addItem(L"Death", 8);
    m_spell.addItem(L"Poison", 9);
    m_spell.addItem(L"Illness", 10);
    m_spell.addItem(L"Delusion", 11);
    m_spell.addItem(L"Fear", 13);
    m_spell.addItem(L"Dull", 14);
    m_spell.addItem(L"Bad Luck", 15);
    m_spell.addItem(L"Pain Lv1", 205);
    m_spell.addItem(L"Pain Lv2", 206);
    m_spell.addItem(L"Freezing Lv1", 208);
    m_spell.addItem(L"Freezing Lv2", 209);
    m_spell.addItem(L"Blindside Lv1", 211);
    m_spell.addItem(L"Blindside Lv2", 212);
    m_spell.addItem(L"Silent Lv1", 214);
    m_spell.addItem(L"Silent Lv2", 215);
    m_spell.addItem(L"Crawl Lv1", 217);
    m_spell.addItem(L"Crawl Lv2", 218);
    m_spell.addItem(L"Panic Lv1", 220);
    m_spell.addItem(L"Panic Lv2", 221);
    m_spell.addItem(L"Curse Lv1", 223);
    m_spell.addItem(L"Curse Lv2", 224);
    m_spell.addItem(L"Slowmo Lv1", 226);
    m_spell.addItem(L"Slowmo Lv2", 227);
    m_spell.addItem(L"Toxin Lv1", 229);
    m_spell.addItem(L"Toxin Lv2", 230);
    m_spell.addItem(L"Disease Lv1", 232);
    m_spell.addItem(L"Disease Lv2", 233);
    m_spell.addItem(L"Confusion Lv1", 235);
    m_spell.addItem(L"Confusion Lv2", 236);
    m_spell.addItem(L"Jinx Lv1", 238);
    m_spell.addItem(L"Jinx Lv2", 239);
    m_spell.addItem(L"Slumber Lv1", 241);
    m_spell.addItem(L"Slumber Lv2", 242);
    m_spellValue.createNumber(m_hWnd, (HMENU)156, 355, y + 19, 50, 19, FIELD(unsigned char, m_item, 0x7A), 0, 0);

    y += 50;
    g4_createLabel(L"Drop group:", 20, y - 4, 35, 26, m_hWnd, 4);
    m_dropGroup.createNumber(m_hWnd, (HMENU)148, 55, y, 50, 18, FIELD(short, m_item, 0xA4), 0, 0);
    g4_createLabel(L"Stack:", 120, y + 2, 30, 18, m_hWnd, 4);
    m_stack.createNumber(m_hWnd, (HMENU)149, 155, y, 50, 18, FIELD(unsigned char, m_item, 0xA9), 0, 0);
    g4_createLabel(L"Elem:", 220, y + 2, 30, 18, m_hWnd, 4);
    g4_createCombo(m_element, m_hWnd, 0x86, 255, y - 2, 95, 200);
    g4_addComboString(m_element, L"None");
    g4_addComboString(m_element, L"Fire Lv1");
    g4_addComboString(m_element, L"Water Lv1");
    g4_addComboString(m_element, L"Earth Lv1");
    g4_addComboString(m_element, L"Wind Lv1");
    g4_addComboString(m_element, L"Fire Lv2");
    g4_addComboString(m_element, L"Water Lv2");
    g4_addComboString(m_element, L"Earth Lv2");
    g4_addComboString(m_element, L"Wind Lv2");

    createPriceRow(20, y + 30, this, 1);
    loadControls();
    return Modal::onCreate();
}

void LapisEditor::loadControls()
{
    ItemEditor::loadControls();
    m_links[7].setCheck(FIELD(WORD, m_item, 0x58) == 1);
    m_links[6].setCheck(FIELD(BYTE, m_item, 0x5A) == 1);
    for (int i = 0; i < 6; ++i)
        m_links[i].setCheck(FIELD(BYTE, m_item, 0x5B + i) == 1);
    m_links[8].setCheck(FIELD(BYTE, m_item, 0x61) == 1);
    m_links[9].setCheck(FIELD(WORD, m_item, 0x64) == 1);
    m_minLevel.setInt(FIELD(unsigned short, m_item, 0x6A));
    m_button324.setCheck(FIELD(WORD, m_item, 0x70) != 0);
    m_cost.selectData(FIELD(unsigned char, m_item, 0x75));
    SendMessageW(m_atkSpeed.m_hWnd, CB_SETCURSEL, FIELD(unsigned char, m_item, 0x78), 0);
    SendMessageW(m_moveSpeed.m_hWnd, CB_SETCURSEL, FIELD(unsigned char, m_item, 0x98), 0);
    m_spell.selectData(FIELD(unsigned char, m_item, 0x7A));
    SendMessageW(m_element.m_hWnd, CB_SETCURSEL, FIELD(unsigned char, m_item, 0x79), 0);
    setSpellValue(FIELD(unsigned char, m_item, 0x7A));
}

void LapisEditor::saveControls()
{
    ItemEditor::saveControls();
    for (int i = 0; i < 6; ++i)
        FIELD(BYTE, m_item, 0x5B + i) = m_links[i].isChecked() != 0;
    FIELD(BYTE, m_item, 0x5A) = m_links[6].isChecked() != 0;
    FIELD(WORD, m_item, 0x58) = m_links[7].isChecked() != 0;
    FIELD(BYTE, m_item, 0x61) = m_links[8].isChecked() != 0;
    FIELD(WORD, m_item, 0x64) = m_links[9].isChecked() != 0;
    FIELD(WORD, m_item, 0x6A) = (WORD)m_minLevel.getInt(FIELD(unsigned short, m_item, 0x6A));
    FIELD(WORD, m_item, 0x70) = m_button324.isChecked() != 0;
    FIELD(BYTE, m_item, 0x75) = (BYTE)g5_comboSelData(m_cost);
    FIELD(WORD, m_item, 0x76) = (unsigned char)SendMessageW(m_atkRange.m_hWnd, CB_GETCURSEL, 0, 0);
    FIELD(BYTE, m_item, 0x78) = (BYTE)SendMessageW(m_atkSpeed.m_hWnd, CB_GETCURSEL, 0, 0);
    FIELD(BYTE, m_item, 0x79) = (BYTE)SendMessageW(m_element.m_hWnd, CB_GETCURSEL, 0, 0);
    FIELD(BYTE, m_item, 0x7A) = (BYTE)g5_comboSelData(m_spell);
#pragma warning(suppress : 4127 6287)
    if ((int)FIELD(unsigned char, m_item, 0x7A) == -1)
        FIELD(BYTE, m_item, 0x7A) = (BYTE)m_spellValue.getInt(FIELD(unsigned char, m_item, 0x7A));
    FIELD(BYTE, m_item, 0x98) = (BYTE)SendMessageW(m_moveSpeed.m_hWnd, CB_GETCURSEL, 0, 0);
}
