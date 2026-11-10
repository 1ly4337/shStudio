#include "QuestEditor.h"
#include <commctrl.h>
#include <cwchar>
#include <cstdio>
#include "String.hpp"
#include "StringTable.h"
#include "HTable.h"
#include "QuestDetails.h"
#include "_imports_F.h"

namespace {

void qeCreateEdit(EditText& edit, HWND parent, DWORD style, UINT id, int x, int y, int cx, int cy, WPARAM limit)
{
    edit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", style, x, y, cx, cy, parent,
                                  (HMENU)(UINT_PTR)id, 0, 0);
    if (edit.m_hWnd) {
        edit.setFontId(edit.m_fontId);
        SetWindowSubclass(edit.m_hWnd, tabSubclassProc, 0x65646974 , (DWORD_PTR)&edit);
    }
    SendMessageW(edit.m_hWnd, EM_LIMITTEXT, limit, 0);
}

void qeCreateCombo(ComboBox& combo, HWND parent, UINT id, int x, int y, int cx, int cy)
{
    combo.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"ComboBox", 0, (combo.m_dropLines & 3) | 0x50010000,
                                   x, y, cx, cy, parent, (HMENU)(UINT_PTR)id, 0, 0);
    if (combo.m_hWnd)
        combo.setFontId(combo.m_fontId);
    else
        GetLastError();
}

void qeAddComboItem(ComboBox& combo, LPCWSTR text)
{
    if (SendMessageW(combo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)text) != CB_ERR) {
        LRESULT count = SendMessageW(combo.m_hWnd, CB_GETCOUNT, 0, 0);
        combo.m_items.StringTable::put((int)count, text);
    }
}

void qeCreateButton(Button& button, HWND parent, LPCWSTR text, DWORD style, UINT id, int x, int y, int cx, int cy)
{
    button.m_hWnd = CreateWindowExW(0, L"Button", text, style, x, y, cx, cy, parent, (HMENU)(UINT_PTR)id, 0, 0);
    if (button.m_hWnd)
        button.setFontId(button.m_fontId);
    else
        GetLastError();
}

void qeSubclassCheck(Button& button)
{
    if (button.m_hWnd)
        SetWindowSubclass(button.m_hWnd, noEraseSubclassProc, 0x6368636B , (DWORD_PTR)&button);
}

}

QuestEditor::QuestEditor(QuestDetails* quest)
    : Modal()
    , m_quest(quest)
{
}

QuestEditor::~QuestEditor()
{
}

bool QuestEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code == BN_CLICKED) {
        if (id == 109) {
            m_repeatable.setCheck(!m_repeatable.isChecked());
            return true;
        }
        if (id == 168) {
            bool checked = !m_defaultReward.isChecked();
            m_defaultReward.setCheck(checked);
            m_rewardMode.enable(!checked);
            return true;
        }
        if (id - 112 <= 5) {
            Button& button = m_classes[id - 112];
            button.setCheck(!button.isChecked());
            return true;
        }
        if (id - 120 <= 5) {
            Button& button = m_rewMobCounter[id - 120];
            bool checked = !button.isChecked();
            button.setCheck(checked);
            m_rewMobCount[id - 120].enable(checked);
            return true;
        }
        if (id - 130 <= 5) {
            Button& button = m_rewItemCounter[id - 130];
            bool checked = !button.isChecked();
            button.setCheck(checked);
            m_rewItemCount[id - 130].enable(checked);
            return true;
        }
    } else if (code == CBN_SELCHANGE && id == 143) {
        LRESULT type = SendMessageW(m_questType.m_hWnd, CB_GETCURSEL, 0, 0);
        enableObjectives((int)type);
        return true;
    }
    return Modal::onCommand(code, ctrl, id);
}

