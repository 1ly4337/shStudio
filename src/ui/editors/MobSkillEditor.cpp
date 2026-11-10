#include "MobSkillEditor.h"
#include <cwchar>
#include "ida_compat.h"
#include "String.hpp"
#include "SkillData.h"
#include "ClientData.h"
#include "SkillCombos.h"
#include "SkillEditorUi.h"
#include "_imports_H.h"

#define MOBSKILL_NAME(s)        FIELD(String, s, 0x0C)
#define MOBSKILL_DESCRIPTION(s) FIELD(String, s, 0x2C)
#define MOBSKILL_DATA(s)        FIELD(SkillData, s, 0x4C)

#define STRIP_IDS(b)            FIELD(int*, &(b), 0x08)
#define STRIP_IDS_BOUND(b)      FIELD(int, &(b), 0x10)
#define DROPDOWN_SELECTED(d)    FIELD(int, &(d), 0x48)

MobSkillEditor::MobSkillEditor(MobsSkill* skill)
    : m_skill(skill)
{
}

MobSkillEditor::~MobSkillEditor()
{
}

void MobSkillEditor::reserved73()
{
    if (m_icon.m_hWnd)
        m_icon.m_dropDown.show(false);
}

bool MobSkillEditor::createEditor(HWND parent)
{
    SkillData& data = MOBSKILL_DATA(m_skill);
    m_icon.m_columns = 16;
    int cy = 560;
    if ((data.version & 0xFFF) >= 0x600)
        cy = 648;
    return Modal::createDialog(parent, 485, cy, 1, L"Mob Skill Editor");
}

bool MobSkillEditor::onCreate()
{
    bool result = buildControls();
    if (result)
    {
        loadControls();
        return Modal::onCreate();
    }
    return result;
}

int MobSkillEditor::mapCommandToResult(int id)
{
    if (id == 1)
        saveControls();
    return id;
}

