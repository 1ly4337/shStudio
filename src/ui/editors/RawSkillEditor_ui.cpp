#include "RawSkillEditor.h"
#include "ToonSkill.h"
#include "SkillData.h"
#include "ClientData.h"
#include "ctlhelpers_g1a.h"
#include "SkillCombos.h"
#include "CtrlHelpers.h"
#include "gdi_cache.h"
#include <cwchar>
#include "_imports_p09.h"
#include "_imports_g1a.h"

bool RawSkillEditor::createEditor(HWND parent)
{
    ToonSkill* skill = m_skill;
    m_compact = false;
    int height = 630;
    int width;
    if (skill->m_levels[0].typeShow)
    {
        GetKeyState(VK_LCONTROL);
        if (GetSystemMetrics(SM_CXFULLSCREEN) < 1900)
        {
            m_compact = true;
            width = 840;
            height = 670;
        }
        else
        {
            width = 1880;
        }
    }
    else
    {
        width = 1280;
    }
    if ((m_skill->m_format & 0xFFF) == 0x400)
        height -= 130;
    if ((m_skill->m_format & 0xFFF) >= 0x600)
        height += 160;

    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, width, height);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Toon Skill Editor");
    if (result)
    {
        m_owner = Modal_getTopLevelWindow(parent);
        Window_centerOn(m_hWnd, m_owner);
        show(true);
        return true;
    }
    return result;
}

