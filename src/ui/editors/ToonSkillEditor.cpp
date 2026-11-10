#include "ToonSkillEditor.h"
#include <cwchar>
#include <cstring>
#include "ida_compat.h"
#include "String.hpp"
#include "SkillData.h"
#include "ClientData.h"
#include "SkillDescEditor.h"
#include "SkillCombos.h"
#include "SkillEditorUi.h"
#include "_imports_H.h"

#define TOONSKILL_FORMAT(s)          FIELD(DWORD, s, 0x0C)
#define TOONSKILL_LEVEL_COUNT(s)     FIELD(int, s, 0x10)
#define TOONSKILL_NAME(s)            FIELD(String, s, 0x18)
#define TOONSKILL_LEVEL_NAME(s, i)   FIELD(String, s, 0x38 + 0x20 * (i))
#define TOONSKILL_DESCRIPTION(s, i)  FIELD(String, s, 0x218 + 0x20 * (i))
#define TOONSKILL_LEVEL(s, i)        FIELD(SkillData, s, 0x3F8 + 0xA4 * (i))

#define STRIP_IDS(b)                 FIELD(int*, &(b), 0x08)
#define STRIP_IDS_BOUND(b)           FIELD(int, &(b), 0x10)
#define DROPDOWN_SELECTED(d)         FIELD(int, &(d), 0x48)

ToonSkillEditor::ToonSkillEditor(ToonSkill* skill)
    : m_skill(skill)
{
    for (int i = 0; i < 15; ++i)
        m_descriptions[i] = TOONSKILL_DESCRIPTION(skill, i);
}

ToonSkillEditor::~ToonSkillEditor()
{
}

bool ToonSkillEditor::createEditor(HWND parent)
{
    return Modal::createDialog(parent, 480, 450, 1, L"Toon Skill Editor");
}

bool ToonSkillEditor::onCreate()
{
    int y = buildControls();
    SIZE size = createTypeControls(y, (short)(unsigned short)TOONSKILL_LEVEL(m_skill, 0).typeDetail);
    resize(size.cx + 30, size.cy + 70);
    loadControls();
    return Modal::onCreate();
}

int ToonSkillEditor::mapCommandToResult(int id)
{
    if (id == 1)
        return saveControls();
    return id;
}