bool MobSkillEditor::buildControls()
{
    SkillData& data = MOBSKILL_DATA(m_skill);

    ImageStrip* icons = reinterpret_cast<ClientData*>((char*)g_studioFrame + 0x230)->getIcons(L"icon_skills");
    if (icons)
    {
        m_icon.m_bitmaps.ImageStrip::operator=(*icons);
        m_icon.m_cells.ImageStrip::operator=(*icons);
        HWND parent = m_hWnd;
        m_icon.m_columns = 18;
        m_icon.create(parent, (HMENU)101, 0, 20, 34, 34, 32, 0);
    }

    createStatic(90, 22, 40, 14, L"Name:", 0, 4);
    skui_createEdit(m_editName, m_hWnd, 0x50010080, 140, 20, 310, 18, 0x67, 0x1A);

    createStatic(90, 49, 40, 14, L"Family:", 0, 4);
    skui_createComboEx(m_cbFamily, m_hWnd, 140, 44, 310, 250, 0x71);
    SkillCombo_fillFamilies(m_cbFamily);

    createStatic(20, 68, 100, 16, L"Description:", 0, 4);
    skui_createEdit(m_editDescription, m_hWnd, 0x50211044, 20, 84, 430, 48, 0x68, 0x200);

    createStatic(20, 147, 80, 14, L"Attack Type:", 0, 4);
    skui_createCombo(m_cbAttackType, m_hWnd, 100, 142, 110, 150, 0x6F);
    skui_addString(m_cbAttackType, L"Passive");
    skui_addString(m_cbAttackType, L"Physical Attack");
    skui_addString(m_cbAttackType, L"Shooting Attack");
    skui_addString(m_cbAttackType, L"Magic Attack");

    createStatic(230, 147, 60, 14, L"Target(s):", 0, 4);
    skui_createComboEx(m_cbTargets, m_hWnd, 290, 142, 160, 250, 0x343);
    SkillCombo_fillMobTargets(m_cbTargets);

    createStatic(20, 177, 80, 14, L"Effect:", 0, 4);
    skui_createCombo(m_cbEffect, m_hWnd, 100, 172, 110, 150, 0x70);
    skui_addString(m_cbEffect, L"Buff, Noss, …");
    skui_addString(m_cbEffect, L"Basic Damage");
    skui_addString(m_cbEffect, L"Healing, Dispel");
    skui_addString(m_cbEffect, L"Debuff");
    skui_addString(m_cbEffect, L"Buff");
    skui_addString(m_cbEffect, L"Fixed Damage");
    skui_addString(m_cbEffect, L"Other");

    createStatic(230, 177, 60, 14, L"State:", 0, 4);
    skui_createCombo(m_cbState, m_hWnd, 290, 172, 160, 250, 0x6C);
    SkillCombo_fillStateTypes(m_cbState);

    createStatic(20, 207, 80, 14, L"Element:", 0, 4);
    skui_createCombo(m_cbElement, m_hWnd, 100, 202, 110, 350, 0x334);
    SkillCombo_fillElements(m_cbElement);

    skui_createButton(m_chkSuccess, m_hWnd, L" Chance of success", 0x50000002, 235, 202, 110, 18, 0x131);
    skui_subclassCheckBox(m_chkSuccess);
    m_editSuccess.EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)320, 345, 202, 35, 18, 0, 0, 0);
    createStatic(380, 204, 15, 14, L"%", 0, 4);
    if (data.successType == 1)
        m_chkSuccess.setCheck(true);
    else
        m_editSuccess.enable(false);

    createStatic(20, 237, 80, 14, L"Damage Type:", 0, 4);
    skui_createCombo(m_cbDamageType, m_hWnd, 100, 232, 250, 250, 0x6B);
    SkillCombo_fillDamageTypes(m_cbDamageType);

    createStatic(20, 264, 75, 14, L"Spell Delay:", 0, 4);
    m_editSpellDelay.EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)260, 100, 262, 40, 18, 0, 0, 0);
    createStatic(170, 264, 65, 14, L"Reset Time:", 0, 4);
    m_editResetTime.EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)275, 240, 262, 40, 18, 0, 0, 0);
    createStatic(310, 264, 65, 14, L"Keep Time:", 0, 4);
    m_editKeepTime.EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)550, 380, 262, 40, 18, 0, 0, 0);

    createStatic(20, 288, 75, 14, L"Attack Range:", 0, 4);
    m_editAttackRange.EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)520, 100, 286, 40, 18, 0, 0, 0);
    createStatic(170, 288, 65, 14, L"Apply Range:", 0, 4);
    m_editApplyRange.EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)535, 240, 286, 40, 18, 0, 0, 0);
    createStatic(310, 288, 60, 14, L"Multi Attack:", 0, 4);
    m_editMultiAttack.EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)290, 380, 286, 40, 18, 0, 0, 0);

    createStatic(20, 320, 80, 14, L"Damage/Heal:", 0, 5);
    createStatic(50, 338, 20, 14, L"HP:", 0, 4);
    createStatic(50, 362, 20, 14, L"SP:", 0, 4);
    createStatic(50, 386, 20, 14, L"MP:", 0, 4);

    createStatic(100, 318, 60, 14, L"Damages:", 0, 4);
    m_editDamage[0].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)565, 100, 336, 60, 18, data.damage[0], 0, 0);
    m_editDamage[1].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)580, 100, 360, 60, 18, data.damage[1], 0, 0);
    m_editDamage[2].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)595, 100, 384, 60, 18, data.damage[2], 0, 0);

    createStatic(170, 318, 60, 14, L"Add Dmg:", 0, 4);
    m_editAddDamage[0].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)610, 170, 336, 60, 18, data.addDamage[0], 0, 0);
    m_editAddDamage[1].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)625, 170, 360, 60, 18, data.addDamage[1], 0, 0);
    m_editAddDamage[2].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)640, 170, 384, 60, 18, data.addDamage[2], 0, 0);

    createStatic(240, 318, 60, 14, L"Time Dmg:", 0, 4);
    m_editTimeDamage[0].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)655, 240, 336, 60, 18, data.timeDamage[0], 0, 0);
    m_editTimeDamage[1].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)670, 240, 360, 60, 18, data.timeDamage[1], 0, 0);
    m_editTimeDamage[2].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)685, 240, 384, 60, 18, data.timeDamage[2], 0, 0);

    createStatic(310, 318, 60, 14, L"Heal:", 0, 4);
    m_editHeal[0].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)700, 310, 336, 60, 18, data.heal[0], 0, 0);
    m_editHeal[1].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)715, 310, 360, 60, 18, data.heal[1], 0, 0);
    m_editHeal[2].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)730, 310, 384, 60, 18, data.heal[2], 0, 0);

    createStatic(380, 318, 60, 14, L"Time Heal:", 0, 4);
    m_editTimeHeal[0].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)745, 380, 336, 60, 18, data.timeHeal[0], 0, 0);
    m_editTimeHeal[1].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)760, 380, 360, 60, 18, data.timeHeal[1], 0, 0);
    m_editTimeHeal[2].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)775, 380, 384, 60, 18, data.timeHeal[2], 0, 0);

    int x = 20;
    createStatic(20, 408, 80, 14, L"Abilities:", 0, 5);

    wchar_t number[16] = {};
    int count = 3;
    if ((MOBSKILL_DATA(m_skill).version & 0xFFF) >= 0x600)
        count = 10;
    int y = 427;
    UINT id = 350;
    int ability = 0;
    for (;;)
    {
        swprintf_s(number, 16, L"%i:", (int)(id - 349));
        createStatic(x, y + 5, 20, 14, number, 0, 4);

        ComboBox& combo = m_cbAbility[ability];
        skui_createComboEx(combo, m_hWnd, x + 20, y, 135, 350, id);

        skui_clearVector(m_abilityIds);
        for (int n = 0; g_skillStatNames[n]; ++n)
        {
            combo.ComboBox::addItem(g_skillStatNames[n], n);
            m_abilityIds.add((short)n);
        }

        WPARAM type = MOBSKILL_DATA(m_skill).ability[ability].type;
        if (type)
        {
            if (!combo.ComboBox::selectData(type))
            {
                wchar_t text[32];
                swprintf_s(text, 32, L"Unknown %i", (int)type);
                combo.ComboBox::addItem(text, (int)type);
                combo.ComboBox::selectData(type);
            }
        }
        else
        {
            SendMessageW(combo.m_hWnd, CB_SETCURSEL, 0, 0);
        }

        m_editAbilityValue[ability].EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)(id + 10), x + 160, y + 3, 40, 18,
                                               MOBSKILL_DATA(m_skill).ability[ability].value, 0, 0);
        if (id & 1)
        {
            y += 26;
            x = 20;
        }
        else
        {
            x += 220;
        }
        ++ability;
        ++id;
        if (--count == 0)
            break;
    }
    return true;
}