bool QuestEditor::open(HWND parent)
{
    m_textCount = 3;
    int count = m_textCount;
    if ((m_quest->m_version & 0xFFF) >= 0x600)
        count = 6;
    m_textCount = count;

    int screenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
    int height;
    if (m_textCount == 6) {
        if (screenHeight > 1180) {
            m_compact = false;
            height = 1110;
        } else {
            height = 965;
            m_compact = true;
        }
    } else {
        if (screenHeight > 900) {
            m_compact = false;
            height = 860;
        } else {
            height = 750;
            m_compact = true;
        }
    }
    m_buttons = MB_OKCANCEL;

    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 835, height);
    bool created = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Quest Details");
    if (created) {
        HWND owner = Modal_getTopLevelWindow(parent);
        m_owner = owner;
        Window_centerOn(m_hWnd, owner);
        show(true);
        return true;
    }
    return created;
}

bool QuestEditor::onCreate()
{
    const QuestReq& req = m_quest->m_req;
    HWND hLabel;

    m_btnMarginRight = 30;

    createStatic(30, 10, 90, 18, L"Name of Quest:", 0, 5);
    qeCreateEdit(m_name, m_hWnd, 0x50010080, 0x65, 120, 8, 285, 18, 0x32);
    m_name.setText(m_quest->m_name.c_str());

    createStatic(435, 12, 60, 18, L"Quest ID:", 0, 5);
    String idText;
    idText << m_quest->m_questId;
    createStatic(495, 12, 50, 18, idText.c_str(), 0, 5);

    createStatic(35, 36, 85, 18, L"Type of quest:", 0, 4);
    qeCreateCombo(m_questType, m_hWnd, 0x8F, 120, 32, 50, 200);
    qeAddComboItem(m_questType, L"Final");
    qeAddComboItem(m_questType, L"Farm");
    qeAddComboItem(m_questType, L"PvE");
    qeAddComboItem(m_questType, L"PvP");
    SendMessageW(m_questType.m_hWnd, CB_SETCURSEL, req.qtyp, 0);

    createStatic(180, 36, 50, 18, L"Obtained:", 0, 4);
    qeCreateCombo(m_obtained, m_hWnd, 0x90, 230, 32, 175, 200);
    qeAddComboItem(m_obtained, L"Conditional quest in a chain");
    qeAddComboItem(m_obtained, L"Standard quest given by a NPC");
    qeAddComboItem(m_obtained, L"Quest given by object");
    qeAddComboItem(m_obtained, L"Hidden quest activated by a drop");
    qeAddComboItem(m_obtained, L"Letter-quest activated by click");
    SendMessageW(m_obtained.m_hWnd, CB_SETCURSEL, req.qmod, 0);

    createStatic(40, 68, 120, 13, L"Received items", 0, 5);
    createStatic(40, 81, 35, 16, L"Type", 0, 4);
    createStatic(75, 81, 35, 16, L"TypeID", 0, 4);
    createStatic(120, 81, 40, 16, L"Count", 0, 4);
    createStatic(20, 101, 15, 16, L"1", 0, 5);
    m_recvItemType[0].EditText::createNumber(m_hWnd, (HMENU)145, 35, 97, 35, 18, req.rty1, 0, 0);
    m_recvItemTypeId[0].EditText::createNumber(m_hWnd, (HMENU)146, 75, 97, 40, 18, req.rti1, 0, 0);
    m_recvItemCount[0].EditText::createNumber(m_hWnd, (HMENU)147, 120, 97, 35, 18, req.rcat1, 0, 0);
    createStatic(20, 125, 15, 16, L"2", 0, 5);
    m_recvItemType[1].EditText::createNumber(m_hWnd, (HMENU)148, 35, 121, 35, 18, req.rty2, 0, 0);
    m_recvItemTypeId[1].EditText::createNumber(m_hWnd, (HMENU)149, 75, 121, 40, 18, req.rti2, 0, 0);
    m_recvItemCount[1].EditText::createNumber(m_hWnd, (HMENU)150, 120, 121, 35, 18, req.rcat2, 0, 0);
    createStatic(20, 149, 15, 16, L"3", 0, 5);
    m_recvItemType[2].EditText::createNumber(m_hWnd, (HMENU)151, 35, 145, 35, 18, req.rty3, 0, 0);
    m_recvItemTypeId[2].EditText::createNumber(m_hWnd, (HMENU)152, 75, 145, 40, 18, req.rti3, 0, 0);
    m_recvItemCount[2].EditText::createNumber(m_hWnd, (HMENU)153, 120, 145, 35, 18, req.rcat3, 0, 0);

    createStatic(180, 68, 120, 13, L"Items to farm", 0, 5);
    createStatic(180, 81, 35, 16, L"Type", 0, 4);
    createStatic(220, 81, 35, 16, L"TypeID", 0, 4);
    createStatic(265, 81, 40, 16, L"Count", 0, 4);
    m_farmItemType[0].EditText::createNumber(m_hWnd, (HMENU)154, 175, 97, 35, 18, req.fty1, 0, 0);
    m_farmItemTypeId[0].EditText::createNumber(m_hWnd, (HMENU)155, 215, 97, 40, 18, req.fti1, 0, 0);
    m_farmItemCount[0].EditText::createNumber(m_hWnd, (HMENU)156, 260, 97, 35, 18, req.fct1, 0, 0);
    m_farmItemType[1].EditText::createNumber(m_hWnd, (HMENU)157, 175, 121, 35, 18, req.fty2, 0, 0);
    m_farmItemTypeId[1].EditText::createNumber(m_hWnd, (HMENU)158, 215, 121, 40, 18, req.fti2, 0, 0);
    m_farmItemCount[1].EditText::createNumber(m_hWnd, (HMENU)159, 260, 121, 35, 18, req.fct2, 0, 0);
    m_farmItemType[2].EditText::createNumber(m_hWnd, (HMENU)160, 175, 145, 35, 18, req.fty3, 0, 0);
    m_farmItemTypeId[2].EditText::createNumber(m_hWnd, (HMENU)161, 215, 145, 40, 18, req.fti3, 0, 0);
    m_farmItemCount[2].EditText::createNumber(m_hWnd, (HMENU)162, 260, 145, 35, 18, req.fct3, 0, 0);

    createStatic(315, 68, 90, 13, L"Mobs to fight", 0, 5);
    createStatic(315, 81, 35, 16, L"Type", 0, 4);
    createStatic(365, 81, 35, 16, L"Count", 0, 4);
    m_mobType[0].EditText::createNumber(m_hWnd, (HMENU)163, 310, 97, 45, 18, req.mbi1, 0, 0);
    m_mobCount[0].EditText::createNumber(m_hWnd, (HMENU)164, 360, 97, 35, 18, req.mct1, 0, 0);
    m_mobType[1].EditText::createNumber(m_hWnd, (HMENU)165, 310, 121, 45, 18, req.mbi2, 0, 0);
    m_mobCount[1].EditText::createNumber(m_hWnd, (HMENU)166, 360, 121, 35, 18, req.mct2, 0, 0);
    createStatic(310, 149, 50, 13, L"PvP kills", 0, 5);
    m_pvpKills.EditText::createNumber(m_hWnd, (HMENU)167, 360, 145, 35, 18, req.kpvp, 0, 0);

    createStatic(435, 32, 50, 18, L"Quest for:", 0, 4);
    qeCreateCombo(m_faction, m_hWnd, 0x6E, 495, 28, 100, 200);
    qeAddComboItem(m_faction, L"Human");
    qeAddComboItem(m_faction, L"Elf");
    qeAddComboItem(m_faction, L"Light");
    qeAddComboItem(m_faction, L"Deatheater");
    qeAddComboItem(m_faction, L"Vail");
    qeAddComboItem(m_faction, L"Fury");
    qeAddComboItem(m_faction, L"All factions");
    SendMessageW(m_faction.m_hWnd, CB_SETCURSEL, req.country, 0);

    createStatic(605, 32, 45, 18, L"in mode:", 0, 4);
    qeCreateCombo(m_mode, m_hWnd, 0x6F, 655, 28, 120, 150);
    qeAddComboItem(m_mode, L"Easy or higher");
    qeAddComboItem(m_mode, L"Normal or higher");
    qeAddComboItem(m_mode, L"Difficult or higher");
    qeAddComboItem(m_mode, L"Ultimate");
    SendMessageW(m_mode.m_hWnd, CB_SETCURSEL, req.grow, 0);

    createStatic(435, 56, 50, 18, L"Classes:", 0, 4);
    qeCreateButton(m_classes[0], m_hWnd, L"Fighter", 0x50010002, 0x70, 495, 54, 70, 18);
    qeCreateButton(m_classes[1], m_hWnd, L"Defender", 0x50000002, 0x71, 585, 54, 70, 18);
    qeSubclassCheck(m_classes[1]);
    qeCreateButton(m_classes[2], m_hWnd, L"Ranger", 0x50000002, 0x72, 675, 54, 70, 18);
    qeSubclassCheck(m_classes[2]);
    qeCreateButton(m_classes[3], m_hWnd, L"Archer", 0x50000002, 0x73, 495, 74, 70, 18);
    qeSubclassCheck(m_classes[3]);
    qeCreateButton(m_classes[4], m_hWnd, L"Mage", 0x50000002, 0x74, 585, 74, 70, 18);
    qeSubclassCheck(m_classes[4]);
    qeCreateButton(m_classes[5], m_hWnd, L"Priest", 0x50000002, 0x75, 675, 74, 70, 18);
    qeSubclassCheck(m_classes[5]);
    for (int i = 0; i < 6; ++i)
        m_classes[i].setCheck(req.classes[i] == 1);

    createStatic(435, 99, 50, 16, L"Level:", 0, 4);
    m_minLevel.EditText::createNumber(m_hWnd, (HMENU)140, 495, 97, 40, 18, req.minLevel, 0, 0);
    createStatic(535, 99, 10, 16, L"-", SS_CENTER, 4);
    m_maxLevel.EditText::createNumber(m_hWnd, (HMENU)141, 545, 97, 40, 18, req.maxLevel, 0, 0);
    createStatic(600, 99, 50, 16, L"Delay:", 0, 4);
    m_delay.EditText::createNumber(m_hWnd, (HMENU)142, 670, 97, 40, 18, req.timer, 0, 0);
    createStatic(715, 99, 60, 16, L"(0: no limit)", 0, 4);

    createStatic(435, 123, 50, 16, L"Given by:", 0, 4);
    m_givenByType.EditText::createNumber(m_hWnd, (HMENU)102, 495, 121, 40, 18, req.nty1, 0, 0);
    m_givenById.EditText::createNumber(m_hWnd, (HMENU)103, 545, 121, 40, 18, req.nti1, 0, 0);
    createStatic(600, 123, 70, 16, L"Received by:", 0, 4);
    m_receivedByType.EditText::createNumber(m_hWnd, (HMENU)104, 670, 121, 40, 18, req.nty2, 0, 0);
    m_receivedById.EditText::createNumber(m_hWnd, (HMENU)105, 720, 121, 40, 18, req.nti2, 0, 0);

    createStatic(435, 147, 50, 16, L"Requires:", 0, 4);
    m_requires1.EditText::createNumber(m_hWnd, (HMENU)106, 495, 145, 40, 18, req.prv1, 0, 0);
    m_requires2.EditText::createNumber(m_hWnd, (HMENU)107, 545, 145, 40, 18, req.prv2, 0, 0);
    m_requires3.EditText::createNumber(m_hWnd, (HMENU)108, 595, 145, 40, 18, req.prv3, 0, 0);

    qeCreateButton(m_repeatable, m_hWnd, L" Repeatable", 0x50010002, 0x6D, 675, 145, 80, 18);
    m_repeatable.setCheck(req.rept == 1);

    hLabel = createStatic(20, 177, 770, 13, L"Welcome Msg:", 0, 3);
    m_textColors.set(hLabel, 0x606060);
    qeCreateEdit(m_welcomeMsg, m_hWnd, 0x50010080, 0x76, 20, 190, 770, 17, 0x100);

    BOOL tall = m_compact == false;
    hLabel = createStatic(20, 211, 770, 13,
        L"Initial Description: (displayed when quest is given, also 'Quest Info' in quest window)", 0, 3);
    m_textColors.set(hLabel, 0x606060);
    int initialHeight = 13 * (2 * (m_compact == false) + 5) + 4;
    qeCreateEdit(m_initialDesc, m_hWnd, 0x50211044, 0x77, 20, 224, 770, initialHeight, 0x400);

    int y = initialHeight + 228;
    hLabel = createStatic(20, y, 770, 13, L"Summary: ('Quest Mission' in quest window)", 0, 3);
    m_textColors.set(hLabel, 0x606060);
    int extra = 13 * tall;
    y += 13;
    qeCreateEdit(m_summary, m_hWnd, 0x50211044, 0x78, 20, y, 770, extra + 30, 0x200);

    y += extra + 34;
    hLabel = createStatic(20, y, 770, 13,
        L"Reminder instructions: (displayed by NPC that gave the quest if it is not completed)", 0, 3);
    m_textColors.set(hLabel, 0x606060);
    y += 13;
    qeCreateEdit(m_reminder, m_hWnd, 0x50010080, 0x79, 20, y, 770, 17, 0x400);

    y += 21;
    hLabel = createStatic(20, y, 770, 13,
        L"Alternate Response (displayed when the quest is done or inactive):", 0, 3);
    m_textColors.set(hLabel, 0x606060);
    y += 13;
    qeCreateEdit(m_alternateResp, m_hWnd, 0x50211044, 0x7A, 20, y, 770, extra + 17, 0x400);

    y += extra + 17 + 4;
    hLabel = createStatic(20, y, 770, 13,
        L"Completion Msg: (displayed when the quest is validated with all requirements fulfilled)", 0, 3);
    m_textColors.set(hLabel, 0x606060);
    y += 13;
    int completionHeight = 13 * (2 * (m_compact == false) + 2) + 4;
    qeCreateEdit(m_completionMsg[0], m_hWnd, 0x50211044, 0x7B, 20, y, 770, completionHeight, 0x400);
    y += completionHeight + 4;

    String prefix;
    prefix << L"Alternate Completion Msg ";
    if (m_textCount > 1) {
        int id = 124;
        EditText* edit = &m_completionMsg[1];
        do {
            {
                String numbered = sub_41E180(prefix, id - 122);
                String caption = sub_41E0C0(numbered, L':');
                hLabel = createStatic(20, y, 770, 13, caption.c_str(), 0, 3);
                m_textColors.set(hLabel, 0x606060);
            }
            y += 13;
            qeCreateEdit(*edit, m_hWnd, 0x50211044, id, 20, y, 770, extra + 30, 0x400);
            ++edit;
            y += extra + 34;
            ++id;
        } while (id - 123 < m_textCount);
    }

    y += 8;
    createStatic(20, y, 75, 16, L"Rewards", 0, 5);
    int headerY = y + 16;
    createStatic(40, headerY, 70, 16, L"Mob Counter", 0, 4);
    createStatic(115, headerY, 70, 16, L"Item Counter", 0, 4);
    createStatic(195, headerY, 40, 16, L"Gold", 0, 4);
    createStatic(250, headerY, 40, 16, L"XP", 0, 4);
    int x = 320;
    for (int n = 3; n != 0; --n) {
        createStatic(x, headerY, 40, 16, L"Type", 0, 4);
        x += 45;
        createStatic(x, headerY, 40, 16, L"TypeID", 0, 4);
        x += 45;
        createStatic(x, headerY, 40, 16, L"Count", 0, 4);
        x += 50;
    }
    createStatic(x, headerY, 40, 16, L"Unlock", 0, 4);

    int rowY = headerY + 16;
    UINT nextId = 200;
    if (m_textCount > 0) {
        int row = 0;
        int checkId = 120;
        do {
            const QuestResult& result = m_quest->m_results[row];
            wchar_t rowCaption[6];
            swprintf_s(rowCaption, 6, L"%i:", checkId - 119);
            createStatic(20, rowY + 4, 20, 16, rowCaption, 0, 5);

            qeCreateButton(m_rewMobCounter[row], m_hWnd, 0, 0x50000002, checkId, 40, rowY, 20, 18);
            qeSubclassCheck(m_rewMobCounter[row]);
            m_rewMobCount[row].EditText::createNumber(m_hWnd, (HMENU)(UINT_PTR)nextId, 60, rowY, 40, 18, result.mc, 0, 0);
            qeCreateButton(m_rewItemCounter[row], m_hWnd, 0, 0x50000002, checkId + 10, 115, rowY, 20, 18);
            qeSubclassCheck(m_rewItemCounter[row]);
            m_rewItemCount[row].EditText::createNumber(m_hWnd, (HMENU)(UINT_PTR)(nextId + 1), 135, rowY, 40, 18, result.ic, 0, 0);
            m_rewMobCounter[row].setCheck(result.mk == 1);
            m_rewItemCounter[row].setCheck(result.ik == 1);
            m_rewMobCount[row].enable(result.mk == 1);
            m_rewItemCount[row].enable(result.ik == 1);
            m_rewGold[row].EditText::createNumber(m_hWnd, (HMENU)(UINT_PTR)(nextId + 2), 190, rowY, 50, 18,
                                      result.gold, 0, 0);
            m_rewXp[row].EditText::createNumber(m_hWnd, (HMENU)(UINT_PTR)(nextId + 3), 245, rowY, 60, 18,
                                    result.exp, 0, 0);

            UINT itemId = nextId + 4;
            int itemX = 315;
            for (int item = 0; item < 3; ++item) {
                m_rewItemType[row * 3 + item].EditText::createNumber(m_hWnd, (HMENU)(UINT_PTR)itemId, itemX, rowY, 40, 18,
                                                         result.t[item], 0, 0);
                ++itemId;
                itemX += 45;
                m_rewItemTypeId[row * 3 + item].EditText::createNumber(m_hWnd, (HMENU)(UINT_PTR)itemId, itemX, rowY, 40, 18,
                                                           result.i[item], 0, 0);
                ++itemId;
                itemX += 45;
                m_rewItemNum[row * 3 + item].EditText::createNumber(m_hWnd, (HMENU)(UINT_PTR)itemId, itemX, rowY, 40, 18,
                                                        result.c[item], 0, 0);
                itemX += 50;
                ++itemId;
            }
            m_rewUnlock[row].EditText::createNumber(m_hWnd, (HMENU)(UINT_PTR)itemId, itemX, rowY, 50, 18,
                                        result.unl, 0, 0);

            ++row;
            nextId = itemId + 1;
            rowY += 24;
            ++checkId;
        } while (checkId - 120 < m_textCount);
    }

    int comboY = rowY + 6;
    qeCreateButton(m_defaultReward, m_hWnd, L"Give default reward", 0x50000002, 0xA8, 40, rowY + 8, 120, 18);
    qeSubclassCheck(m_defaultReward);
    m_defaultReward.setCheck(req.drwd == 1);
    createStatic(160, comboY + 6, 10, 16, L"or", 0, 5);
    qeCreateCombo(m_rewardMode, m_hWnd, nextId, 180, comboY, 290, 200);
    qeAddComboItem(m_rewardMode, L"Give all rewards or the one matching above criteria");
    qeAddComboItem(m_rewardMode, L"--");
    qeAddComboItem(m_rewardMode, L"Give 1 reward among the 2 listed choices");
    qeAddComboItem(m_rewardMode, L"Give 1 reward among the 3 listed choices");
    qeAddComboItem(m_rewardMode, L"Give 1 reward among the 4 listed choices");
    qeAddComboItem(m_rewardMode, L"Give 1 reward among the 5 listed choices");
    qeAddComboItem(m_rewardMode, L"Give 1 reward among the 6 listed choices");
    SendMessageW(m_rewardMode.m_hWnd, CB_SETCURSEL, req.clrw, 0);
    m_rewardMode.enable(req.drwd == 0);

    if (Modal::onCreate()) {
        enableObjectives(req.qtyp);
        loadTexts();
        return true;
    }
    return false;
}