bool RawSkillEditor::onCreate()
{
    SkillData* first = &m_skill->m_levels[0];
    bool isNoos = m_skill->m_levels[0].typeShow == 0;
    if (!m_skill->m_levels[0].typeShow)
        setText(L"Noos Editor");

    ImageStrip* icons = reinterpret_cast<ClientData*>(reinterpret_cast<BYTE*>(((void*)g_studioFrame)) + 0x230)->getIcons(L"icon_skills");
    if (icons)
    {
        m_icon.m_bitmaps = *icons;
        m_icon.m_cells = *icons;
        m_icon.m_columns = 24;
        m_icon.create(m_hWnd, (HMENU)101, 0, 21, 34, 34, 32, 0);
    }

    createStatic(90, 18, 40, 14, L"Name:", 0, 4);
    m_name.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, 140, 16, 280, 18, m_hWnd,
                                        (HMENU)0x67, 0, 0);
    if (m_name.m_hWnd)
    {
        m_name.setFontId(m_name.m_fontId);
        SetWindowSubclass(m_name.m_hWnd, tabSubclassProc, 0x65646974 , (DWORD_PTR)&m_name);
    }
    m_name.setFont(getStdFont(6));
    SendMessageW(m_name.m_hWnd, EM_SETLIMITTEXT, 26, 0);

    createStatic(90, 45, 40, 14, L"Family:", 0, 4);
    g1a_createComboEx(m_family, m_hWnd, 140, 40, 280, 250, 0x71);
    SkillCombo_fillFamilies(m_family);
    short family = first->typeDetail;
    m_family.selectData(family);

    createStatic(80, 72, 50, 18, L"Category:", 0, 4);
    g1a_createCombo(m_category, m_hWnd, 140, 68, 100, 150, 0x6E);
    m_category.addString(L"Noos - Other");
    m_category.addString(L"Passive");
    m_category.addString(L"Basic");
    m_category.addString(L"Combat");
    m_category.addString(L"Special");

    createStatic(260, 72, 50, 18, L"Animation:", 0, 4);
    m_animation.createNumber(m_hWnd, (HMENU)0x66, 320, 70, 50, 18, 0, 0, 0);

    createStatic(450, 20, 40, 24, L"Type:", 0, 4);
    g1a_createCombo(m_type, m_hWnd, 490, 16, 120, 150, 0x6F);
    m_type.addString(L"Passive");
    m_type.addString(L"Physical Attack");
    m_type.addString(L"Shooting Attack");
    m_type.addString(L"Magic Attack");

    createStatic(450, 44, 40, 24, L"Effect:", 0, 4);
    g1a_createCombo(m_effect, m_hWnd, 490, 40, 120, 150, 0x70);
    m_effect.addString(L"Buff, Noss, \u2026");
    m_effect.addString(L"Basic Damage");
    m_effect.addString(L"Healing, Dispel");
    m_effect.addString(L"Debuff");
    m_effect.addString(L"Buff");
    m_effect.addString(L"Fixed Damage");
    m_effect.addString(L"Other");

    createStatic(20, 94, 100, 16, L"Description:", 0, 4);
    m_description.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50211044, 20, 110, 320, 48, m_hWnd,
                                               (HMENU)0x68, 0, 0);
    if (m_description.m_hWnd)
    {
        m_description.setFontId(m_description.m_fontId);
        SetWindowSubclass(m_description.m_hWnd, tabSubclassProc, 0x65646974 ,
                          (DWORD_PTR)&m_description);
    }
    m_description.setFont(getStdFont(6));
    SendMessageW(m_description.m_hWnd, EM_SETLIMITTEXT, 0x200, 0);
    m_description.enable(false);
    g1a_createButton(m_editDescription, m_hWnd, L"Edit...", 0x50000000, 360, 122, 60, 24, 0x36B);

    createStatic(20, 172, 80, 18, L"Skill used by:", 0, 5);
    g1a_createButton(m_classes[0], m_hWnd, L"Fighter", 0x50010002, 35, 186, 60, 18, 0x78);
    g1a_createButton(m_classes[1], m_hWnd, L"Defender", 0x50000002, 100, 186, 60, 18, 0x79);
    g1a_subclassCheckBox(m_classes[1]);
    g1a_createButton(m_classes[2], m_hWnd, L"Ranger", 0x50000002, 165, 186, 60, 18, 0x7A);
    g1a_subclassCheckBox(m_classes[2]);
    g1a_createButton(m_classes[3], m_hWnd, L"Archer", 0x50000002, 230, 186, 60, 18, 0x7B);
    g1a_subclassCheckBox(m_classes[3]);
    g1a_createButton(m_classes[4], m_hWnd, L"Mage", 0x50000002, 295, 186, 60, 18, 0x7C);
    g1a_subclassCheckBox(m_classes[4]);
    g1a_createButton(m_classes[5], m_hWnd, L"Priest", 0x50000002, 360, 186, 60, 18, 0x7D);
    g1a_subclassCheckBox(m_classes[5]);

    createStatic(35, 214, 40, 18, L"from:", 0, 4);
    g1a_createCombo(m_faction, m_hWnd, 70, 210, 130, 200, 0x69);
    m_faction.addString(L"Human");
    m_faction.addString(L"Elf");
    m_faction.addString(L"Light");
    m_faction.addString(L"Deatheater");
    m_faction.addString(L"Vail");
    m_faction.addString(L"Fury");
    m_faction.addString(L"All factions");

    createStatic(210, 214, 45, 18, L"in mode:", 0, 4);
    g1a_createCombo(m_mode, m_hWnd, 255, 210, 120, 150, 0x6A);
    m_mode.addString(L"Easy or higher");
    m_mode.addString(L"Normal or higher");
    m_mode.addString(L"Difficult or higher");
    m_mode.addString(L"Ultimate");

    createStatic(450, 80, 150, 24, L"Skill usable with:", 0, 5);
    g1a_createButton(m_weapons[0], m_hWnd, g_weaponTypeNames[0], 0x50010002, 450, 96, 100, 18, 0x82);
    g1a_createButton(m_weapons[1], m_hWnd, g_weaponTypeNames[1], 0x50000002, 450, 114, 100, 18, 0x83);
    g1a_subclassCheckBox(m_weapons[1]);
    g1a_createButton(m_weapons[2], m_hWnd, g_weaponTypeNames[2], 0x50000002, 450, 132, 100, 18, 0x84);
    g1a_subclassCheckBox(m_weapons[2]);
    g1a_createButton(m_weapons[3], m_hWnd, g_weaponTypeNames[3], 0x50000002, 450, 150, 100, 18, 0x85);
    g1a_subclassCheckBox(m_weapons[3]);
    g1a_createButton(m_weapons[4], m_hWnd, g_weaponTypeNames[4], 0x50000002, 450, 168, 100, 18, 0x86);
    g1a_subclassCheckBox(m_weapons[4]);
    g1a_createButton(m_weapons[5], m_hWnd, g_weaponTypeNames[5], 0x50010002, 570, 96, 100, 18, 0x87);
    g1a_createButton(m_weapons[6], m_hWnd, g_weaponTypeNames[6], 0x50000002, 570, 114, 100, 18, 0x88);
    g1a_subclassCheckBox(m_weapons[6]);
    g1a_createButton(m_weapons[7], m_hWnd, g_weaponTypeNames[7], 0x50000002, 570, 132, 100, 18, 0x89);
    g1a_subclassCheckBox(m_weapons[7]);
    g1a_createButton(m_weapons[8], m_hWnd, g_weaponTypeNames[8], 0x50000002, 570, 150, 100, 18, 0x8A);
    g1a_subclassCheckBox(m_weapons[8]);
    g1a_createButton(m_weapons[9], m_hWnd, g_weaponTypeNames[9], 0x50000002, 570, 168, 100, 18, 0x8B);
    g1a_subclassCheckBox(m_weapons[9]);
    g1a_createButton(m_weapons[10], m_hWnd, g_weaponTypeNames[10], 0x50010002, 690, 96, 100, 18, 0x8C);
    g1a_createButton(m_weapons[11], m_hWnd, g_weaponTypeNames[11], 0x50000002, 690, 114, 100, 18, 0x8D);
    g1a_subclassCheckBox(m_weapons[11]);
    g1a_createButton(m_weapons[12], m_hWnd, g_weaponTypeNames[12], 0x50000002, 690, 132, 100, 18, 0x8E);
    g1a_subclassCheckBox(m_weapons[12]);
    g1a_createButton(m_weapons[13], m_hWnd, g_weaponTypeNames[13], 0x50000002, 690, 150, 100, 18, 0x8F);
    g1a_subclassCheckBox(m_weapons[13]);
    g1a_createButton(m_weapons[14], m_hWnd, g_weaponTypeNames[14], 0x50000002, 690, 168, 100, 18, 0x90);
    g1a_subclassCheckBox(m_weapons[14]);
    g1a_createButton(m_weapons[15], m_hWnd, g_weaponTypeNames[15], 0x50010002, 450, 186, 100, 18, 0x91);

    createStatic(450, 218, 50, 18, L"Requires:", 0, 4);
    g1a_createCombo(m_requiredWeapon1, m_hWnd, 510, 214, 130, 150, 0x72);
    createStatic(650, 218, 20, 18, L"or", 0, 4);
    g1a_createCombo(m_requiredWeapon2, m_hWnd, 670, 214, 130, 150, 0x73);
    m_requiredWeapon1.addString(L"None");
    m_requiredWeapon2.addString(L"None");
    for (const wchar_t* const* name = g_weaponTypeNames; name < g_weaponTypeNames + 16; ++name)
    {
        m_requiredWeapon1.addString(*name);
        m_requiredWeapon2.addString(*name);
    }
    WPARAM weapon = first->weapon[0];
    if (weapon == 19)
        weapon = 16;
    SendMessageW(m_requiredWeapon1.m_hWnd, CB_SETCURSEL, weapon, 0);
    weapon = first->weapon[1];
    if (weapon == 19)
        weapon = 16;
    SendMessageW(m_requiredWeapon2.m_hWnd, CB_SETCURSEL, weapon, 0);

    createStatic(20, 249, 60, 14, L"Def. Type:", 0, 4);
    g1a_createCombo(m_defenceType, m_hWnd, 80, 246, 150, 250, 0x6D);
    m_defenceType.addString(L"none");
    m_defenceType.addString(L"Melee Attacks");
    m_defenceType.addString(L"Ranged Attacks");
    m_defenceType.addString(L"Magical Attacks");
    SendMessageW(m_defenceType.m_hWnd, CB_SETCURSEL, first->defenceType, 0);

    createStatic(240, 249, 60, 14, L"State Type:", 0, 4);
    g1a_createCombo(m_stateType, m_hWnd, 300, 246, 150, 250, 0x6C);
    SkillCombo_fillStateTypes(m_stateType);
    SendMessageW(m_stateType.m_hWnd, CB_SETCURSEL, first->stateType, 0);

    createStatic(460, 249, 50, 14, L"Damage:", 0, 4);
    g1a_createCombo(m_damageType, m_hWnd, 510, 246, 250, 250, 0x6B);
    SkillCombo_fillDamageTypes(m_damageType);
    SendMessageW(m_damageType.m_hWnd, CB_SETCURSEL, first->damageType, 0);

    int abilityCount = 3;
    if ((m_skill->m_format & 0xFFF) >= 0x600)
        abilityCount = 9;
    int rows = abilityCount / 3;
    int ability = 0;
    int abilityY = 278;
    if (rows > 0)
    {
        do
        {
            int x = 20;
            int column = 0;
            ComboBox* combo = &m_statTypes[ability];
            do
            {
                wchar_t buffer[16] = {};
                int next = ability + 1;
                swprintf_s(buffer, 16, L"Ability %d:", next);
                createStatic(x, abilityY + 6, 50, 14, buffer, 0, 4);
                int offset = 60;
                if (column == 2)
                    offset = 50;
                int comboX = offset + x;
                g1a_createComboEx(*combo, m_hWnd, comboX, abilityY, 150, 350, ability + 350);

                for (int i = 0; i < m_statIds.m_capacity; ++i)
                    m_statIds.resetItem(&m_statIds.m_items[i]);
                m_statIds.m_count = 0;
                const wchar_t* name = g_skillStatNames[0];
                int index = 0;
                if (name)
                {
                    do
                    {
                        combo->addItem(name, index);
                        m_statIds.add((short)index);
                        name = g_skillStatNames[++index];
                    } while (name);
                }
                combo->resize(150, 350);
                SkillCombo_selectStat(*combo, m_skill->m_levels, m_skill->m_levelCount, ability);
                x = comboX + 160;
                ability = next;
                ++combo;
                ++column;
            } while (column < 3);
            abilityY += 28;
        } while (--rows);
    }

    int headerY = abilityY + 24;
    int numberY = headerY + (m_compact ? 33 : 17) + 3;
    if (m_skill->m_levelCount > 0)
    {
        int n = 0;
        do
        {
            ++n;
            wchar_t number[4] = {};
            swprintf_s(number, 4, L"%d", n);
            createStatic(20, numberY, 20, 18, number, 0, 1);
            numberY += 23;
        } while (n < m_skill->m_levelCount);
    }

    int x0;
    int extraHeight;
    int tableY = headerY;
    if (m_compact)
    {
        int contentHeight = 225;
        if ((m_skill->m_format & 0xFFF) == 0x400)
            contentHeight = 95;
        if ((m_skill->m_format & 0xFFF) >= 0x600)
            contentHeight += 140;
        RECT rc;
        rc.top = headerY;
        rc.bottom = contentHeight + headerY + 17;
        rc.left = 40;
        rc.right = 800;
        AdjustWindowRectEx(&rc, WS_HSCROLL, FALSE, 0);
        rc.bottom += GetSystemMetrics(SM_CYVSCROLL);
        RECT rect;
        CopyRect(&rect, &rc);
        if (m_scroller.Window::create(m_hWnd, (HMENU)0x3E9, &rect, 0x50100000, 0, 0))
            m_scroller.updateScrollBars();
        m_scroller.m_contentHeight = contentHeight;
        m_scroller.m_contentWidth = 1815;
        m_scroller.updateScrollBars();
        m_scroller.invalidate(true);
        x0 = 5;
        extraHeight = headerY + 24;
        tableY = 16;
    }
    else
    {
        x0 = 20;
        extraHeight = 0;
    }
    Window* panel = m_compact ? static_cast<Window*>(&m_scroller) : static_cast<Window*>(this);

    if (!m_compact)
    {
        createStatic(x0, tableY, 20, 18, L"#", 0, 4);
        x0 += 20;
    }
    int x = x0;
    panel->createStatic(x, tableY, 30, 18, L"Lvl", 0, 4);
    x += 35;
    panel->createStatic(x, tableY, 30, 14, L"Pts", 0, 4);
    x += 30;
    panel->createStatic(x, tableY, 45, 14, L"Cost SP", 0, 4);
    x += 45;
    panel->createStatic(x, tableY, 45, 14, L"Cost MP", 0, 4);
    x += 50;
    panel->createStatic(x, tableY, 50, 14, L"Cast", 0, 4);
    x += 35;
    panel->createStatic(x, tableY, 40, 14, L"Reset", 0, 4);
    x += 45;
    if (!isNoos)
    {
        panel->createStatic(x, tableY, 40, 14, L"Hits", 0, 4);
        x += 30;
    }
    panel->createStatic(x, tableY, 50, 14, L"Chance", 0, 4);
    x += 50;
    if (!isNoos)
    {
        panel->createStatic(x, tableY, 40, 14, L"Weapon", 0, 4);
        x += 50;
    }
    if (family == 83)
    {
        panel->createStatic(x, tableY, 40, 14, L"Phys", 0, 4);
        x += 45;
        panel->createStatic(x, tableY, 40, 14, L"Dist", 0, 4);
        x += 40;
        panel->createStatic(x, tableY, 40, 14, L"Magic", 0, 4);
        x += 40;
    }
    else
    {
        panel->createStatic(x, tableY, 40, 14, L"Abt.1", 0, 4);
        x += 45;
        int n = 1;
        do
        {
            wchar_t label[16] = {};
            ++n;
            swprintf_s(label, 16, L"Abt.%d", n);
            panel->createStatic(x, tableY, 40, 14, label, 0, 4);
            x += 40;
        } while (n < abilityCount);
    }
    if (!isNoos)
    {
        panel->createStatic(x, tableY, 40, 14, L"Element", 0, 4);
        x += 70;
    }
    panel->createStatic(x, tableY, 50, 14, L"Target", 0, 4);
    x += 125;
    panel->createStatic(x, tableY, 40, 14, L"Dist.", 0, 4);
    x += 35;
    panel->createStatic(x, tableY, 40, 14, L"Range", 0, 4);
    x += 35;
    panel->createStatic(x, tableY - 16, 40, 28, L"Keep Time", SS_CENTER, 4);
    x += 45;
    if (!isNoos)
    {
        panel->createStatic(x, tableY - 16, 130, 14, L"Base Damage", SS_CENTER, 4);
        panel->createStatic(x, tableY, 40, 14, L"HP", SS_CENTER, 4);
        x += 45;
        panel->createStatic(x, tableY, 40, 14, L"SP", SS_CENTER, 4);
        x += 45;
        panel->createStatic(x, tableY, 40, 14, L"MP", SS_CENTER, 4);
        x += 45;
        panel->createStatic(x, tableY - 16, 130, 14, L"Additional Damage", SS_CENTER, 4);
        panel->createStatic(x, tableY, 40, 14, L"HP", SS_CENTER, 4);
        x += 45;
        panel->createStatic(x, tableY, 40, 14, L"SP", SS_CENTER, 4);
        x += 45;
        panel->createStatic(x, tableY, 40, 14, L"MP", SS_CENTER, 4);
        x += 45;
        panel->createStatic(x, tableY - 16, 130, 14, L"Damage over time", SS_CENTER, 4);
        panel->createStatic(x, tableY, 40, 14, L"HP", SS_CENTER, 4);
        x += 45;
        panel->createStatic(x, tableY, 40, 14, L"SP", SS_CENTER, 4);
        x += 45;
        panel->createStatic(x, tableY, 40, 14, L"MP", SS_CENTER, 4);
        x += 45;
    }
    panel->createStatic(x, tableY - 16, 130, 14, L"Heal", SS_CENTER, 4);
    panel->createStatic(x, tableY, 40, 14, L"HP", SS_CENTER, 4);
    x += 45;
    panel->createStatic(x, tableY, 40, 14, L"SP", SS_CENTER, 4);
    x += 45;
    panel->createStatic(x, tableY, 40, 14, L"MP", SS_CENTER, 4);
    x += 45;
    panel->createStatic(x, tableY - 16, 130, 14, L"Heal over time", SS_CENTER, 4);
    panel->createStatic(x, tableY, 40, 14, L"HP", SS_CENTER, 4);
    x += 45;
    panel->createStatic(x, tableY, 40, 14, L"SP", SS_CENTER, 4);
    x += 45;
    panel->createStatic(x, tableY, 40, 14, L"MP", SS_CENTER, 4);
    x += 45;
    if (isNoos)
    {
        panel->createStatic(x, tableY - 16, 40, 28, L"Fix Range", SS_CENTER, 4);
    }
    else
    {
        panel->createStatic(x, tableY - 16, 40, 28, L"Defence Value", SS_CENTER, 4);
        panel->createStatic(x + 45, tableY, 40, 28, L"Aggro", SS_CENTER, 4);
    }

    int rowY = tableY + 18;
    if (m_skill->m_levelCount > 0)
    {
        int comboY = rowY - 2;
        int id = 215;
        int abilityId = 375;
        int level = 0;
        do
        {
            SkillData* lvl = &m_skill->m_levels[level];
            int col = 35;
            if (m_compact)
                col = 0;
            m_reqLevel[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id - 15), col, rowY, 30, 18, lvl->reqLevel, 0, 0);
            col += 35;
            m_points[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)id, col, rowY, 30, 18, lvl->points, 0, 0);
            col += 35;
            m_costSP[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 15), col, rowY, 40, 18, lvl->sp, 0, 0);
            col += 45;
            m_costMP[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 30), col, rowY, 40, 18, lvl->mp, 0, 0);
            col += 45;
            m_castTime[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 45), col, rowY, 30, 18, lvl->readyTime, 0, 0);
            col += 35;
            m_resetTime[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 60), col, rowY, 40, 18, lvl->resetTime, 0, 0);
            col += 45;
            if (!isNoos)
            {
                m_hits[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 75), col, rowY, 25, 18,
                                                    lvl->multiAttack, 0, 0);
                col += 30;
            }
            g1a_createButton(m_chanceEnabled[level], panel->m_hWnd, 0, 0x50000002, col, rowY, 20, 18, id + 90);
            g1a_subclassCheckBox(m_chanceEnabled[level]);
            col += 20;
            m_chance[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 105), col, rowY, 30, 18, lvl->successValue, 0, 0);
            col += 35;
            if (!isNoos)
            {
                m_weapon[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 120), col, rowY, 40, 18,
                                                    lvl->weaponValue, 0, 0);
                col += 45;
            }
            m_statValues[level][0].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 145), col, rowY, 40, 18,
                                                      lvl->ability[0].value, 0, 0);
            col += 45;
            if (family == 83)
            {
                m_statValues[level][1].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 160), col, rowY, 35, 18,
                                                              lvl->ability[1].value, 0, 0);
                col += 40;
                m_statValues[level][2].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 175), col, rowY, 35, 18,
                                                              lvl->ability[2].value, 0, 0);
                col += 40;
            }
            else
            {
                int menuId = abilityId;
                int k = 1;
                int remaining = abilityCount - 1;
                do
                {
                    m_statValues[level][k].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)menuId, col, rowY, 35, 18,
                                                                  lvl->ability[k].value, 0, 0);
                    menuId += 15;
                    col += 40;
                    ++k;
                } while (--remaining);
            }
            if (!isNoos)
            {
                g1a_createCombo(m_element[level], panel->m_hWnd, col, comboY, 65, 350, id + 605);
                col += 70;
            }
            g1a_createCombo(m_target[level], panel->m_hWnd, col, comboY, 120, 100, id + 620);
            col += 125;
            SkillCombo_fillToonTargets(m_target[level], (unsigned short)(family - 41) > 2u);
            SendMessageW(m_target[level].m_hWnd, CB_SETCURSEL, (WPARAM)(lvl->targetType - 1), 0);
            m_distance[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 305), col, rowY, 30, 18, lvl->attackRange, 0, 0);
            col += 35;
            m_range[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 320), col, rowY, 30, 18, lvl->applyRange, 0, 0);
            col += 35;
            m_keepTime[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 335), col, rowY, 40, 18, lvl->keepTime, 0, 0);
            col += 45;
            if (!isNoos)
            {
                m_baseDmgHP[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 350), col, rowY, 40, 18, lvl->damage[0], 0, 0);
                col += 45;
                m_baseDmgSP[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 365), col, rowY, 40, 18, lvl->damage[1], 0, 0);
                col += 45;
                m_baseDmgMP[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 380), col, rowY, 40, 18, lvl->damage[2], 0, 0);
                col += 45;
                m_addDmgHP[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 395), col, rowY, 40, 18, lvl->addDamage[0], 0, 0);
                col += 45;
                m_addDmgSP[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 410), col, rowY, 40, 18, lvl->addDamage[1], 0, 0);
                col += 45;
                m_addDmgMP[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 425), col, rowY, 40, 18, lvl->addDamage[2], 0, 0);
                col += 45;
                m_dotHP[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 440), col, rowY, 40, 18, lvl->timeDamage[0], 0, 0);
                col += 45;
                m_dotSP[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 455), col, rowY, 40, 18, lvl->timeDamage[1], 0, 0);
                col += 45;
                m_dotMP[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 470), col, rowY, 40, 18, lvl->timeDamage[2], 0, 0);
                col += 45;
            }
            m_healHP[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 485), col, rowY, 40, 18, lvl->heal[0], 0, 0);
            col += 45;
            m_healSP[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 500), col, rowY, 40, 18, lvl->heal[1], 0, 0);
            col += 45;
            m_healMP[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 515), col, rowY, 40, 18, lvl->heal[2], 0, 0);
            col += 45;
            m_hotHP[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 530), col, rowY, 40, 18, lvl->timeHeal[0], 0, 0);
            col += 45;
            m_hotSP[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 545), col, rowY, 40, 18, lvl->timeHeal[1], 0, 0);
            col += 45;
            m_hotMP[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 560), col, rowY, 40, 18, lvl->timeHeal[2], 0, 0);
            col += 45;
            if (isNoos)
            {
                m_fixRange[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 675), col, rowY, 40, 18, lvl->fixRange, 0, 0);
            }
            else
            {
                m_defence[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 575), col, rowY, 40, 18,
                                                     lvl->defenceValue, 0, 0);
                m_aggro[level].createNumber(panel->m_hWnd, (HMENU)(UINT_PTR)(id + 635), col + 45, rowY, 40, 18,
                                              lvl->arrow, 0, 0);
            }
            comboY += 23;
            abilityId += 2;
            ++level;
            rowY += 23;
        } while (id++ - 214 < m_skill->m_levelCount);
    }
    int bottom = rowY + 16;

    for (int targetId = 835; targetId < m_skill->m_levelCount + 835; ++targetId)
        onCommand(1, 0, targetId);

    if (m_skill->m_levelCount > 0)
    {
        int level = 0;
        do
        {
            SkillData* lvl = &m_skill->m_levels[level];
            bool chanceEnabled = lvl->successType == 1;
            m_chanceEnabled[level].setCheck(chanceEnabled);
            m_chance[level].enable(chanceEnabled);
            SkillCombo_fillElements(m_element[level]);
            SendMessageW(m_element[level].m_hWnd, CB_SETCURSEL, lvl->attribType, 0);
            ++level;
        } while (level < m_skill->m_levelCount);
    }

    SIZE size;
    getSize(&size);
    resize(size.cx, extraHeight + bottom + 70);
    loadControls();
    return Modal::onCreate();
}