int ToonSkillEditor::buildControls()
{
    ImageStrip* icons = reinterpret_cast<ClientData*>((char*)g_studioFrame + 0x230)->getIcons(L"icon_skills");
    if (icons)
    {
        m_icon.m_bitmaps.ImageStrip::operator=(*icons);
        m_icon.m_cells.ImageStrip::operator=(*icons);
        HWND parent = m_hWnd;
        m_icon.m_columns = 24;
        m_icon.create(parent, (HMENU)101, 0, 21, 34, 34, 32, 0);
    }

    createStatic(90, 18, 40, 14, L"Name:", 0, 4);
    skui_createEdit(m_name, m_hWnd, 0x50010080, 140, 16, 280, 18, 0x67, 0x1A);

    createStatic(90, 45, 40, 14, L"Family:", 0, 4);
    skui_createComboEx(m_family, m_hWnd, 140, 40, 280, 250, 0x71);
    SkillCombo_fillFamilies(m_family);

    createStatic(80, 72, 50, 18, L"Category:", 0, 4);
    skui_createCombo(m_category, m_hWnd, 140, 68, 100, 150, 0x6E);
    skui_addString(m_category, L"Noos - Other");
    skui_addString(m_category, L"Passive");
    skui_addString(m_category, L"Basic");
    skui_addString(m_category, L"Combat");
    skui_addString(m_category, L"Special");

    createStatic(260, 72, 50, 18, L"Animation:", 0, 4);
    m_animation.EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)102, 320, 70, 50, 18, 0, 0, 0);

    createStatic(20, 94, 100, 16, L"Description:", 0, 4);
    skui_createEdit(m_description, m_hWnd, 0x50211044, 20, 110, 320, 48, 0x68, 0x200);
    m_description.enable(false);
    skui_createButton(m_editDescription, m_hWnd, L"Edit...", 0x50000000, 360, 122, 60, 24, 0x36B);

    createStatic(20, 172, 50, 18, L"Used by:", 0, 4);
    skui_createButton(m_classes[0], m_hWnd, L"Fighter", 0x50010002, 70, 170, 60, 18, 0x78);
    skui_createButton(m_classes[1], m_hWnd, L"Defender", 0x50000002, 135, 170, 60, 18, 0x79);
    skui_subclassCheckBox(m_classes[1]);
    skui_createButton(m_classes[2], m_hWnd, L"Ranger", 0x50000002, 200, 170, 60, 18, 0x7A);
    skui_subclassCheckBox(m_classes[2]);
    skui_createButton(m_classes[3], m_hWnd, L"Archer", 0x50000002, 265, 170, 60, 18, 0x7B);
    skui_subclassCheckBox(m_classes[3]);
    skui_createButton(m_classes[4], m_hWnd, L"Mage", 0x50000002, 330, 170, 60, 18, 0x7C);
    skui_subclassCheckBox(m_classes[4]);
    skui_createButton(m_classes[5], m_hWnd, L"Priest", 0x50000002, 395, 170, 60, 18, 0x7D);
    skui_subclassCheckBox(m_classes[5]);

    createStatic(20, 202, 40, 18, L"Fct°:", 0, 4);
    skui_createCombo(m_faction, m_hWnd, 55, 198, 120, 200, 0x69);
    skui_addString(m_faction, L"Human");
    skui_addString(m_faction, L"Elf");
    skui_addString(m_faction, L"Light");
    skui_addString(m_faction, L"Deatheater");
    skui_addString(m_faction, L"Vail");
    skui_addString(m_faction, L"Fury");
    skui_addString(m_faction, L"All factions");

    createStatic(185, 202, 45, 18, L"in mode:", 0, 4);
    skui_createCombo(m_mode, m_hWnd, 230, 198, 120, 150, 0x6A);
    skui_addString(m_mode, L"Easy or higher");
    skui_addString(m_mode, L"Normal or higher");
    skui_addString(m_mode, L"Difficult or higher");
    skui_addString(m_mode, L"Ultimate");

    createStatic(20, 230, 35, 24, L"Type:", 0, 4);
    skui_createCombo(m_type, m_hWnd, 55, 226, 120, 150, 0x6F);
    skui_addString(m_type, L"Passive");
    skui_addString(m_type, L"Physical Attack");
    skui_addString(m_type, L"Shooting Attack");
    skui_addString(m_type, L"Magic Attack");

    createStatic(190, 230, 40, 24, L"Effect:", 0, 4);
    skui_createCombo(m_effect, m_hWnd, 230, 226, 120, 150, 0x70);
    skui_addString(m_effect, L"Buff, Noss, …");
    skui_addString(m_effect, L"Basic Damage");
    skui_addString(m_effect, L"Healing, Dispel");
    skui_addString(m_effect, L"Debuff");
    skui_addString(m_effect, L"Buff");
    skui_addString(m_effect, L"Fixed Damage");
    skui_addString(m_effect, L"Other");

    createStatic(20, 258, 150, 24, L"Skill compliant / usable with:", 0, 4);

    int y = 276;
    int rowId = 130;
    const wchar_t** name = g_weaponTypeNames;
    Button* box = m_weapons;
    do
    {
        int column = 0;
        int x = 20;
        do
        {
            skui_createButton(*box, m_hWnd, *name, 0x50000002, x, y, 100, 18, rowId + column);
            skui_subclassCheckBox(*box);
            ++column;
            x += 110;
            ++name;
            ++box;
        } while (x < 460);
        rowId += 4;
        y += 18;
    } while (name < g_weaponTypeNames + 16);
    return y;
}