BOOL QuestEditor::enableObjectives(int questType)
{
    bool farm = questType == 1;
    for (int i = 0; i < 3; ++i) {
        m_farmItemType[i].enable(farm);
        m_farmItemTypeId[i].enable(farm);
        m_farmItemCount[i].enable(farm);
    }
    bool pve = questType == 2;
    for (int i = 0; i < 2; ++i) {
        m_mobType[i].enable(pve);
        m_mobCount[i].enable(pve);
    }
    return m_pvpKills.enable(questType == 3);
}

LRESULT QuestEditor::loadTexts()
{
    m_welcomeMsg.setText(m_quest->m_welcome.c_str());
    m_initialDesc.setText(m_quest->m_desc.c_str());
    m_summary.setText(m_quest->m_summary.c_str());
    m_reminder.setText(m_quest->m_reminder.c_str());
    m_alternateResp.setText(m_quest->m_altMessage.c_str());
    LRESULT result;
    for (int i = 0; i < 6; ++i)
        result = m_completionMsg[i].setText(m_quest->m_replies[i].c_str());
    return result;
}

int QuestEditor::mapCommandToResult(int id)
{
    if (id != IDOK)
        return id;

    QuestReq& req = m_quest->m_req;

    { String text; m_quest->m_name       = *m_name.getText(&text); }
    { String text; m_quest->m_welcome    = *m_welcomeMsg.getText(&text); }
    { String text; m_quest->m_summary    = *m_summary.getText(&text); }
    { String text; m_quest->m_desc       = *m_initialDesc.getText(&text); }
    { String text; m_quest->m_reminder   = *m_reminder.getText(&text); }
    { String text; m_quest->m_altMessage = *m_alternateResp.getText(&text); }
    for (int i = 0; i < 6; ++i) {
        String text;
        m_quest->m_replies[i] = *m_completionMsg[i].getText(&text);
    }

    for (int i = 0; i < 6; ++i)
        req.classes[i] = m_classes[i].isChecked() != 0;

    req.minLevel = (int16_t)m_minLevel.EditText::getInt(0);
    req.maxLevel = (int16_t)m_maxLevel.EditText::getInt(70);
    req.country = (uint8_t)SendMessageW(m_faction.m_hWnd, CB_GETCURSEL, 0, 0);
    req.grow = (uint8_t)SendMessageW(m_mode.m_hWnd, CB_GETCURSEL, 0, 0);
    req.prv1 = (int16_t)m_requires1.EditText::getInt(req.prv1);
    req.prv2 = (int16_t)m_requires2.EditText::getInt(req.prv2);
    req.prv3 = (int16_t)m_requires3.EditText::getInt(req.prv3);
    req.timer = (int16_t)m_delay.EditText::getInt(req.timer);
    req.rept = m_repeatable.isChecked() != 0;
    req.qmod = (uint8_t)SendMessageW(m_obtained.m_hWnd, CB_GETCURSEL, 0, 0);
    req.nty1 = (uint8_t)m_givenByType.EditText::getInt(req.nty1);
    req.nti1 = (int16_t)m_givenById.EditText::getInt(req.nti1);
    req.qtyp = (uint8_t)SendMessageW(m_questType.m_hWnd, CB_GETCURSEL, 0, 0);
    req.nty2 = (uint8_t)m_receivedByType.EditText::getInt(req.nty2);
    req.nti2 = (int16_t)m_receivedById.EditText::getInt(req.nti2);
    req.kpvp = (uint8_t)m_pvpKills.EditText::getInt(0);
    req.fty1 = (uint8_t)m_farmItemType[0].EditText::getInt(0);
    req.fti1 = (uint8_t)m_farmItemTypeId[0].EditText::getInt(0);
    req.fct1 = (uint8_t)m_farmItemCount[0].EditText::getInt(0);
    req.fty2 = (uint8_t)m_farmItemType[1].EditText::getInt(0);
    req.fti2 = (uint8_t)m_farmItemTypeId[1].EditText::getInt(0);
    req.fct2 = (uint8_t)m_farmItemCount[1].EditText::getInt(0);
    req.fty3 = (uint8_t)m_farmItemType[2].EditText::getInt(0);
    req.fti3 = (uint8_t)m_farmItemTypeId[2].EditText::getInt(0);
    req.fct3 = (uint8_t)m_farmItemCount[2].EditText::getInt(0);
    req.mbi1 = (int16_t)m_mobType[0].EditText::getInt(0);
    req.mct1 = (uint8_t)m_mobCount[0].EditText::getInt(0);
    req.mbi2 = (int16_t)m_mobType[1].EditText::getInt(0);
    req.mct2 = (uint8_t)m_mobCount[1].EditText::getInt(0);
    req.rty1 = (uint8_t)m_recvItemType[0].EditText::getInt(0);
    req.rti1 = (uint8_t)m_recvItemTypeId[0].EditText::getInt(0);
    req.rcat1 = (uint8_t)m_recvItemCount[0].EditText::getInt(0);
    req.rty2 = (uint8_t)m_recvItemType[1].EditText::getInt(0);
    req.rti2 = (uint8_t)m_recvItemTypeId[1].EditText::getInt(0);
    req.rcat2 = (uint8_t)m_recvItemCount[1].EditText::getInt(0);
    req.rty3 = (uint8_t)m_recvItemType[2].EditText::getInt(0);
    req.rti3 = (uint8_t)m_recvItemTypeId[2].EditText::getInt(0);
    req.rcat3 = (uint8_t)m_recvItemCount[2].EditText::getInt(0);

    for (int row = 0; row < 6; ++row) {
        QuestResult& result = m_quest->m_results[row];
        result.mk = m_rewMobCounter[row].isChecked() != 0;
        result.ik = m_rewItemCounter[row].isChecked() != 0;
        result.mc = (uint8_t)m_rewMobCount[row].EditText::getInt(0);
        result.ic = (uint8_t)m_rewItemCount[row].EditText::getInt(0);
        for (int item = 0; item < 3; ++item) {
            result.t[item] = (uint8_t)m_rewItemType[row * 3 + item].EditText::getInt(0);
            result.i[item] = (uint8_t)m_rewItemTypeId[row * 3 + item].EditText::getInt(0);
            result.c[item] = (uint8_t)m_rewItemNum[row * 3 + item].EditText::getInt(0);
        }
        result.exp = (int)m_rewXp[row].EditText::getInt(0);
        result.gold = (int)m_rewGold[row].EditText::getInt(0);
        result.unl = (int16_t)m_rewUnlock[row].EditText::getInt(0);
    }

    req.drwd = m_defaultReward.isChecked() != 0;
    uint8_t mode = (uint8_t)SendMessageW(m_rewardMode.m_hWnd, CB_GETCURSEL, 0, 0);
    req.clrw = mode;
    if (mode != 0)
        req.drwd = 0;
    return 1;
}
