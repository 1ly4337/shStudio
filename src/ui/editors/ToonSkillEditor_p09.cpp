#include "ToonSkillEditor.h"
#include "ToonSkill.h"
#include "SkillData.h"
#include "ctlhelpers_g1a.h"
#include "SkillCombos.h"
#include "CtrlHelpers.h"
#include "gdi_cache.h"
#include <cwchar>
#include "_imports_p09.h"
#include "_imports_g1a.h"

SIZE ToonSkillEditor::createEnhanceControls(int y)
{
    SkillData* levels = m_skill->m_levels;
    int width = 450;

    createStatic(30, y, 200, 14, L"Enhanced Abilities:", 0, 1);

    int statCount = 3;
    if ((m_skill->m_format & 0xFFF) >= 0x600)
        statCount = 10;
    int headerY = createStatRows(y + 18, &width, statCount);

    wchar_t buffer[16] = {};
    createStatic(30, headerY, 20, 14, L"#", 0, 4);
    createStatic(55, headerY, 25, 14, L"Lvl", 0, 4);
    createStatic(90, headerY, 25, 14, L"Pts", 0, 4);
    int x = 125;
    int n = 0;
    do
    {
        ++n;
        swprintf_s(buffer, 16, L"Stat. %d", n);
        createStatic(x, headerY, 50, 14, buffer, 0, 4);
        x += 60;
    } while (n < statCount);

    int maxX = width;
    if (width < x)
        maxX = x;

    int rowY = headerY + 18;
    if (m_skill->m_levelCount > 0)
    {
        int id = 200;
        int level = 0;
        do
        {
            SkillData* lvl = &levels[level];
            swprintf_s(buffer, 16, L"%d", id - 199);
            createStatic(30, rowY + 2, 25, 14, buffer, 0, 1);
            m_reqLevel[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)id, 50, rowY, 30, 18, lvl->reqLevel, 0, 0);
            m_points[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 15), 85, rowY, 30, 18, lvl->points, 0, 0);
            int statX = 120;
            int statId = id + 160;
            int k = 0;
            int remaining = statCount;
            do
            {
                m_statValues[level][k].createNumber(m_hWnd, (HMENU)(UINT_PTR)statId, statX, rowY, 50, 18,
                                                      lvl->ability[k].value, 0, 0);
                statX += 60;
                statId += 15;
                ++k;
            } while (--remaining);
            ++level;
            ++id;
            rowY += 23;
        } while (id - 200 < m_skill->m_levelCount);
    }

    SIZE size;
    size.cx = maxX + 10;
    size.cy = rowY + 16;
    return size;
}

SIZE ToonSkillEditor::createLevelDescriptionControls(int y)
{
    SkillData* levels = m_skill->m_levels;

    createStatic(20, y, 20, 18, L"#", 0, 4);
    createStatic(40, y, 25, 18, L"Lvl", 0, 4);
    createStatic(75, y, 25, 18, L"Pts", 0, 4);
    createStatic(105, y, 50, 18, L"Desc", 0, 4);

    int rowY = y + 18;
    wchar_t buffer[16] = {};
    int id = 200;
    int level = 0;
    int remaining = 3;
    do
    {
        SkillData* lvl = &levels[level];
        swprintf_s(buffer, 16, L"%d", id - 199);
        createStatic(20, rowY + 3, 20, 18, buffer, 0, 1);
        m_reqLevel[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)id, 35, rowY, 25, 18, lvl->reqLevel, 0, 0);
        m_points[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 15), 70, rowY, 25, 18, lvl->points, 0, 0);

        EditText& desc = m_levelDescriptions[level];
        desc.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50211044, 105, rowY, 330, 32, m_hWnd,
                                      (HMENU)(UINT_PTR)(id - 84), 0, 0);
        if (desc.m_hWnd)
        {
            desc.setFontId(desc.m_fontId);
            SetWindowSubclass(desc.m_hWnd, tabSubclassProc, 0x65646974 , (DWORD_PTR)&desc);
        }
        desc.setFont(getStdFont(6));
        desc.setText(m_skill->m_levelTexts[level].c_str());

        rowY += 38;
        ++id;
        ++level;
    } while (--remaining);

    SIZE size;
    size.cy = rowY + 24;
    size.cx = 450;
    return size;
}