void ToonSkillEditor::loadControls()
{
    SkillData& first = TOONSKILL_LEVEL(m_skill, 0);

    int bound = STRIP_IDS_BOUND(m_icon.m_bitmaps);
    int sel = 0;
    if (bound <= 0)
    {
        sel = -1;
    }
    else
    {
        int* ids = STRIP_IDS(m_icon.m_bitmaps);
        while (*ids != TOONSKILL_LEVEL(m_skill, 0).icon)
        {
            ++sel;
            ++ids;
            if (sel >= bound)
            {
                sel = -1;
                break;
            }
        }
    }
    m_icon.m_selected = sel;
    m_icon.showIcon(sel);
    DROPDOWN_SELECTED(m_icon.m_dropDown) = sel;

    m_name.setText(TOONSKILL_NAME(m_skill).c_str());
    m_description.setText(TOONSKILL_DESCRIPTION(m_skill, 0).c_str());
    SendMessageW(m_category.m_hWnd, CB_SETCURSEL, first.typeShow, 0);
    m_animation.EditText::setInt(first.anim);
    for (int i = 0; i < 6; ++i)
        m_classes[i].setCheck(first.classes[i] == 1);
    SendMessageW(m_faction.m_hWnd, CB_SETCURSEL, first.country, 0);
    SendMessageW(m_mode.m_hWnd, CB_SETCURSEL, first.grow, 0);
    SendMessageW(m_type.m_hWnd, CB_SETCURSEL, first.typeAttack, 0);
    SendMessageW(m_effect.m_hWnd, CB_SETCURSEL, first.typeEffect, 0);
    short family = first.typeDetail;
    m_family.ComboBox::selectData((WPARAM)(int)family);
    m_family.enable(family == 0);
    if (family == 5)
        m_editDescription.enable(false);
    for (int j = 0; j < 15; ++j)
        m_weapons[j].setCheck(first.needWeapon[j] == 1);
    m_weapons[15].setCheck(first.shield == 1);
    if (m_stateType.m_hWnd)
        SendMessageW(m_stateType.m_hWnd, CB_SETCURSEL, first.stateType, 0);

    if (m_statTypes[0].m_hWnd)
    {
        int count = 3;
        if ((TOONSKILL_FORMAT(m_skill) & 0xFFF) >= 0x600)
            count = 10;
        for (int k = 0; k < count; ++k)
        {
            if (!m_statTypes[k].m_hWnd || !skui_curData(m_statTypes[k]))
            {
                for (int level = 0; level < TOONSKILL_LEVEL_COUNT(m_skill); ++level)
                    m_statValues[level][k].enable(false);
            }
        }
    }
}

