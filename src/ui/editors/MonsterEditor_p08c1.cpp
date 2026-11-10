#include <windows.h>
#include <commctrl.h>
#include <cwchar>
#include "ida_compat.h"
#include "MonsterEditor.h"
#include "Window.h"
#include "Container.h"
#include "Modal.h"
#include "String.hpp"
#include "ptrArray.h"
#include "EditText.h"
#include "Button.h"
#include "ComboBox.h"
#include "StringTable.h"
#include "BmpCtrl.h"
#include "CtrlHelpers.h"
#include "BagsList.h"
#include "MetaModel3D.h"
#include "MonsterModels.h"
#include "StudioFrame.h"
#include "RecordSet.h"
#include "_imports_C1.h"
#include "Model3D.h"
#include "SqlServer.h"

class Monster;
class MonsterModel;

const wchar_t* g_debuffNames[15] = {
    L" Sleep",    L" Stun",    L" Silent", L" Darkness", L" Pause",
    L" Slow",     L" Dying",   L" Death",  L" Poison",   L" Illness",
    L" Delusion", L" Doom",    L" Fear",   L" Dull",     L" Bad Luck",
};

static HWND makeCombo(ComboBox& cb, DWORD exStyle, const wchar_t* className, DWORD style,
                        int x, int y, int cx, int cy, HWND parent, int id)
{
    cb.m_hWnd = CreateWindowExW(exStyle, className, 0, style, x, y, cx, cy, parent, (HMENU)(INT_PTR)id, 0, 0);
    if (cb.m_hWnd)
        cb.setFontId(cb.m_fontId);
    else
        GetLastError();
    return cb.m_hWnd;
}

static HWND makeDropList(ComboBox& cb, int x, int y, int cx, int cy, HWND parent, int id)
{
    return makeCombo(cb, WS_EX_CLIENTEDGE, L"ComboBox", (cb.m_dropLines & 3) | 0x50010000,
                       x, y, cx, cy, parent, id);
}

static void makeAttackTypeCombo(ComboBox& cb, int x, int y, HWND parent, int id)
{
    cb.m_hWnd = CreateWindowExW(0, L"ComboBoxEx32", 0, 0x50210013, x, y, 170, 250, parent, (HMENU)(INT_PTR)id, 0, 0);
    if (cb.m_hWnd) {
        cb.setFontId(cb.m_fontId);
        SendMessageW(cb.m_hWnd, CB_SETEXTENDEDUI, 1, 0);
        cb.m_flags = 1;
        SetWindowSubclass(cb.m_hWnd, ComboBox::ownerDrawSubclassProc, 0x636D626F , (DWORD_PTR)&cb);
    } else {
        GetLastError();
    }
}

static HWND makeButton(Button& b, const wchar_t* text, DWORD style, int x, int y, int cx, int cy,
                         HWND parent, int id)
{
    b.m_hWnd = CreateWindowExW(0, L"Button", text, style, x, y, cx, cy, parent, (HMENU)(INT_PTR)id, 0, 0);
    if (b.m_hWnd)
        b.setFontId(b.m_fontId);
    else
        GetLastError();
    return b.m_hWnd;
}

static HWND makeNumber(EditText& e, HWND parent, int id, int x, int y, int cx, int cy)
{
    return e.createNumber(parent, (HMENU)(INT_PTR)id, x, y, cx, cy, 0, 0, 0);
}

static int rsInt(RecordSet& rs, const wchar_t* column)
{
    return rs.m_rs ? rs.m_rs->getIterator()->getInt(column) : 0;
}

#define ME_RESIST_STATE(i)   FIELD(Button,   this, 0x334 + 0x0C * (i))
#define ME_RESIST_SKILL(i)   FIELD(EditText, this, 0x3E8 + 0x0C * (i))
#define ME_ATTACK_OK(i)      FIELD(ComboBox, this, 0x46C + 0x28 * (i))
#define ME_ATTACK(i)         FIELD(EditText, this, 0x4E4 + 0x0C * (i))
#define ME_ATTACK_PLUS(i)    FIELD(EditText, this, 0x508 + 0x0C * (i))
#define ME_ATTACK_TIME(i)    FIELD(EditText, this, 0x52C + 0x0C * (i))
#define ME_ATTACK_RANGE(i)   FIELD(EditText, this, 0x550 + 0x0C * (i))
#define ME_ATTACK_TYPE(i)    FIELD(ComboBox, this, 0x574 + 0x28 * (i))
#define ME_ATTACK_SPECIAL(i) FIELD(EditText, this, 0x5EC + 0x0C * (i))
#define ME_ATTACK_ATTRIB(i)  FIELD(ComboBox, this, 0x610 + 0x28 * (i))