SIZE ToonSkillEditor::createHpLimitControls(int y)
{
    SkillData* levels = m_skill->m_levels;
    int statCount = 3;
    if ((m_skill->m_format & 0xFFF) >= 0x600)
        statCount = 10;
    int width = 450;
    int headerY = createStatRows(y, &width, statCount);

    createStatic(45, headerY, 20, 14, L"#", 0, 4);
    createStatic(60, headerY, 25, 14, L"Lvl", 0, 4);
    createStatic(90, headerY, 25, 14, L"Pts", 0, 4);

    wchar_t buffer[16] = {};
    int x = 115;
    int n = 0;
    do
    {
        ++n;
        swprintf_s(buffer, 16, L"Stat. %d", n);
        createStatic(x, headerY, 50, 14, buffer, SS_CENTER, 4);
        x += 55;
    } while (n < statCount);
    createStatic(x, headerY, 50, 14, L"HP limit", SS_CENTER, 4);

    int maxX = width;
    if (width < x + 55)
        maxX = x + 55;

    int rowY = headerY + 18;
    if (m_skill->m_levelCount > 0)
    {
        int id = 200;
        int level = 0;
        do
        {
            SkillData* lvl = &levels[level];
            swprintf_s(buffer, 8, L"%d", id - 199);
            createStatic(45, rowY + 3, 20, 14, buffer, 0, 1);
            m_reqLevel[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)id, 60, rowY, 25, 18, lvl->reqLevel, 0, 0);
            m_points[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 15), 90, rowY, 25, 18, lvl->points, 0, 0);
            int statX = 120;
            int statId = id + 160;
            int k = 0;
            int remaining = statCount;
            do
            {
                m_statValues[level][k].createNumber(m_hWnd, (HMENU)(UINT_PTR)statId, statX, rowY, 50, 18,
                                                      lvl->ability[k].value, 0, 0);
                statX += 55;
                statId += 15;
                ++k;
            } while (--remaining);
            m_hpLimit[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 505), statX, rowY, 40, 18,
                                            lvl->limitHP, 0, 0);
            ++level;
            ++id;
            rowY += 23;
        } while (id - 200 < m_skill->m_levelCount);
    }

    SIZE size;
    size.cx = maxX;
    size.cy = rowY + 16;
    return size;
}