void MobSkillEditor::loadControls()
{
    SkillData& data = MOBSKILL_DATA(m_skill);
    int bound = STRIP_IDS_BOUND(m_icon.m_bitmaps);
    int icon = data.icon;
    int sel = 0;
    if (bound <= 0)
    {
        sel = -1;
    }
    else
    {
        int* ids = STRIP_IDS(m_icon.m_bitmaps);
        while (*ids != icon)
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

    m_editName.setText(MOBSKILL_NAME(m_skill).c_str());
    m_editDescription.setText(MOBSKILL_DESCRIPTION(m_skill).c_str());
    m_cbFamily.ComboBox::selectData((WPARAM)(int)data.typeDetail);
    SendMessageW(m_cbAttackType.m_hWnd, CB_SETCURSEL, data.typeAttack, 0);
    SendMessageW(m_cbEffect.m_hWnd, CB_SETCURSEL, data.typeEffect, 0);
    SendMessageW(m_cbElement.m_hWnd, CB_SETCURSEL, data.attribType, 0);
    m_cbTargets.ComboBox::selectData(MOBSKILL_DATA(m_skill).targetType);
    SendMessageW(m_cbDamageType.m_hWnd, CB_SETCURSEL, data.damageType, 0);
    SendMessageW(m_cbState.m_hWnd, CB_SETCURSEL, data.stateType, 0);
    m_editSpellDelay.EditText::setInt(MOBSKILL_DATA(m_skill).readyTime);
    m_editResetTime.EditText::setInt(MOBSKILL_DATA(m_skill).resetTime);
    m_editSuccess.EditText::setInt(MOBSKILL_DATA(m_skill).successValue);
    m_editAttackRange.EditText::setInt(MOBSKILL_DATA(m_skill).attackRange);
    m_editApplyRange.EditText::setInt(MOBSKILL_DATA(m_skill).applyRange);
    m_editKeepTime.EditText::setInt(MOBSKILL_DATA(m_skill).keepTime);
    m_editMultiAttack.EditText::setInt(MOBSKILL_DATA(m_skill).multiAttack);
}

void MobSkillEditor::saveControls()
{
    {
        String text;
        MOBSKILL_NAME(m_skill) = *m_editName.getText(&text);
    }
    {
        String text;
        MOBSKILL_DESCRIPTION(m_skill) = *m_editDescription.getText(&text);
    }

    int sel = m_icon.m_selected;
    short icon;
    if (sel < 0 || sel >= STRIP_IDS_BOUND(m_icon.m_bitmaps))
        icon = 0;
    else
        icon = (short)STRIP_IDS(m_icon.m_bitmaps)[sel];
    MOBSKILL_DATA(m_skill).icon = icon;

    MOBSKILL_DATA(m_skill).typeDetail = (short)skui_curData(m_cbFamily);
    MOBSKILL_DATA(m_skill).typeAttack = (unsigned char)SendMessageW(m_cbAttackType.m_hWnd, CB_GETCURSEL, 0, 0);
    MOBSKILL_DATA(m_skill).typeEffect = (unsigned char)SendMessageW(m_cbEffect.m_hWnd, CB_GETCURSEL, 0, 0);
    MOBSKILL_DATA(m_skill).attribType = (unsigned char)SendMessageW(m_cbElement.m_hWnd, CB_GETCURSEL, 0, 0);
    unsigned char target;
    if (m_cbTargets.m_hWnd)
        target = (unsigned char)skui_curData(m_cbTargets);
    else
        target = 0;
    MOBSKILL_DATA(m_skill).targetType = target;
    MOBSKILL_DATA(m_skill).damageType = (unsigned char)SendMessageW(m_cbDamageType.m_hWnd, CB_GETCURSEL, 0, 0);
    MOBSKILL_DATA(m_skill).stateType = (unsigned char)SendMessageW(m_cbState.m_hWnd, CB_GETCURSEL, 0, 0);
    MOBSKILL_DATA(m_skill).readyTime = (unsigned char)m_editSpellDelay.EditText::getInt(0);
    MOBSKILL_DATA(m_skill).resetTime = (short)m_editResetTime.EditText::getInt(0);
    MOBSKILL_DATA(m_skill).attackRange = (unsigned char)m_editAttackRange.EditText::getInt(0);
    MOBSKILL_DATA(m_skill).applyRange = (unsigned char)m_editApplyRange.EditText::getInt(0);
    MOBSKILL_DATA(m_skill).keepTime = (unsigned short)m_editKeepTime.EditText::getInt(0);
    MOBSKILL_DATA(m_skill).multiAttack = (unsigned char)m_editMultiAttack.EditText::getInt(0);
    if (m_chkSuccess.isChecked())
    {
        MOBSKILL_DATA(m_skill).successType = 1;
        MOBSKILL_DATA(m_skill).successValue = (unsigned char)m_editSuccess.EditText::getInt(0);
    }
    else
    {
        MOBSKILL_DATA(m_skill).successType = 0;
        MOBSKILL_DATA(m_skill).successValue = 0;
    }

    for (int k = 0; k < 3; ++k)
    {
        MOBSKILL_DATA(m_skill).ability[k].type = 0;
        MOBSKILL_DATA(m_skill).ability[k].value = 0;
        unsigned char type;
        if (m_cbAbility[k].m_hWnd)
            type = (unsigned char)skui_curData(m_cbAbility[k]);
        else
            type = 0;
        short value = (short)m_editAbilityValue[k].EditText::getInt(0);
        if (type)
        {
            MOBSKILL_DATA(m_skill).ability[k].type = type;
            MOBSKILL_DATA(m_skill).ability[k].value = value;
        }
    }

    MOBSKILL_DATA(m_skill).damage[0] = (short)m_editDamage[0].EditText::getInt(0);
    MOBSKILL_DATA(m_skill).damage[1] = (short)m_editDamage[1].EditText::getInt(0);
    MOBSKILL_DATA(m_skill).damage[2] = (short)m_editDamage[2].EditText::getInt(0);
    MOBSKILL_DATA(m_skill).addDamage[0] = (short)m_editAddDamage[0].EditText::getInt(0);
    MOBSKILL_DATA(m_skill).addDamage[1] = (short)m_editAddDamage[1].EditText::getInt(0);
    MOBSKILL_DATA(m_skill).addDamage[2] = (short)m_editAddDamage[2].EditText::getInt(0);
    MOBSKILL_DATA(m_skill).timeDamage[0] = (short)m_editTimeDamage[0].EditText::getInt(0);
    MOBSKILL_DATA(m_skill).timeDamage[1] = (short)m_editTimeDamage[1].EditText::getInt(0);
    MOBSKILL_DATA(m_skill).timeDamage[2] = (short)m_editTimeDamage[2].EditText::getInt(0);
    MOBSKILL_DATA(m_skill).heal[0] = (unsigned short)m_editHeal[0].EditText::getInt(0);
    MOBSKILL_DATA(m_skill).heal[1] = (unsigned short)m_editHeal[1].EditText::getInt(0);
    MOBSKILL_DATA(m_skill).heal[2] = (unsigned short)m_editHeal[2].EditText::getInt(0);
    MOBSKILL_DATA(m_skill).timeHeal[0] = (unsigned short)m_editTimeHeal[0].EditText::getInt(0);
    MOBSKILL_DATA(m_skill).timeHeal[1] = (unsigned short)m_editTimeHeal[1].EditText::getInt(0);
    MOBSKILL_DATA(m_skill).timeHeal[2] = (unsigned short)m_editTimeHeal[2].EditText::getInt(0);
}

bool MobSkillEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (!code && id == 305)
    {
        if (ctrl == m_chkSuccess.m_hWnd)
        {
            bool checked = m_chkSuccess.isChecked() == 0;
            m_chkSuccess.setCheck(checked);
            m_editSuccess.enable(checked);
            return true;
        }
        return true;
    }
    return Modal::onCommand(code, ctrl, id);
}