bool ToonSkillEditor::saveControls()
{
    LRESULT family = skui_curData(m_family);

    String name;
    m_name.getText(&name);
    TOONSKILL_NAME(m_skill) = name;

    int statCount = 3;
    if ((TOONSKILL_FORMAT(m_skill) & 0xFFF) >= 0x600)
        statCount = 10;

    for (int i = 0; i < TOONSKILL_LEVEL_COUNT(m_skill); ++i)
    {
        SkillData& level = TOONSKILL_LEVEL(m_skill, i);
        level.level = (unsigned char)(i + 1);
        level.reqLevel = (short)m_reqLevel[i].EditText::getInt(0);

        if (name.length())
        {
            String& levelName = TOONSKILL_LEVEL_NAME(m_skill, i);
            levelName = name;
            levelName << L" Lv";
            levelName << (long)(i + 1);
        }
        TOONSKILL_DESCRIPTION(m_skill, i) = m_descriptions[i];

        if (family == 2)
        {
            if (i <= 2)
            {
                level.bag = (unsigned char)m_bags[i].EditText::getInt(0);
            }
            else
            {
                int version = level.version;
                int skillId = level.skillId;
                memset(&level, 0, sizeof(SkillData));
                level.version = version;
                level.skillId = skillId;
            }
        }
        level.typeDetail = (short)family;
        level.typeShow = (unsigned char)SendMessageW(m_category.m_hWnd, CB_GETCURSEL, 0, 0);
        level.anim = (unsigned char)m_animation.EditText::getInt(-1);
        level.country = (unsigned char)SendMessageW(m_faction.m_hWnd, CB_GETCURSEL, 0, 0);
        level.grow = (unsigned char)SendMessageW(m_mode.m_hWnd, CB_GETCURSEL, 0, 0);
        level.typeAttack = (unsigned char)SendMessageW(m_type.m_hWnd, CB_GETCURSEL, 0, 0);
        level.typeEffect = (unsigned char)SendMessageW(m_effect.m_hWnd, CB_GETCURSEL, 0, 0);
        level.stateType = (unsigned char)SendMessageW(m_stateType.m_hWnd, CB_GETCURSEL, 0, 0);
        level.damageType = (unsigned char)SendMessageW(m_damageType.m_hWnd, CB_GETCURSEL, 0, 0);
        for (int c = 0; c < 6; ++c)
            level.classes[c] = m_classes[c].isChecked() != 0;
        for (int w = 0; w < 15; ++w)
            level.needWeapon[w] = m_weapons[w].isChecked() != 0;
        level.shield = m_weapons[15].isChecked() != 0;

        if (m_icon.m_hWnd)
        {
            int sel = m_icon.m_selected;
            short icon;
            if (sel < 0 || sel >= STRIP_IDS_BOUND(m_icon.m_bitmaps))
                icon = 0;
            else
                icon = (short)STRIP_IDS(m_icon.m_bitmaps)[sel];
            level.icon = icon;
        }
        if (m_requiredWeapon1.m_hWnd)
            level.weapon[0] = (unsigned char)SendMessageW(m_requiredWeapon1.m_hWnd, CB_GETCURSEL, 0, 0);
        if (m_requiredWeapon2.m_hWnd)
            level.weapon[1] = (unsigned char)SendMessageW(m_requiredWeapon2.m_hWnd, CB_GETCURSEL, 0, 0);

        if (m_weapon[i].m_hWnd)
            level.weaponValue = (unsigned char)m_weapon[i].EditText::getInt(0);
        if (m_points[i].m_hWnd)
            level.points = (unsigned char)m_points[i].EditText::getInt(0);
        if (m_costSP[i].m_hWnd)
            level.sp = (short)m_costSP[i].EditText::getInt(0);
        if (m_costMP[i].m_hWnd)
            level.mp = (short)m_costMP[i].EditText::getInt(0);
        if (m_castTime[i].m_hWnd)
            level.readyTime = (unsigned char)m_castTime[i].EditText::getInt(0);
        if (m_resetTime[i].m_hWnd)
            level.resetTime = (short)m_resetTime[i].EditText::getInt(0);
        if (m_chanceEnabled[i].m_hWnd && m_chanceEnabled[i].isChecked())
        {
            level.successType = 1;
            level.successValue = (unsigned char)m_chance[i].EditText::getInt(0);
        }
        else
        {
            level.successType = 0;
            level.successValue = 0;
        }

        for (int k = 0; k < statCount; ++k)
        {
            if (!m_statTypes[k].m_hWnd)
                continue;
            LRESULT type = skui_curData(m_statTypes[k]);
            UINT value = m_statValues[i][k].EditText::getInt(0);
            if ((unsigned char)type && value)
            {
                level.ability[k].type = (unsigned char)type;
                level.ability[k].value = (short)value;
            }
        }

        if (m_hpLimit[i].m_hWnd)
            level.limitHP = (unsigned char)m_hpLimit[i].EditText::getInt(0);
        if (m_fixRange[i].m_hWnd)
            level.fixRange = (unsigned char)m_fixRange[i].EditText::getInt(0);
        if (m_element[i].m_hWnd)
            level.attribType = (unsigned char)SendMessageW(m_element[i].m_hWnd, CB_GETCURSEL, 0, 0);
        if (m_target[i].m_hWnd)
            level.targetType = (unsigned char)(SendMessageW(m_target[i].m_hWnd, CB_GETCURSEL, 0, 0) + 1);
        if (m_distance[i].m_hWnd)
            level.attackRange = (unsigned char)m_distance[i].EditText::getInt(0);
        if (m_range[i].m_hWnd)
            level.applyRange = (unsigned char)m_range[i].EditText::getInt(0);
        if (m_keepTime[i].m_hWnd)
            level.keepTime = (unsigned short)m_keepTime[i].EditText::getInt(0);
        if (m_hits[i].m_hWnd)
            level.multiAttack = (unsigned char)m_hits[i].EditText::getInt(0);
        if (m_baseDmgHP[i].m_hWnd)
            level.damage[0] = (short)m_baseDmgHP[i].EditText::getInt(0);
        if (m_baseDmgSP[i].m_hWnd)
            level.damage[1] = (short)m_baseDmgSP[i].EditText::getInt(0);
        if (m_baseDmgMP[i].m_hWnd)
            level.damage[2] = (short)m_baseDmgMP[i].EditText::getInt(0);
        if (m_addDmgHP[i].m_hWnd)
            level.addDamage[0] = (short)m_addDmgHP[i].EditText::getInt(0);
        if (m_addDmgSP[i].m_hWnd)
            level.addDamage[1] = (short)m_addDmgSP[i].EditText::getInt(0);
        if (m_addDmgMP[i].m_hWnd)
            level.addDamage[2] = (short)m_addDmgMP[i].EditText::getInt(0);
        if (m_dotHP[i].m_hWnd)
            level.timeDamage[0] = (short)m_dotHP[i].EditText::getInt(0);
        if (m_dotSP[i].m_hWnd)
            level.timeDamage[1] = (short)m_dotSP[i].EditText::getInt(0);
        if (m_dotMP[i].m_hWnd)
            level.timeDamage[2] = (short)m_dotMP[i].EditText::getInt(0);
        if (m_healHP[i].m_hWnd)
            level.heal[0] = (unsigned short)m_healHP[i].EditText::getInt(0);
        if (m_healSP[i].m_hWnd)
            level.heal[1] = (unsigned short)m_healSP[i].EditText::getInt(0);
        if (m_healMP[i].m_hWnd)
            level.heal[2] = (unsigned short)m_healMP[i].EditText::getInt(0);
        if (m_hotHP[i].m_hWnd)
            level.timeHeal[0] = (unsigned short)m_hotHP[i].EditText::getInt(0);
        if (m_hotSP[i].m_hWnd)
            level.timeHeal[1] = (unsigned short)m_hotSP[i].EditText::getInt(0);
        if (m_hotMP[i].m_hWnd)
            level.timeHeal[2] = (unsigned short)m_hotMP[i].EditText::getInt(0);
        if (m_defenceType.m_hWnd)
            level.defenceType = (unsigned char)SendMessageW(m_defenceType.m_hWnd, CB_GETCURSEL, 0, 0);
        if (m_defence[i].m_hWnd)
            level.defenceValue = (unsigned char)m_defence[i].EditText::getInt(0);
        if (m_aggro[i].m_hWnd)
            level.arrow = (short)m_aggro[i].EditText::getInt(0);
    }
    return true;
}