SIZE ToonSkillEditor::createAttackControls(int y, short type)
{
    SkillData* levels = m_skill->m_levels;
    BYTE effect = levels[0].typeEffect;
    int width = 20;
    bool hasKeepTime = false;
    if (effect == 3 || effect == 4)
        hasKeepTime = true;
    BYTE damageType = levels[0].damageType;
    BYTE stateType = levels[0].stateType;

    int rowsY;
    if (type < 41 || type > 60)
    {
        createStatic(20, y + 3, 70, 14, L"Damage Type:", 0, 4);
        g1a_createCombo(m_damageType, m_hWnd, 100, y, 250, 250, 0x6B);
        SkillCombo_fillDamageTypes(m_damageType);
        SendMessageW(m_damageType.m_hWnd, CB_SETCURSEL, damageType, 0);

        createStatic(360, y + 3, 60, 14, L"State Type:", 0, 4);
        g1a_createCombo(m_stateType, m_hWnd, 420, y, 160, 250, 0x6C);
        width = 580;
        SkillCombo_fillStateTypes(m_stateType);
        SendMessageW(m_stateType.m_hWnd, CB_SETCURSEL, stateType, 0);
        rowsY = y + 32;
    }
    else
    {
        rowsY = y;
    }

    switch (type)
    {
    case 21:
    case 62:
    case 64:
    case 65:
    case 67:
    case 82:
        rowsY = createStatRows(rowsY, &width, 3);
        break;
    case 91:
        rowsY = createStatRows(rowsY, &width, 6);
        break;
    default:
        break;
    }

    int headerY = rowsY + 16;
    createStatic(20, headerY, 20, 18, L"#", 0, 4);
    createStatic(40, headerY, 30, 18, L"Lvl", 0, 4);
    createStatic(75, headerY, 30, 14, L"Pts", 0, 4);
    createStatic(110, headerY, 45, 14, L"Cost SP", 0, 4);
    createStatic(155, headerY, 45, 14, L"Cost MP", 0, 4);
    createStatic(210, headerY, 50, 14, L"Cast", 0, 4);
    createStatic(240, headerY, 40, 14, L"Reset", 0, 4);
    createStatic(290, headerY, 50, 14, L"Chance", 0, 4);
    createStatic(350, headerY, 40, 14, L"Element", 0, 4);
    createStatic(425, headerY, 50, 14, L"Target", 0, 4);
    createStatic(550, headerY, 40, 14, L"Dist.", 0, 4);
    createStatic(580, headerY, 40, 14, L"Range", 0, 4);

    int x = 620;
    switch (type)
    {
    case 22:
        createStatic(625, headerY, 40, 14, L"Hits", 0, 4);
        x = 650;
    case 21:
        createStatic(x, headerY - 16, 135, 14, L"Abilities", SS_CENTER, 4);
        createStatic(x, headerY, 40, 14, L"Stat.1", SS_CENTER, 4);
        createStatic(x + 45, headerY, 40, 14, L"Stat.2", SS_CENTER, 4);
        createStatic(x + 90, headerY, 40, 14, L"Stat.3", SS_CENTER, 4);
        x += 135;
    case 27:
    case 74:
        createStatic(x, headerY - 16, 135, 14, L"Base Damage", SS_CENTER, 4);
        createStatic(x, headerY, 40, 14, L"HP", SS_CENTER, 4);
        createStatic(x + 45, headerY, 40, 14, L"SP", SS_CENTER, 4);
        createStatic(x + 90, headerY, 40, 14, L"MP", SS_CENTER, 4);
        x += 135;
        break;
    case 25:
        createStatic(620, headerY - 16, 135, 14, L"Base Damage", SS_CENTER, 4);
        createStatic(620, headerY, 40, 14, L"HP", SS_CENTER, 4);
        createStatic(665, headerY, 40, 14, L"SP", SS_CENTER, 4);
        createStatic(710, headerY, 40, 14, L"MP", SS_CENTER, 4);
        createStatic(755, headerY - 16, 135, 14, L"Add. Damage", SS_CENTER, 4);
        createStatic(755, headerY, 40, 14, L"HP", SS_CENTER, 4);
        createStatic(800, headerY, 40, 14, L"SP", SS_CENTER, 4);
        createStatic(845, headerY, 40, 14, L"MP", SS_CENTER, 4);
        x = 890;
        break;
    case 41:
        createStatic(620, headerY - 16, 135, 14, L"Heal", SS_CENTER, 4);
        createStatic(620, headerY, 40, 14, L"HP", SS_CENTER, 4);
        createStatic(665, headerY, 40, 14, L"SP", SS_CENTER, 4);
        createStatic(710, headerY, 40, 14, L"MP", SS_CENTER, 4);
        x = 755;
        break;
    case 61:
        createStatic(620, headerY - 16, 135, 14, L"Base Damage", SS_CENTER, 4);
        createStatic(620, headerY, 40, 14, L"HP", SS_CENTER, 4);
        createStatic(665, headerY, 40, 14, L"SP", SS_CENTER, 4);
        createStatic(710, headerY, 40, 14, L"MP", SS_CENTER, 4);
        createStatic(755, headerY - 16, 135, 14, L"Time Damage", SS_CENTER, 4);
        createStatic(755, headerY, 40, 14, L"HP", SS_CENTER, 4);
        createStatic(800, headerY, 40, 14, L"SP", SS_CENTER, 4);
        createStatic(845, headerY, 40, 14, L"MP", SS_CENTER, 4);
        x = 890;
        break;
    case 62:
    case 67:
        createStatic(620, headerY - 16, 135, 14, L"Base Damage", SS_CENTER, 4);
        createStatic(620, headerY, 40, 14, L"HP", SS_CENTER, 4);
        createStatic(665, headerY, 40, 14, L"SP", SS_CENTER, 4);
        createStatic(710, headerY, 40, 14, L"MP", SS_CENTER, 4);
        createStatic(755, headerY - 16, 135, 14, L"Abilities", SS_CENTER, 4);
        createStatic(755, headerY, 40, 14, L"Abt.1", SS_CENTER, 4);
        createStatic(800, headerY, 40, 14, L"Abt.2", SS_CENTER, 4);
        createStatic(845, headerY, 40, 14, L"Abt.3", SS_CENTER, 4);
        x = 890;
        break;
    case 64:
    case 65:
        createStatic(620, headerY - 16, 135, 14, L"Abilities", SS_CENTER, 4);
        createStatic(620, headerY, 40, 14, L"Stat.1", SS_CENTER, 4);
        createStatic(665, headerY, 40, 14, L"Stat.2", SS_CENTER, 4);
        createStatic(710, headerY, 40, 14, L"Stat.3", SS_CENTER, 4);
        createStatic(755, headerY, 40, 14, L"Dmg HP", SS_CENTER, 4);
        x = 800;
        break;
    case 66:
    case 126:
        createStatic(620, headerY, 40, 14, L"Dmg HP", SS_CENTER, 4);
        x = 665;
        break;
    case 69:
        createStatic(620, headerY, 40, 14, L"Taunt", SS_CENTER, 4);
        x = 665;
        hasKeepTime = false;
        break;
    case 72:
        createStatic(620, headerY, 40, 14, L"Taunt", SS_CENTER, 4);
        x = 665;
        hasKeepTime = true;
        break;
    case 81:
        createStatic(620, headerY - 16, 135, 14, L"Heal over time", SS_CENTER, 4);
        createStatic(620, headerY, 50, 14, L"HP", SS_CENTER, 4);
        createStatic(665, headerY, 50, 14, L"SP", SS_CENTER, 4);
        createStatic(710, headerY, 50, 14, L"MP", SS_CENTER, 4);
        x = 755;
        break;
    case 82:
        createStatic(620, headerY - 16, 135, 14, L"Abilities", SS_CENTER, 4);
        createStatic(620, headerY, 40, 14, L"Abt.1", SS_CENTER, 4);
        createStatic(665, headerY, 40, 14, L"Abt.2", SS_CENTER, 4);
        createStatic(710, headerY, 40, 14, L"Abt.3", SS_CENTER, 4);
        x = 755;
        break;
    case 83:
        createStatic(620, headerY, 40, 14, L"Phys", SS_CENTER, 4);
        createStatic(665, headerY, 40, 14, L"Dist", SS_CENTER, 4);
        createStatic(710, headerY, 40, 14, L"Magic", SS_CENTER, 4);
        x = 755;
        break;
    case 84:
        createStatic(620, headerY - 16, 135, 14, L"Base Damage", SS_CENTER, 4);
        createStatic(620, headerY, 40, 14, L"HP", SS_CENTER, 4);
        createStatic(665, headerY, 40, 14, L"SP", SS_CENTER, 4);
        createStatic(710, headerY, 40, 14, L"MP", SS_CENTER, 4);
        createStatic(755, headerY, 40, 14, L"Taunt", SS_CENTER, 4);
        x = 800;
        break;
    case 85:
    case 86:
        createStatic(620, headerY, 40, 28, L"Defence", SS_CENTER, 4);
        x = 665;
        break;
    case 91:
        createStatic(620, headerY - 16, 135, 14, L"Abilities", SS_CENTER, 4);
        createStatic(620, headerY, 40, 14, L"Abt.1", SS_CENTER, 4);
        createStatic(665, headerY, 40, 14, L"Abt.2", SS_CENTER, 4);
        createStatic(710, headerY, 40, 14, L"Abt.3", SS_CENTER, 4);
        createStatic(755, headerY, 40, 14, L"Abt.4", SS_CENTER, 4);
        createStatic(800, headerY, 40, 14, L"Abt.5", SS_CENTER, 4);
        createStatic(845, headerY, 40, 14, L"Abt.6", SS_CENTER, 4);
        x = 890;
        break;
    case 93:
        hasKeepTime = true;
        break;
    case 101:
        createStatic(620, headerY, 40, 14, L"Dmg HP", SS_CENTER, 4);
        x = 665;
        hasKeepTime = true;
        break;
    default:
        break;
    }

    switch (stateType)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 9:
    case 10:
    case 11:
        hasKeepTime = true;
        break;
    default:
        break;
    }
    if (hasKeepTime)
    {
        createStatic(x - 2, headerY, 60, 14, L"KeepTime", 0, 4);
        x += 55;
    }

    int rowY = headerY + 18;
    wchar_t buffer[16] = {};
    if (m_skill->m_levelCount > 0)
    {
        int comboY = rowY - 2;
        int id = 200;
        int level = 0;
        do
        {
            SkillData* lvl = &levels[level];
            swprintf_s(buffer, 16, L"%d", id - 199);
            createStatic(20, comboY + 5, 20, 18, buffer, 0, 1);
            m_reqLevel[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)id, 35, rowY, 30, 18, lvl->reqLevel, 0, 0);
            m_points[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 15), 70, rowY, 30, 18, lvl->points, 0, 0);
            m_costSP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 30), 110, rowY, 40, 18, lvl->sp, 0, 0);
            m_costMP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 45), 155, rowY, 40, 18, lvl->mp, 0, 0);
            m_castTime[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 60), 205, rowY, 30, 18, lvl->readyTime, 0, 0);
            m_resetTime[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 75), 240, rowY, 40, 18, lvl->resetTime, 0, 0);
            g1a_createButton(m_chanceEnabled[level], m_hWnd, 0, 0x50000002, 285, rowY, 20, 18, id + 105);
            g1a_subclassCheckBox(m_chanceEnabled[level]);
            m_chance[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 120), 305, rowY, 30, 18, lvl->successValue, 0, 0);
            g1a_createCombo(m_element[level], m_hWnd, 345, comboY, 65, 350, id + 620);
            g1a_createCombo(m_target[level], m_hWnd, 420, comboY, 120, 100, id + 635);
            SkillCombo_fillToonTargets(m_target[level], (unsigned short)(type - 41) > 2u);
            SendMessageW(m_target[level].m_hWnd, CB_SETCURSEL, (WPARAM)(lvl->targetType - 1), 0);
            m_distance[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 320), 545, rowY, 30, 18, lvl->attackRange, 0, 0);
            m_range[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 335), 580, rowY, 30, 18, lvl->applyRange, 0, 0);

            int colX = 620;
            x = 620;
            switch (type)
            {
            case 22:
                m_hits[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 90), 620, rowY, 25, 18, lvl->multiAttack, 0, 0);
                colX = 650;
            case 21:
                m_statValues[level][0].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 160), colX, rowY, 40, 18,
                                                      lvl->ability[0].value, 0, 0);
                m_statValues[level][1].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 175), colX + 45, rowY, 40, 18,
                                                      lvl->ability[1].value, 0, 0);
                m_statValues[level][2].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 190), colX + 90, rowY, 40, 18,
                                                      lvl->ability[2].value, 0, 0);
                colX += 135;
            case 27:
            case 74:
                m_baseDmgHP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 365), colX, rowY, 40, 18,
                                                  lvl->damage[0], 0, 0);
                m_baseDmgSP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 380), colX + 45, rowY, 40, 18,
                                                  lvl->damage[1], 0, 0);
                m_baseDmgMP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 395), colX + 90, rowY, 40, 18,
                                                  lvl->damage[2], 0, 0);
                colX += 135;
                x = colX;
                break;
            case 25:
                m_baseDmgHP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 365), 620, rowY, 40, 18, lvl->damage[0], 0, 0);
                m_baseDmgSP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 380), 665, rowY, 40, 18, lvl->damage[1], 0, 0);
                m_baseDmgMP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 395), 710, rowY, 40, 18, lvl->damage[2], 0, 0);
                m_addDmgHP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 410), 755, rowY, 40, 18, lvl->addDamage[0], 0, 0);
                m_addDmgSP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 425), 800, rowY, 40, 18, lvl->addDamage[1], 0, 0);
                m_addDmgMP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 440), 845, rowY, 40, 18, lvl->addDamage[2], 0, 0);
                colX = 890;
                x = colX;
                break;
            case 41:
                m_healHP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 500), 620, rowY, 40, 18, lvl->heal[0], 0, 0);
                m_healSP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 515), 665, rowY, 40, 18, lvl->heal[1], 0, 0);
                m_healMP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 530), 710, rowY, 40, 18, lvl->heal[2], 0, 0);
                colX = 755;
                x = colX;
                break;
            case 61:
                m_baseDmgHP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 365), 620, rowY, 40, 18, lvl->damage[0], 0, 0);
                m_baseDmgSP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 380), 665, rowY, 40, 18, lvl->damage[1], 0, 0);
                m_baseDmgMP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 395), 710, rowY, 40, 18, lvl->damage[2], 0, 0);
                m_dotHP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 455), 755, rowY, 40, 18, lvl->timeDamage[0], 0, 0);
                m_dotSP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 470), 800, rowY, 40, 18, lvl->timeDamage[1], 0, 0);
                m_dotMP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 485), 845, rowY, 40, 18, lvl->timeDamage[2], 0, 0);
                colX = 890;
                x = colX;
                break;
            case 62:
            case 67:
                m_baseDmgHP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 365), 620, rowY, 40, 18, lvl->damage[0], 0, 0);
                m_baseDmgSP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 380), 665, rowY, 40, 18, lvl->damage[1], 0, 0);
                m_baseDmgMP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 395), 710, rowY, 40, 18, lvl->damage[2], 0, 0);
                m_statValues[level][0].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 160), 755, rowY, 40, 18, lvl->ability[0].value, 0, 0);
                m_statValues[level][1].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 175), 800, rowY, 40, 18, lvl->ability[1].value, 0, 0);
                m_statValues[level][2].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 190), 845, rowY, 40, 18, lvl->ability[2].value, 0, 0);
                colX = 890;
                x = colX;
                break;
            case 64:
            case 65:
                m_statValues[level][0].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 160), 620, rowY, 40, 18, lvl->ability[0].value, 0, 0);
                m_statValues[level][1].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 175), 665, rowY, 40, 18, lvl->ability[1].value, 0, 0);
                m_statValues[level][2].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 190), 710, rowY, 40, 18, lvl->ability[2].value, 0, 0);
                m_baseDmgHP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 365), 755, rowY, 40, 18, lvl->damage[0], 0, 0);
                colX = 800;
                x = colX;
                break;
            case 66:
            case 101:
            case 126:
                m_baseDmgHP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 365), 620, rowY, 40, 18, lvl->damage[0], 0, 0);
                colX = 665;
                x = colX;
                break;
            case 69:
            case 72:
                m_aggro[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 650), 620, rowY, 45, 18, lvl->arrow, 0, 0);
                colX = 665;
                x = colX;
                break;
            case 81:
                m_hotHP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 545), 620, rowY, 40, 18, lvl->timeHeal[0], 0, 0);
                m_hotSP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 560), 665, rowY, 40, 18, lvl->timeHeal[1], 0, 0);
                m_hotMP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 575), 710, rowY, 40, 18, lvl->timeHeal[2], 0, 0);
                colX = 755;
                x = colX;
                break;
            case 82:
            case 83:
                m_statValues[level][0].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 160), 620, rowY, 40, 18, lvl->ability[0].value, 0, 0);
                m_statValues[level][1].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 175), 665, rowY, 40, 18, lvl->ability[1].value, 0, 0);
                m_statValues[level][2].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 190), 710, rowY, 40, 18, lvl->ability[2].value, 0, 0);
                colX = 755;
                x = colX;
                break;
            case 84:
                m_baseDmgHP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 365), 620, rowY, 40, 18, lvl->damage[0], 0, 0);
                m_baseDmgSP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 380), 665, rowY, 40, 18, lvl->damage[1], 0, 0);
                m_baseDmgMP[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 395), 710, rowY, 40, 18, lvl->damage[2], 0, 0);
                m_aggro[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 650), 755, rowY, 45, 18, lvl->arrow, 0, 0);
                colX = 800;
                x = colX;
                break;
            case 85:
            case 86:
                m_defence[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 590), 620, rowY, 40, 18, lvl->defenceValue, 0, 0);
                colX = 665;
                x = colX;
                break;
            case 91:
                m_statValues[level][0].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 160), 620, rowY, 40, 18, lvl->ability[0].value, 0, 0);
                m_statValues[level][1].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 175), 665, rowY, 40, 18, lvl->ability[1].value, 0, 0);
                m_statValues[level][2].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 190), 710, rowY, 40, 18, lvl->ability[2].value, 0, 0);
                m_statValues[level][3].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 205), 755, rowY, 40, 18, lvl->ability[3].value, 0, 0);
                m_statValues[level][4].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 220), 800, rowY, 40, 18, lvl->ability[4].value, 0, 0);
                m_statValues[level][5].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 235), 845, rowY, 40, 18, lvl->ability[5].value, 0, 0);
                colX = 890;
                x = colX;
                break;
            default:
                break;
            }
            if (hasKeepTime)
            {
                m_keepTime[level].createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 350), colX, rowY, 40, 18,
                                                 lvl->keepTime, 0, 0);
                x += 55;
            }

            comboY += 23;
            ++level;
            ++id;
            rowY += 23;
        } while (id - 200 < m_skill->m_levelCount);
    }

    int bottom = rowY + 16;
    for (int targetId = 835; targetId < m_skill->m_levelCount + 835; ++targetId)
        onCommand(1, 0, targetId);

    if (m_skill->m_levelCount > 0)
    {
        int level = 0;
        do
        {
            SkillData* lvl = &levels[level];
            bool chanceEnabled = lvl->successType == 1;
            m_chanceEnabled[level].setCheck(chanceEnabled);
            m_chance[level].enable(chanceEnabled);
            SkillCombo_fillElements(m_element[level]);
            SendMessageW(m_element[level].m_hWnd, CB_SETCURSEL, lvl->attribType, 0);
            ++level;
        } while (level < m_skill->m_levelCount);
    }

    SIZE size;
    size.cx = x;
    size.cy = bottom;
    return size;
}