bool MonsterEditor::onCreate()
{
    EditText& nameEdit     = FIELD(EditText, this, 0x1FC);
    EditText& levelEdit    = FIELD(EditText, this, 0x208);
    EditText& aiEdit       = FIELD(EditText, this, 0x214);
    EditText& expEdit      = FIELD(EditText, this, 0x220);
    EditText& money1Edit   = FIELD(EditText, this, 0x22C);
    EditText& money2Edit   = FIELD(EditText, this, 0x238);
    EditText& questEdit    = FIELD(EditText, this, 0x244);
    EditText& hpEdit       = FIELD(EditText, this, 0x250);
    EditText& spEdit       = FIELD(EditText, this, 0x25C);
    EditText& mpEdit       = FIELD(EditText, this, 0x268);
    EditText& dexEdit      = FIELD(EditText, this, 0x274);
    EditText& wisEdit      = FIELD(EditText, this, 0x280);
    EditText& lucEdit      = FIELD(EditText, this, 0x28C);
    EditText& defenseEdit  = FIELD(EditText, this, 0x298);
    EditText& magicEdit    = FIELD(EditText, this, 0x2A4);
    EditText& sizeEdit     = FIELD(EditText, this, 0x2B0);
    ComboBox& modelCombo   = FIELD(ComboBox, this, 0x2BC);
    ComboBox& attribCombo  = FIELD(ComboBox, this, 0x2E4);
    ComboBox& dayCombo     = FIELD(ComboBox, this, 0x30C);
    EditText& normalTime   = FIELD(EditText, this, 0x430);
    EditText& normalStep   = FIELD(EditText, this, 0x43C);
    EditText& chaseTime    = FIELD(EditText, this, 0x448);
    EditText& chaseStep    = FIELD(EditText, this, 0x454);
    EditText& chaseRange   = FIELD(EditText, this, 0x460);
    Container& dropPanel   = FIELD(Container, this, 0x688);
    Button&   dropsButton  = FIELD(Button, this, 0x6F0);
    BagsList& bagsList     = FIELD(BagsList, this, 0x700);
    EditText& gradeEdit    = FIELD(EditText, this, 0x810);
    EditText& rateEdit     = FIELD(EditText, this, 0x81C);
    Button&   setButton    = FIELD(Button, this, 0x828);
    Button&   clearButton  = FIELD(Button, this, 0x834);
    MonsterEditor::ItemsList& itemsList = FIELD(MonsterEditor::ItemsList, this, 0x840);
    Button&   modelButton  = FIELD(Button, this, 0x95C);
    BmpCtrl&  preview      = FIELD(BmpCtrl, this, 0x968);

    wchar_t number[32];
    wchar_t text[130];
    HWND label;

    m_highlightColor = 0xFF0000;

    createStatic(20, 22, 40, 16, L"Name:", 0, 4);
    if (nameEdit.create(m_hWnd, (HMENU)100, 60, 20, 245, 18, L"", 0, 0))
        SetWindowSubclass(nameEdit.m_hWnd, tabSubclassProc, 0x65646974 , (DWORD_PTR)&nameEdit);
    SendMessageW(nameEdit.m_hWnd, EM_LIMITTEXT, 40, 0);

    createStatic(20, 49, 40, 16, L"Model:", 0, 4);
    if (makeCombo(modelCombo, 0, L"ComboBoxEx32", (modelCombo.m_dropLines & 3) | 0x50010000,
                    60, 44, 210, 250, m_hWnd, 101)) {
        SendMessageW(modelCombo.m_hWnd, CB_SETEXTENDEDUI, 1, 0);
        modelCombo.m_flags = 1;
    }
    MonsterModels* models = g_studioFrame->m_client.m_clientData.getMonsterModels(L"monsters");
    if (models) {
        ptrArray<MonsterModel*>& list = *models;
        int count = list.size();
        for (int i = 0; i < count; ++i) {
            MonsterModel* model = (i < 0 || i >= list.m_count) ? 0 : list.m_items[i];
            swprintf_s(text, 128, L"%i (%s)", i, FIELD(const wchar_t*, model, 0x18));
            modelCombo.addItem(text, FIELD(int, model, 0x10));
        }
    }
    SetWindowPos(modelCombo.m_hWnd, 0, 0, 0, 210, 250, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    makeButton(modelButton, L">>", 0x50000000, 275, 45, 30, 20, m_hWnd, 183);

    createStatic(340, 22, 40, 16, L"Level:", 0, 4);
    makeNumber(levelEdit, m_hWnd, 102, 380, 20, 50, 18);
    createStatic(340, 48, 30, 16, L"AI:", 0, 4);
    makeNumber(aiEdit, m_hWnd, 103, 380, 46, 50, 18);

    createStatic(20, 76, 120, 16, L"Characteristics", 0, 5);
    createStatic(20, 94, 30, 16, L"HP:", 0, 4);
    makeNumber(hpEdit, m_hWnd, 104, 60, 92, 70, 18);

    label = createStatic(140, 94, 30, 16, L"SP:", 0, 4);
    m_highlighted.add(label);
    m_highlighted.add(makeNumber(spEdit, m_hWnd, 105, 180, 92, 50, 18));
    label = createStatic(240, 94, 30, 16, L"MP:", 0, 4);
    m_highlighted.add(label);
    m_highlighted.add(makeNumber(mpEdit, m_hWnd, 106, 280, 92, 50, 18));
    label = createStatic(20, 118, 30, 16, L"Dex:", 0, 4);
    m_highlighted.add(label);
    m_highlighted.add(makeNumber(dexEdit, m_hWnd, 107, 60, 116, 50, 18));
    label = createStatic(140, 118, 30, 16, L"Wis:", 0, 4);
    m_highlighted.add(label);
    m_highlighted.add(makeNumber(wisEdit, m_hWnd, 108, 180, 116, 50, 18));
    label = createStatic(240, 118, 30, 16, L"Luc:", 0, 4);
    m_highlighted.add(label);
    m_highlighted.add(makeNumber(lucEdit, m_hWnd, 109, 280, 116, 50, 18));
    label = createStatic(20, 142, 40, 16, L"Def:", 0, 4);
    m_highlighted.add(label);
    m_highlighted.add(makeNumber(defenseEdit, m_hWnd, 110, 60, 140, 50, 18));
    label = createStatic(140, 142, 40, 16, L"Resist:", 0, 4);
    m_highlighted.add(label);
    m_highlighted.add(makeNumber(magicEdit, m_hWnd, 111, 180, 140, 50, 18));

    createStatic(240, 142, 40, 16, L"Elem:", 0, 4);
    makeDropList(attribCombo, 280, 138, 55, 200, m_hWnd, 112);
    SendMessageW(attribCombo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"None");
    SendMessageW(attribCombo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Fire");
    SendMessageW(attribCombo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Water");
    SendMessageW(attribCombo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Earth");
    SendMessageW(attribCombo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Wind");

    createStatic(20, 168, 40, 16, L"Height:", 0, 4);
    makeDropList(dayCombo, 60, 164, 50, 200, m_hWnd, 113);
    for (int j = 0; j <= 9; ++j) {
        swprintf_s(text, 128, L"- %d -", j);
        if (SendMessageW(dayCombo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)text) != CB_ERR) {
            LRESULT n = SendMessageW(dayCombo.m_hWnd, CB_GETCOUNT, 0, 0);
            if ((int)n >= 0) {
                StringTable::Entry* entry = new StringTable::Entry((long)n, text);
                dayCombo.m_items.add(entry);
            }
        }
    }

    createStatic(140, 168, 40, 16, L"Radius:", 0, 4);
    makeNumber(sizeEdit, m_hWnd, 114, 180, 166, 50, 18);

    createStatic(380, 76, 50, 16, L"Normal", 1, 4);
    createStatic(440, 76, 50, 16, L"Chase", 1, 4);
    createStatic(340, 94, 40, 16, L"Delay:", 0, 4);
    makeNumber(normalTime, m_hWnd, 115, 380, 92, 50, 18);
    makeNumber(chaseTime, m_hWnd, 116, 440, 92, 50, 18);
    createStatic(340, 118, 40, 16, L"Step:", 0, 4);
    makeNumber(normalStep, m_hWnd, 117, 380, 116, 50, 18);
    makeNumber(chaseStep, m_hWnd, 118, 440, 116, 50, 18);
    label = createStatic(350, 142, 80, 16, L"Vision:", 2, 4);
    m_highlighted.add(label);
    m_highlighted.add(makeNumber(chaseRange, m_hWnd, 119, 440, 140, 50, 18));
    createStatic(350, 166, 80, 16, L"Quest Item:", 2, 4);
    makeNumber(questEdit, m_hWnd, 120, 440, 164, 50, 18);

    createStatic(20, 194, 120, 16, L"Attacks", 0, 5);
    createStatic(35, 210, 40, 16, L"Mode", 0, 4);
    createStatic(85, 210, 40, 16, L"Damage", 0, 4);
    createStatic(140, 210, 50, 16, L"Addit.", 0, 4);
    createStatic(185, 210, 30, 16, L"Delay", 0, 4);
    createStatic(235, 210, 30, 16, L"Range", 0, 4);
    createStatic(280, 210, 90, 16, L"Attack Type", 0, 4);
    createStatic(450, 210, 30, 16, L"Param.", 0, 4);
    createStatic(495, 210, 75, 16, L"Element", 0, 4);

    createStatic(20, 230, 10, 16, L"1", 0, 4);
    makeDropList(ME_ATTACK_OK(0), 30, 226, 45, 14, m_hWnd, 121);
    m_highlighted.add(makeNumber(ME_ATTACK(0), m_hWnd, 122, 80, 228, 50, 18));
    m_highlighted.add(makeNumber(ME_ATTACK_PLUS(0), m_hWnd, 123, 135, 228, 40, 18));
    m_highlighted.add(makeNumber(ME_ATTACK_TIME(0), m_hWnd, 124, 180, 228, 45, 18));
    m_highlighted.add(makeNumber(ME_ATTACK_RANGE(0), m_hWnd, 125, 230, 228, 40, 18));
    makeAttackTypeCombo(ME_ATTACK_TYPE(0), 275, 225, m_hWnd, 126);
    m_highlighted.add(makeNumber(ME_ATTACK_SPECIAL(0), m_hWnd, 127, 450, 228, 35, 18));
    makeDropList(ME_ATTACK_ATTRIB(0), 490, 226, 60, 90, m_hWnd, 128);
    m_highlighted.add(ME_ATTACK_ATTRIB(0).m_hWnd);

    createStatic(20, 258, 10, 16, L"2", 0, 4);
    makeDropList(ME_ATTACK_OK(1), 30, 254, 45, 100, m_hWnd, 129);
    m_highlighted.add(ME_ATTACK_OK(1).m_hWnd);
    m_highlighted.add(makeNumber(ME_ATTACK(1), m_hWnd, 130, 80, 256, 50, 18));
    m_highlighted.add(makeNumber(ME_ATTACK_PLUS(1), m_hWnd, 131, 135, 256, 40, 18));
    m_highlighted.add(makeNumber(ME_ATTACK_TIME(1), m_hWnd, 132, 180, 256, 45, 18));
    makeNumber(ME_ATTACK_RANGE(1), m_hWnd, 133, 230, 256, 40, 18);
    makeAttackTypeCombo(ME_ATTACK_TYPE(1), 275, 253, m_hWnd, 134);
    m_highlighted.add(makeNumber(ME_ATTACK_SPECIAL(1), m_hWnd, 135, 450, 256, 35, 18));
    makeDropList(ME_ATTACK_ATTRIB(1), 490, 254, 60, 90, m_hWnd, 136);
    m_highlighted.add(ME_ATTACK_ATTRIB(1).m_hWnd);

    createStatic(20, 286, 10, 16, L"3", 0, 4);
    makeDropList(ME_ATTACK_OK(2), 30, 282, 45, 100, m_hWnd, 137);
    m_highlighted.add(makeNumber(ME_ATTACK(2), m_hWnd, 138, 80, 284, 50, 18));
    makeNumber(ME_ATTACK_PLUS(2), m_hWnd, 139, 135, 284, 40, 18);
    m_highlighted.add(makeNumber(ME_ATTACK_TIME(2), m_hWnd, 140, 180, 284, 45, 18));
    m_highlighted.add(makeNumber(ME_ATTACK_RANGE(2), m_hWnd, 141, 230, 284, 40, 18));
    makeAttackTypeCombo(ME_ATTACK_TYPE(2), 275, 281, m_hWnd, 142);
    m_highlighted.add(makeNumber(ME_ATTACK_SPECIAL(2), m_hWnd, 143, 450, 284, 35, 18));
    makeDropList(ME_ATTACK_ATTRIB(2), 490, 282, 60, 90, m_hWnd, 144);
    m_highlighted.add(ME_ATTACK_ATTRIB(2).m_hWnd);

    SendMessageW(ME_ATTACK_OK(0).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Off");
    SendMessageW(ME_ATTACK_OK(0).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"On");
    SendMessageW(ME_ATTACK_OK(1).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Off");
    SendMessageW(ME_ATTACK_OK(1).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"On");
    SendMessageW(ME_ATTACK_OK(1).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"2");
    SendMessageW(ME_ATTACK_OK(1).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"3");
    SendMessageW(ME_ATTACK_OK(1).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"4");
    SendMessageW(ME_ATTACK_OK(1).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"5");
    SendMessageW(ME_ATTACK_OK(1).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"6");
    SendMessageW(ME_ATTACK_OK(2).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"0");
    SendMessageW(ME_ATTACK_OK(2).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"1");
    SendMessageW(ME_ATTACK_OK(2).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"2");
    SendMessageW(ME_ATTACK_OK(2).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"3");
    SendMessageW(ME_ATTACK_OK(2).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"4");
    SendMessageW(ME_ATTACK_OK(2).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"5");
    SendMessageW(ME_ATTACK_OK(2).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"6");
    for (int k = 0; k < 2; ++k) {
        HWND attrib = ME_ATTACK_ATTRIB(k).m_hWnd;
        SendMessageW(attrib, CB_ADDSTRING, 0, (LPARAM)L"None");
        SendMessageW(attrib, CB_ADDSTRING, 0, (LPARAM)L"Fire");
        SendMessageW(attrib, CB_ADDSTRING, 0, (LPARAM)L"Water");
        SendMessageW(attrib, CB_ADDSTRING, 0, (LPARAM)L"Earth");
        SendMessageW(attrib, CB_ADDSTRING, 0, (LPARAM)L"Wind");
        SendMessageW(attrib, CB_ADDSTRING, 0, (LPARAM)L"Fire 2");
        SendMessageW(attrib, CB_ADDSTRING, 0, (LPARAM)L"Water 2");
        SendMessageW(attrib, CB_ADDSTRING, 0, (LPARAM)L"Earth 2");
        SendMessageW(attrib, CB_ADDSTRING, 0, (LPARAM)L"Wind 2");
    }
    SendMessageW(ME_ATTACK_ATTRIB(2).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"All");
    SendMessageW(ME_ATTACK_ATTRIB(2).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Light");
    SendMessageW(ME_ATTACK_ATTRIB(2).m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Dark");

    HFONT listFont = getStdFont(6);
    ME_ATTACK_TYPE(0).setFont(listFont);
    ME_ATTACK_TYPE(1).setFont(listFont);
    ME_ATTACK_TYPE(2).setFont(listFont);
    MonsterEditor::fillNpcSkillCombo(&ME_ATTACK_TYPE(0));
    MonsterEditor::fillNpcSkillCombo(&ME_ATTACK_TYPE(1));
    MonsterEditor::fillNpcSkillCombo(&ME_ATTACK_TYPE(2));

    createStatic(20, 320, 120, 16, L"Resistance to debuffs:", 0, 5);
    int index = 0;
    int y = 336;
    for (int x = 20; x < 260; x += 80) {
        y = 336;
        for (int n = 5; n != 0; --n) {
            Button& box = ME_RESIST_STATE(index);
            makeButton(box, g_debuffNames[index], 0x50010002, x, y, 70, 18, m_hWnd, index + 150);
            m_highlighted.add(box.m_hWnd);
            y += 20;
            ++index;
        }
    }
    int lootsY = y + 8;

    createStatic(290, 320, 120, 16, L"Resistance to spells:", 0, 5);
    int spellY = 336;
    int spellId = 170;
    int spellNo = 1;
    EditText* spellEdit = &ME_RESIST_SKILL(0);
    for (int n = 3; n != 0; --n) {
        swprintf_s(text, 128, L"#%d", spellNo);
        label = createStatic(290, spellY + 3, 20, 16, text, 0, 4);
        m_highlighted.add(label);
        m_highlighted.add(spellEdit[0].createNumber(m_hWnd, (HMENU)(INT_PTR)spellId, 310, spellY, 40, 18, 0, 0, 0));
        swprintf_s(text, 128, L"#%d", spellNo + 1);
        label = createStatic(360, spellY + 3, 20, 16, text, 0, 4);
        m_highlighted.add(label);
        m_highlighted.add(spellEdit[1].createNumber(m_hWnd, (HMENU)(INT_PTR)(spellId + 1), 380, spellY, 40, 18, 0, 0, 0));
        spellId += 2;
        spellEdit += 2;
        spellY += 20;
        spellNo += 2;
    }

    createStatic(20, lootsY, 40, 16, L"Loots:", 0, 5);
    label = createStatic(20, lootsY + 18, 40, 16, L"XP:", 0, 4);
    m_highlighted.add(label);
    m_highlighted.add(expEdit.createNumber(m_hWnd, (HMENU)180, 60, lootsY + 16, 60, 18, 0, 0, 0));
    label = createStatic(140, lootsY + 18, 30, 16, L"Gold:", 0, 4);
    m_highlighted.add(label);
    m_highlighted.add(money1Edit.createNumber(m_hWnd, (HMENU)181, 180, lootsY + 16, 60, 18, 0, 0, 0));
    label = createStatic(250, lootsY + 18, 25, 16, L"up to", 0, 4);
    m_highlighted.add(label);
    m_highlighted.add(money2Edit.createNumber(m_hWnd, (HMENU)182, 280, lootsY + 16, 60, 18, 0, 0, 0));
    makeButton(dropsButton, L"Drops >>", 0x50000000, 360, lootsY + 14, 70, 22, m_hWnd, 184);

    dropPanel.m_bkColor = Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0;
    dropPanel.createChildAt(m_hWnd, (HMENU)1001, 600, 28, 260, 470, WS_CHILD, 0, 0);
    dropPanel.createStatic(0, 0, 80, 16, L"Drop Bags List", 0, 4);
    int bagsHeight = dword_5A0124 + dword_5A0120 + 4 + 8 * dword_5A0124;
    bagsList.create(dropPanel.m_hWnd, (HMENU)200, 0, 16, 120, bagsHeight, 0x10800000, 0, 0);
    itemsList.create(dropPanel.m_hWnd, (HMENU)205, 0, bagsHeight + 26, 250, 450 - (bagsHeight + 10),
                         0x10800000, 0, 0);
    dropPanel.createStatic(130, 18, 35, 14, L"Grade:", 0, 4);
    gradeEdit.createNumber(dropPanel.m_hWnd, (HMENU)201, 170, 16, 50, 18, 0, 0, 0);
    dropPanel.createStatic(130, 45, 35, 14, L"Rate:", 0, 4);
    if (rateEdit.create(dropPanel.m_hWnd, (HMENU)202, 170, 43, 50, 18, L"", 0, 0)) {
        SetWindowSubclass(rateEdit.m_hWnd, decimalEditSubclassProc, 0x65646974 , (DWORD_PTR)&rateEdit);
        HWND rateWnd = rateEdit.m_hWnd;
        swprintf_s(number, 32, L"%.4f", 0.0);
        SendMessageW(rateWnd, WM_SETTEXT, 0, (LPARAM)number);
    }
    dropPanel.createStatic(222, 45, 10, 14, L"%", 0, 4);
    makeButton(setButton, L"Set", 0x50000000, 130, 73, 45, 22, dropPanel.m_hWnd, 203);
    makeButton(clearButton, L"Clear", 0x50000000, 180, 73, 45, 22, dropPanel.m_hWnd, 204);

    RECT rc;
    preview.m_bkColor = Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0;
    preview.m_bitmapSize.cx = 300;
    preview.m_bitmapSize.cy = 450;
    SetRect(&rc, 600, 28, 900, 478);
    preview.create(m_hWnd, (HMENU)1002, &rc, 0x50000000, 0, 0);

    bool result = Modal::onCreate();
    if (result) {
        loadMonster();
        return true;
    }
    return result;
}

void MonsterEditor::loadMonster()
{
    EditText& nameEdit     = FIELD(EditText, this, 0x1FC);
    EditText& levelEdit    = FIELD(EditText, this, 0x208);
    EditText& aiEdit       = FIELD(EditText, this, 0x214);
    EditText& expEdit      = FIELD(EditText, this, 0x220);
    EditText& money1Edit   = FIELD(EditText, this, 0x22C);
    EditText& money2Edit   = FIELD(EditText, this, 0x238);
    EditText& questEdit    = FIELD(EditText, this, 0x244);
    EditText& hpEdit       = FIELD(EditText, this, 0x250);
    EditText& spEdit       = FIELD(EditText, this, 0x25C);
    EditText& mpEdit       = FIELD(EditText, this, 0x268);
    EditText& dexEdit      = FIELD(EditText, this, 0x274);
    EditText& wisEdit      = FIELD(EditText, this, 0x280);
    EditText& lucEdit      = FIELD(EditText, this, 0x28C);
    EditText& defenseEdit  = FIELD(EditText, this, 0x298);
    EditText& magicEdit    = FIELD(EditText, this, 0x2A4);
    EditText& sizeEdit     = FIELD(EditText, this, 0x2B0);
    ComboBox& modelCombo   = FIELD(ComboBox, this, 0x2BC);
    ComboBox& attribCombo  = FIELD(ComboBox, this, 0x2E4);
    ComboBox& dayCombo     = FIELD(ComboBox, this, 0x30C);
    EditText& normalTime   = FIELD(EditText, this, 0x430);
    EditText& normalStep   = FIELD(EditText, this, 0x43C);
    EditText& chaseTime    = FIELD(EditText, this, 0x448);
    EditText& chaseStep    = FIELD(EditText, this, 0x454);
    EditText& chaseRange   = FIELD(EditText, this, 0x460);
    BagsList& bagsList     = FIELD(BagsList, this, 0x700);
    BagsList::Items& bags  = bagsList.m_items;
#define MOB FIELD(Monster*, this, 0xAC)

    SqlServer* server = (SqlServer*)g_studioFrame->getSqlConnection();

    nameEdit.setFont(getStdFont(6));
    nameEdit.setText(FIELD(const wchar_t*, MOB, 0x1C));
    modelCombo.selectData((WPARAM)(int)FIELD(short, MOB, 0x38));
    levelEdit.setInt(FIELD(short, MOB, 0x3C));
    aiEdit.setInt(FIELD(BYTE, MOB, 0x40));
    hpEdit.setInt(FIELD(int, MOB, 0x80));
    SendMessageW(dayCombo.m_hWnd, CB_SETCURSEL, FIELD(BYTE, MOB, 0x8E), 0);
    sizeEdit.setInt(FIELD(BYTE, MOB, 0x8F));
    SendMessageW(attribCombo.m_hWnd, CB_SETCURSEL, FIELD(BYTE, MOB, 0x90), 0);
    normalTime.setInt(FIELD(int, MOB, 0xAC));
    normalStep.setInt(FIELD(BYTE, MOB, 0xB0));
    chaseTime.setInt(FIELD(int, MOB, 0xB4));
    chaseStep.setInt(FIELD(BYTE, MOB, 0xB8));
    questEdit.setInt(FIELD(short, MOB, 0x7C));
    SendMessageW(ME_ATTACK_OK(0).m_hWnd, CB_SETCURSEL, FIELD(BYTE, MOB, 0xCC), 0);
    ME_ATTACK_TYPE(0).selectData((WPARAM)(int)FIELD(short, MOB, 0xBE));
    ME_ATTACK_RANGE(1).setInt(FIELD(BYTE, MOB, 0xD8));
    ME_ATTACK_TYPE(1).selectData((WPARAM)(int)FIELD(short, MOB, 0xD2));
    SendMessageW(ME_ATTACK_OK(2).m_hWnd, CB_SETCURSEL, FIELD(BYTE, MOB, 0xF4), 0);
    ME_ATTACK_PLUS(2).setInt(FIELD(unsigned short, MOB, 0xF0));
    ME_ATTACK_TYPE(2).selectData((WPARAM)(int)FIELD(short, MOB, 0xE6));

    Monster* mob = MOB;
    if (FIELD(BYTE, mob, 0x10)) {
        m_highlightColor = 0x101010;
        bagsList.m_textColor = 0x101010;
    } else {
        if ((FIELD(DWORD, mob, 0x0C) & 0x1000) == 0) {
            if (!server) {
                bagsList.m_textColor = 0x606060;
                m_highlightColor = 0x606060;
                return;
            }
            m_highlightColor = 0x8000;
            bagsList.m_textColor = 0x8000;

            String star;
            star.append(L"* ", (unsigned)-1);
            String sql;
            sql.append(L"Select * from PS_GameDefs.dbo.Mobs where MobID=", (unsigned)-1);
            sql << (long)FIELD(int, MOB, 0x14);
            RecordSet rs = server->query(sql.m_buffer);
            if (!rs.m_rs || !rs.m_rs->getIterator()->next())
                throw 0;

            if (rsInt(rs, L"Level") != FIELD(short, MOB, 0x3C))
                m_textColors.set(levelEdit.m_hWnd, 255);
            if (rsInt(rs, L"AI") != FIELD(BYTE, MOB, 0x40))
                m_textColors.set(aiEdit.m_hWnd, 255);
            if (rsInt(rs, L"HP") != FIELD(int, MOB, 0x80))
                m_textColors.set(hpEdit.m_hWnd, 255);
            spEdit.setInt(rsInt(rs, L"SP"));
            mpEdit.setInt(rsInt(rs, L"MP"));
            dexEdit.setInt(rsInt(rs, L"Dex"));
            wisEdit.setInt(rsInt(rs, L"Wis"));
            lucEdit.setInt(rsInt(rs, L"Luc"));
            if (rsInt(rs, L"Attrib") != FIELD(BYTE, MOB, 0x90))
                m_textColors.set(attribCombo.m_hWnd, 255);
            if (rsInt(rs, L"Day") != FIELD(BYTE, MOB, 0x8E))
                m_textColors.set(dayCombo.m_hWnd, 255);
            if (rsInt(rs, L"Size") != FIELD(BYTE, MOB, 0x8F))
                m_textColors.set(sizeEdit.m_hWnd, 255);
            if (rsInt(rs, L"NormalTime") != FIELD(int, MOB, 0xAC))
                m_textColors.set(normalTime.m_hWnd, 255);
            if (rsInt(rs, L"NormalStep") != FIELD(BYTE, MOB, 0xB0))
                m_textColors.set(normalStep.m_hWnd, 255);
            if (rsInt(rs, L"ChaseTime") != FIELD(int, MOB, 0xB4))
                m_textColors.set(chaseTime.m_hWnd, 255);
            if (rsInt(rs, L"ChaseStep") != FIELD(BYTE, MOB, 0xB8))
                m_textColors.set(chaseStep.m_hWnd, 255);
            if (rsInt(rs, L"QuestItemID") != FIELD(short, MOB, 0x7C))
                m_textColors.set(questEdit.m_hWnd, 255);
            defenseEdit.setInt(rsInt(rs, L"Defense"));
            magicEdit.setInt(rsInt(rs, L"Magic"));
            chaseRange.setInt(rsInt(rs, L"ChaseRange"));
            expEdit.setInt(rsInt(rs, L"Exp"));
            money1Edit.setInt(rsInt(rs, L"Money1"));
            money2Edit.setInt(rsInt(rs, L"Money2"));

            if (rsInt(rs, L"AttackOK1") != FIELD(BYTE, MOB, 0xCC))
                m_textColors.set(ME_ATTACK_OK(0).m_hWnd, 255);
            ME_ATTACK(0).setInt(rsInt(rs, L"Attack1"));
            ME_ATTACK_PLUS(0).setInt(rsInt(rs, L"AttackPlus1"));
            ME_ATTACK_TIME(0).setInt(rsInt(rs, L"AttackTime1"));
            ME_ATTACK_RANGE(0).setInt(rsInt(rs, L"AttackRange1"));
            ME_ATTACK_TYPE(0).selectData((WPARAM)rsInt(rs, L"AttackType1"));
            ME_ATTACK_SPECIAL(0).setInt(rsInt(rs, L"AttackSpecial1"));
            SendMessageW(ME_ATTACK_ATTRIB(0).m_hWnd, CB_SETCURSEL, (WPARAM)rsInt(rs, L"AttackAttrib1"), 0);

            SendMessageW(ME_ATTACK_OK(1).m_hWnd, CB_SETCURSEL, (WPARAM)rsInt(rs, L"AttackOK2"), 0);
            ME_ATTACK(1).setInt(rsInt(rs, L"Attack2"));
            ME_ATTACK_PLUS(1).setInt(rsInt(rs, L"AttackPlus2"));
            ME_ATTACK_TIME(1).setInt(rsInt(rs, L"AttackTime2"));
            if (rsInt(rs, L"AttackRange2") != FIELD(BYTE, MOB, 0xD8)) {
                String both;
                int dbRange = rsInt(rs, L"AttackRange2");
                both << (unsigned char)FIELD(BYTE, MOB, 0xD8) << " / " << (long)dbRange;
                ME_ATTACK_RANGE(1).setText(both.m_buffer);
                m_textColors.set(ME_ATTACK_RANGE(1).m_hWnd, 255);
            }
            ME_ATTACK_TYPE(1).selectData((WPARAM)rsInt(rs, L"AttackType2"));
            ME_ATTACK_SPECIAL(1).setInt(rsInt(rs, L"AttackSpecial2"));
            SendMessageW(ME_ATTACK_ATTRIB(1).m_hWnd, CB_SETCURSEL, (WPARAM)rsInt(rs, L"AttackAttrib2"), 0);

            if (rsInt(rs, L"AttackOK3") != FIELD(BYTE, MOB, 0xF4))
                m_textColors.set(ME_ATTACK_OK(2).m_hWnd, 255);
            ME_ATTACK(2).setInt(rsInt(rs, L"Attack3"));
            if (rsInt(rs, L"AttackPlus3") != FIELD(unsigned short, MOB, 0xF0))
                m_textColors.set(ME_ATTACK_PLUS(2).m_hWnd, 255);
            ME_ATTACK_TIME(2).setInt(rsInt(rs, L"AttackTime3"));
            ME_ATTACK_RANGE(2).setInt(rsInt(rs, L"AttackRange3"));
            ME_ATTACK_TYPE(2).selectData((WPARAM)rsInt(rs, L"AttackType3"));
            ME_ATTACK_SPECIAL(2).setInt(rsInt(rs, L"AttackSpecial3"));
            SendMessageW(ME_ATTACK_ATTRIB(2).m_hWnd, CB_SETCURSEL, (WPARAM)rsInt(rs, L"AttackAttrib3"), 0);

            String prefix(L"ResistState", (unsigned)-1);
            for (int i = 0; i < 15; ++i) {
                String column = sub_41E180(prefix, i + 1);
                int value = rsInt(rs, column.m_buffer);
                ME_RESIST_STATE(i).setCheck(value == 1);
            }
            prefix = L"ResistSkill";
            for (int i = 0; i < 6; ++i) {
                String column = sub_41E180(prefix, i + 1);
                ME_RESIST_SKILL(i).setInt(rsInt(rs, column.m_buffer));
            }

            sql.clear();
            sql << L"SELECT Grade,DropRate FROM PS_GameDefs.dbo.MobItems WHERE MobID=";
            sql << (long)FIELD(int, MOB, 0x14);
            sql << L" ORDER BY ItemOrder";
            rs = server->query(sql.m_buffer);
            if (!rs.m_rs || !rs.m_rs->getIterator()->next())
                throw 0;
            int rows = rs.m_rs ? rs.m_rs->getCount() : 0;
            int limit = 9;
            if (rows > 9)
                limit = rows;
            for (int k = 0; k < limit; ++k) {
                int rate = 0;
                int grade = 0;
                if (rs.m_rs) {
                    rate = rs.m_rs->getIterator()->getIntAt(1);
                    if (rs.m_rs)
                        grade = rs.m_rs->getIterator()->getIntAt(0);
                }
                BagsList::Item* bag = bags.get(k);
                if (bag) {
                    bag->m_grade = (unsigned short)grade;
                    float scale = k >= 6 ? 100000.0f : 1.0f;
                    bag->m_rate = (float)rate / scale;
                }
                if (!rs.m_rs || !rs.m_rs->getIterator()->next())
                    break;
            }
            bagsList.updateAll();
            return;
        }
        m_highlightColor = 0x800000;
        bagsList.m_textColor = 0x800000;
    }

    spEdit.setInt(FIELD(unsigned short, mob, 0x84));
    mpEdit.setInt(FIELD(unsigned short, MOB, 0x86));
    dexEdit.setInt(FIELD(unsigned short, MOB, 0x88));
    wisEdit.setInt(FIELD(unsigned short, MOB, 0x8A));
    lucEdit.setInt(FIELD(unsigned short, MOB, 0x8C));
    defenseEdit.setInt(FIELD(unsigned short, MOB, 0x92));
    magicEdit.setInt(FIELD(unsigned short, MOB, 0x94));
    chaseRange.setInt(FIELD(BYTE, MOB, 0xB9));
    expEdit.setInt(FIELD(unsigned short, MOB, 0x3E));
    money1Edit.setInt(FIELD(unsigned short, MOB, 0x42));
    money2Edit.setInt(FIELD(unsigned short, MOB, 0x44));

    SendMessageW(ME_ATTACK_OK(0).m_hWnd, CB_SETCURSEL, FIELD(BYTE, MOB, 0xCC), 0);
    ME_ATTACK(0).setInt(FIELD(unsigned short, MOB, 0xC6));
    ME_ATTACK_PLUS(0).setInt(FIELD(unsigned short, MOB, 0xC8));
    ME_ATTACK_TIME(0).setInt(FIELD(int, MOB, 0xC0));
    ME_ATTACK_RANGE(0).setInt(FIELD(BYTE, MOB, 0xC4));
    ME_ATTACK_TYPE(0).selectData((WPARAM)(int)FIELD(short, MOB, 0xBE));
    ME_ATTACK_SPECIAL(0).setInt(FIELD(BYTE, MOB, 0xCB));
    SendMessageW(ME_ATTACK_ATTRIB(0).m_hWnd, CB_SETCURSEL, FIELD(BYTE, MOB, 0xCA), 0);

    SendMessageW(ME_ATTACK_OK(1).m_hWnd, CB_SETCURSEL, FIELD(BYTE, MOB, 0xE0), 0);
    ME_ATTACK(1).setInt(FIELD(unsigned short, MOB, 0xDA));
    ME_ATTACK_PLUS(1).setInt(FIELD(unsigned short, MOB, 0xDC));
    ME_ATTACK_TIME(1).setInt(FIELD(int, MOB, 0xD4));
    ME_ATTACK_RANGE(1).setInt(FIELD(BYTE, MOB, 0xD8));
    ME_ATTACK_TYPE(1).selectData((WPARAM)(int)FIELD(short, MOB, 0xD2));
    ME_ATTACK_SPECIAL(1).setInt(FIELD(BYTE, MOB, 0xDF));
    SendMessageW(ME_ATTACK_ATTRIB(1).m_hWnd, CB_SETCURSEL, FIELD(BYTE, MOB, 0xDE), 0);

    SendMessageW(ME_ATTACK_OK(2).m_hWnd, CB_SETCURSEL, FIELD(BYTE, MOB, 0xF4), 0);
    ME_ATTACK(2).setInt(FIELD(unsigned short, MOB, 0xEE));
    ME_ATTACK_PLUS(2).setInt(FIELD(unsigned short, MOB, 0xF0));
    ME_ATTACK_TIME(2).setInt(FIELD(int, MOB, 0xE8));
    ME_ATTACK_RANGE(2).setInt(FIELD(BYTE, MOB, 0xEC));
    ME_ATTACK_TYPE(2).selectData((WPARAM)(int)FIELD(short, MOB, 0xE6));
    ME_ATTACK_SPECIAL(2).setInt(FIELD(BYTE, MOB, 0xF3));
    SendMessageW(ME_ATTACK_ATTRIB(2).m_hWnd, CB_SETCURSEL, FIELD(BYTE, MOB, 0xF2), 0);

    for (int i = 0; i < 15; ++i)
        ME_RESIST_STATE(i).setCheck(FIELD(BYTE, MOB, 0x96 + i) == 1);
    for (int i = 0; i < 6; ++i)
        ME_RESIST_SKILL(i).setInt(FIELD(BYTE, MOB, 0xA5 + i));

    int gradeOffset = 0x46;
    int bag = 0;
    for (int rateOffset = 0x58; rateOffset < 0x7C; rateOffset += 4) {
        Monster* m = MOB;
        int rate = FIELD(int, m, rateOffset);
        unsigned short grade = FIELD(unsigned short, m, gradeOffset);
        BagsList::Item* item = bags.get(bag);
        if (item) {
            item->m_grade = grade;
            float scale = bag >= 6 ? 100000.0f : 1.0f;
            item->m_rate = (float)rate / scale;
        }
        gradeOffset += 2;
        ++bag;
    }
    bagsList.updateAll();
#undef MOB
}

bool MonsterEditor::updatePreview()
{
    ComboBox& modelCombo = FIELD(ComboBox, this, 0x2BC);
    BmpCtrl&  preview    = FIELD(BmpCtrl, this, 0x968);

    LRESULT index;
    if (modelCombo.m_hWnd) {
        LRESULT sel = SendMessageW(modelCombo.m_hWnd, CB_GETCURSEL, 0, 0);
        index = SendMessageW(modelCombo.m_hWnd, CB_GETITEMDATA, sel, 0);
        if (index == -1)
            return false;
    } else {
        index = 0;
    }
    MonsterModels* models = g_studioFrame->m_client.m_clientData.getMonsterModels(L"monsters");
    if (!models)
        return false;
    ptrArray<MonsterModel*>& list = *models;
    if (index < 0 || index >= list.m_count)
        return false;
    MonsterModel* model = list.m_items[index];
    if (!model)
        return false;

    String workspace;
    String base = *g_studioFrame->getWorkspacePath(&workspace) + L"Monster\\";
    MetaModel3D meta;
    ptrArray<String*>& meshes   = FIELD(ptrArray<String*>, model, 0x64);
    ptrArray<String*>& textures = FIELD(ptrArray<String*>, model, 0x90);
    int count = meshes.size();
    bool loaded = true;
    for (int i = 0; i < count; ++i) {
        Struct3D* part;
        {
            String textureDir = base + L"DDS\\";
            String texture = textureDir + ((i >= 0 && i < textures.m_count && textures.m_items[i])
                                               ? textures.m_items[i]->m_buffer : (const wchar_t*)0);
            String meshDir = base + L"3DC\\";
            String mesh = meshDir + ((i >= 0 && i < meshes.m_count && meshes.m_items[i])
                                         ? meshes.m_items[i]->m_buffer : (const wchar_t*)0);
            part = ClientData::loadModel(mesh, texture, 0x6B334443 );
        }
        if (!part) {
            loaded = false;
            break;
        }
        FIELD(MetaStruct3D::Structs3D, &meta, 0x68).add(part);
    }

    bool result;
    if (loaded) {
        COLORREF bk = Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0;
        HBITMAP bitmap = meta.Drawable::makeBitmap(bk, SIZE{ 300, 450 }, 0, 0x6D6F6273 );
        if (bitmap) {
            preview.setBitmap(bitmap);
            result = true;
        } else {
            result = false;
        }
    } else {
        result = false;
    }
    return result;
}

bool MonsterEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    Container& dropPanel   = FIELD(Container, this, 0x688);
    Button&    dropsButton = FIELD(Button, this, 0x6F0);
    bool&      dropsShown  = FIELD(bool, this, 0x6FC);
    BagsList&  bagsList    = FIELD(BagsList, this, 0x700);
    EditText&  gradeEdit   = FIELD(EditText, this, 0x810);
    EditText&  rateEdit    = FIELD(EditText, this, 0x81C);
    Button&    modelButton = FIELD(Button, this, 0x95C);
    BmpCtrl&   preview     = FIELD(BmpCtrl, this, 0x968);
    bool&      previewShown = FIELD(bool, this, 0x9E0);

    if (id == 184) {
        bool show = !dropsShown;
        bool hadPreview = previewShown;
        dropsShown = show;
        if (hadPreview)
            previewShown = false;
        int extra = 0;
        if (show)
            extra = 300;
        SetWindowPos(m_hWnd, 0, 0, 0, extra + 580, 575, SWP_NOMOVE | SWP_NOZORDER);
        SetWindowTextW(dropsButton.m_hWnd, dropsShown ? L"Drops <<" : L"Drops >>");
        SetWindowTextW(modelButton.m_hWnd, previewShown ? L"<<" : L">>");
        dropPanel.show(dropsShown);
        preview.show(false);
        return true;
    }
    if (id == 183) {
        bool show = !previewShown;
        bool hadDrops = dropsShown;
        previewShown = show;
        if (hadDrops)
            dropsShown = false;
        if (show)
            previewShown = updatePreview();
        int extra = 0;
        if (previewShown)
            extra = 300;
        SetWindowPos(m_hWnd, 0, 0, 0, extra + 580, 575, SWP_NOMOVE | SWP_NOZORDER);
        SetWindowTextW(dropsButton.m_hWnd, dropsShown ? L"Drops <<" : L"Drops >>");
        SetWindowTextW(modelButton.m_hWnd, previewShown ? L"<<" : L">>");
        dropPanel.show(false);
        preview.show(previewShown);
        return true;
    }
    if (id == 101) {
        if (code == CBN_SELCHANGE) {
            updatePreview();
            return true;
        }
    } else {
        if (id - 150 <= 14) {
            Button& box = ME_RESIST_STATE(id - 150);
            box.setCheck(box.isChecked() == 0);
            return true;
        }
        if (id == 203) {
            float rate = rateEdit.getFloat();
            int grade = gradeEdit.getInt(0);
            bagsList.setSelectedBag(grade, rate);
            return true;
        }
        if (id == 204) {
            bagsList.setSelectedBag(0, 0.0f);
            return true;
        }
    }
    return Modal::onCommand(code, ctrl, id);
}