bool ToonSkillEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id == 875)
    {
        SkillDescEditor dialog(m_descriptions, TOONSKILL_LEVEL_COUNT(m_skill));
        if (dialog.createEditor(m_hWnd))
            dialog.doModal();
        return true;
    }
    if (id - 120 <= 5)
    {
        Button& box = m_classes[id - 120];
        box.setCheck(box.isChecked() == 0);
        return true;
    }
    if (id - 130 <= 15)
    {
        Button& box = m_weapons[id - 130];
        box.setCheck(box.isChecked() == 0);
        return true;
    }
    if (id >= 305 && id < (UINT)(TOONSKILL_LEVEL_COUNT(m_skill) + 305))
    {
        UINT n = id - 305;
        bool checked = m_chanceEnabled[n].isChecked() == 0;
        m_chanceEnabled[n].setCheck(checked);
        m_chance[n].enable(checked);
        return true;
    }
    if (code != 1)
        return Modal::onCommand(code, ctrl, id);
    if (id < 835 || id >= (UINT)(TOONSKILL_LEVEL_COUNT(m_skill) + 835))
    {
        if (id - 350 <= 10)
        {
            ComboBox* combo = &m_statTypes[0] + (id - 350);
            LRESULT data = combo->ComboBox::getSelectedData();
            enableStatColumn(id - 350, data != 0);
            return true;
        }
        return Modal::onCommand(code, ctrl, id);
    }

    UINT n = id - 835;
    switch ((unsigned char)m_target[n].ComboBox::getCurSel())
    {
    case 0xFF:
        m_distance[n].enable(false);
        m_range[n].enable(false);
        return true;
    case 0:
    case 2:
    case 6:
        m_distance[n].enable(true);
        m_range[n].enable(true);
        return true;
    case 1:
    case 3:
    case 4:
    case 5:
    case 7:
        m_distance[n].enable(false);
        m_range[n].enable(true);
        return true;
    default:
        return true;
    }
}