int ToonSkillEditor::createStatRows(int y, int* maxWidth, int count)
{
    if (!count)
    {
        count = 3;
        if ((m_skill->m_format & 0xFFF) >= 0x600)
            count = 9;
    }
    int stat = 0;
    int rows = count / 3;
    if (rows <= 0)
        return y + 10;

    vShort& statIds = m_statIds;
    int rowY = y;
    int lastY;
    do
    {
        int x = 20;
        int column = 3;
        do
        {
            ComboBox& combo = m_statTypes[stat];
            wchar_t buffer[16] = {};
            int next = stat + 1;
            swprintf_s(buffer, 16, L"Stat. %d:", next);
            createStatic(x, rowY + 6, 45, 14, buffer, 0, 4);
            g1a_createComboEx(combo, m_hWnd, x + 45, rowY, 150, 350, stat + 350);

            for (int i = 0; i < statIds.m_capacity; ++i)
                statIds.resetItem(&statIds.m_items[i]);
            statIds.m_count = 0;
            const wchar_t* name = g_skillStatNames[0];
            int index = 0;
            if (name)
            {
                do
                {
                    combo.addItem(name, index);
                    statIds.add((short)index);
                    name = g_skillStatNames[++index];
                } while (name);
            }
            combo.resize(150, 350);
            SkillCombo_selectStat(combo, m_skill->m_levels, m_skill->m_levelCount, stat);
            stat = next;
            x += 200;
        } while (--column);
        if (*maxWidth < x)
            *maxWidth = x;
        lastY = rowY + 26;
        rowY += 26;
    } while (--rows);
    return lastY + 10;
}

void ToonSkillEditor::enableStatColumn(int column, bool enable)
{
    for (int level = 0; level < m_skill->m_levelCount; ++level)
        m_statValues[level][column].enable(enable);
}