SIZE ToonSkillEditor::createTypeControls(int y, short type)
{
    y += 12;
    switch (type)
    {
    case 1:
    case 8:
    case 10:
        return createWeaponControls(y);
    case 2:
        return createBagsControls(y);
    case 4:
        return createEnhanceControls(y);
    case 5:
        return createLevelDescriptionControls(y);
    case 6:
        return createHpLimitControls(y);
    default:
        return createAttackControls(y, type);
    }
}

SIZE ToonSkillEditor::createBagsControls(int y)
{
    SkillData* levels = &TOONSKILL_LEVEL(m_skill, 0);
    createStatic(40, y, 20, 18, L"#", 0, 4);
    createStatic(60, y, 25, 18, L"Lvl", 0, 4);
    createStatic(95, y, 25, 18, L"Pts", 0, 4);
    createStatic(125, y, 50, 18, L"« bags »", 0, 4);

    wchar_t number[16] = {};
    int rowY = y + 18;
    int id = 200;
    int rows = 3;
    int i = 0;
    do
    {
        swprintf_s(number, 16, L"%d", id - 199);
        createStatic(40, rowY + 3, 20, 18, number, 0, 1);
        m_reqLevel[i].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)id, 55, rowY, 25, 18, levels[i].reqLevel, 0, 0);
        m_points[i].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)(id + 15), 90, rowY, 25, 18, levels[i].points, 0, 0);
        m_bags[i].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)(id - 84), 125, rowY, 40, 18, levels[i].bag, 0, 0);
        ++i;
        rowY += 20;
        ++id;
    } while (--rows);

    SIZE size;
    size.cy = rowY + 16;
    size.cx = 450;
    return size;
}

SIZE ToonSkillEditor::createWeaponControls(int y)
{
    SkillData* levels = &TOONSKILL_LEVEL(m_skill, 0);
    createStatic(20, y + 4, 50, 18, L"Requires:", 0, 4);
    skui_createCombo(m_requiredWeapon1, m_hWnd, 70, y, 130, 150, 0x72);
    createStatic(210, y + 4, 20, 18, L"or", 0, 4);
    skui_createCombo(m_requiredWeapon2, m_hWnd, 230, y, 130, 150, 0x73);
    skui_addString(m_requiredWeapon1, L"None");
    skui_addString(m_requiredWeapon2, L"None");
    for (const wchar_t** name = g_weaponTypeNames; name < g_weaponTypeNames + 16; ++name)
    {
        skui_addString(m_requiredWeapon1, *name);
        skui_addString(m_requiredWeapon2, *name);
    }
    int weapon = levels[0].weapon[0];
    if (weapon == 19)
        weapon = 16;
    SendMessageW(m_requiredWeapon1.m_hWnd, CB_SETCURSEL, weapon, 0);
    weapon = levels[0].weapon[1];
    if (weapon == 19)
        weapon = 16;
    SendMessageW(m_requiredWeapon2.m_hWnd, CB_SETCURSEL, weapon, 0);

    int headerY = y + 28;
    createStatic(30, headerY, 20, 18, L"#", 0, 4);
    createStatic(55, headerY, 25, 18, L"Lvl", 0, 4);
    createStatic(95, headerY, 25, 18, L"Pts", 0, 4);
    createStatic(130, headerY, 80, 18, L"Incr. Speed", 0, 4);

    wchar_t number[16] = {};
    int rowY = y + 46;
    if (TOONSKILL_LEVEL_COUNT(m_skill) > 0)
    {
        int id = 200;
        int i = 0;
        do
        {
            swprintf_s(number, 16, L"%d", id - 199);
            createStatic(30, rowY + 3, 20, 18, number, 0, 1);
            m_reqLevel[i].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)id, 50, rowY, 30, 18, levels[i].reqLevel, 0, 0);
            m_points[i].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)(id + 15), 90, rowY, 30, 18, levels[i].points, 0, 0);
            m_weapon[i].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)(id + 135), 130, rowY, 40, 18, levels[i].weaponValue, 0, 0);
            rowY += 20;
            ++i;
            ++id;
        } while (id - 200 < TOONSKILL_LEVEL_COUNT(m_skill));
    }

    SIZE size;
    size.cx = 450;
    size.cy = rowY + 16;
    return size;
}
