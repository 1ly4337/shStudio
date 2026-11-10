#include <windows.h>
#include <cwchar>
#include "ida_compat.h"
#include "MonsterEditor.h"
#include "String.hpp"
#include "EditText.h"
#include "ComboBox.h"
#include "Button.h"
#include "BagsList.h"
#include "_imports_C2.h"

class Monster;

static String& String_putChar(String& s, wchar_t c)
{
    if (s.String::reserve(FIELD(unsigned, &s, 0x08) + 1)) {
        FIELD(wchar_t*, &s, 0x04)[FIELD(unsigned, &s, 0x08)++] = c;
        FIELD(wchar_t*, &s, 0x04)[FIELD(unsigned, &s, 0x08)] = 0;
    }
    return s;
}

static String& String_putString(String& s, const String& other)
{
    return s.String::append(FIELD(const wchar_t*, &other, 0x04), FIELD(unsigned, &other, 0x08));
}

static void String_clear(String& s)
{
    FIELD(unsigned, &s, 0x08) = 0;
    FIELD(unsigned, &s, 0x0C) = 0;
    if (FIELD(wchar_t*, &s, 0x04)) {
        delete[] FIELD(wchar_t*, &s, 0x04);
        FIELD(wchar_t*, &s, 0x04) = 0;
    }
}

static short ComboBox_curData(ComboBox& cb)
{
    HWND hWnd = cb.m_hWnd;
    if (!hWnd)
        return 0;
    LRESULT sel = SendMessageW(hWnd, CB_GETCURSEL, 0, 0);
    return (short)SendMessageW(cb.m_hWnd, CB_GETITEMDATA, sel, 0);
}

static LRESULT ComboBox_curSel(ComboBox& cb)
{
    return SendMessageW(cb.m_hWnd, CB_GETCURSEL, 0, 0);
}

int MonsterEditor::mapCommandToResult(int code)
{
    if (code != 1)
        return code;

    ComboBox& model = FIELD(ComboBox, this, 0x2BC);
    short modelId = ComboBox_curData(model);
    if (modelId == -1)
        return -1;

    Monster* mob = FIELD(Monster*, this, 0xAC);

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
    ComboBox& attribCombo  = FIELD(ComboBox, this, 0x2E4);
    ComboBox& dayCombo     = FIELD(ComboBox, this, 0x30C);
    EditText& normalTime   = FIELD(EditText, this, 0x430);
    EditText& normalStep   = FIELD(EditText, this, 0x43C);
    EditText& chaseTime    = FIELD(EditText, this, 0x448);
    EditText& chaseStep    = FIELD(EditText, this, 0x454);
    EditText& chaseRange   = FIELD(EditText, this, 0x460);

#define RESIST_STATE(i)   FIELD(Button,   this, 0x334 + 0x0C * (i))
#define RESIST_SKILL(i)   FIELD(EditText, this, 0x3E8 + 0x0C * (i))
#define ATTACK_OK(i)      FIELD(ComboBox, this, 0x46C + 0x28 * (i))
#define ATTACK(i)         FIELD(EditText, this, 0x4E4 + 0x0C * (i))
#define ATTACK_PLUS(i)    FIELD(EditText, this, 0x508 + 0x0C * (i))
#define ATTACK_TIME(i)    FIELD(EditText, this, 0x52C + 0x0C * (i))
#define ATTACK_RANGE(i)   FIELD(EditText, this, 0x550 + 0x0C * (i))
#define ATTACK_TYPE(i)    FIELD(ComboBox, this, 0x574 + 0x28 * (i))
#define ATTACK_SPECIAL(i) FIELD(EditText, this, 0x5EC + 0x0C * (i))
#define ATTACK_ATTRIB(i)  FIELD(ComboBox, this, 0x610 + 0x28 * (i))
#define MOB(T, off)       FIELD(T, mob, off)

    String fields;

    MOB(short, 0x38) = modelId;
    {
        String text;
        MOB(String, 0x18).String::operator=(*nameEdit.getText(&text));
    }
    {
        String quoted = String::sqlQuote(MOB(const wchar_t*, 0x1C));
        fields.String::operator<<(L"MobName=").String::append(FIELD(const wchar_t*, &quoted, 0x04),
                                                 FIELD(unsigned, &quoted, 0x08));
    }

    MOB(short, 0x3C) = (short)levelEdit.EditText::getInt(MOB(short, 0x3C));
    fields.String::operator<<(L",Level=").String::operator<<(MOB(short, 0x3C));
    MOB(unsigned char, 0x40) = (unsigned char)aiEdit.EditText::getInt(MOB(unsigned char, 0x40));
    fields.String::operator<<(L",AI=").String::operator<<(MOB(unsigned char, 0x40));
    MOB(int, 0x80) = (int)hpEdit.EditText::getInt(MOB(int, 0x80));
    fields.String::operator<<(L",HP=").String::operator<<(MOB(int, 0x80));
    MOB(unsigned char, 0x90) = (unsigned char)ComboBox_curSel(attribCombo);
    fields.String::operator<<(L",Attrib=").String::operator<<(MOB(unsigned char, 0x90));
    MOB(unsigned char, 0x8E) = (unsigned char)ComboBox_curSel(dayCombo);
    fields.String::operator<<(L",Day=").String::operator<<(MOB(unsigned char, 0x8E));
    MOB(unsigned char, 0x8F) = (unsigned char)sizeEdit.EditText::getInt(MOB(unsigned char, 0x8F));
    fields.String::operator<<(L",Size=").String::operator<<(MOB(unsigned char, 0x8F));
    MOB(int, 0xAC) = (int)normalTime.EditText::getInt(MOB(int, 0xAC));
    fields.String::operator<<(L",NormalTime=").String::operator<<(MOB(int, 0xAC));
    MOB(unsigned char, 0xB0) = (unsigned char)normalStep.EditText::getInt(MOB(unsigned char, 0xB0));
    fields.String::operator<<(L",NormalStep=").String::operator<<(MOB(unsigned char, 0xB0));
    MOB(int, 0xB4) = (int)chaseTime.EditText::getInt(MOB(int, 0xB4));
    fields.String::operator<<(L",ChaseTime=").String::operator<<(MOB(int, 0xB4));
    MOB(unsigned char, 0xB8) = (unsigned char)chaseStep.EditText::getInt(MOB(unsigned char, 0xB8));
    fields.String::operator<<(L",ChaseStep=").String::operator<<(MOB(unsigned char, 0xB8));
    MOB(short, 0x7C) = (short)questEdit.EditText::getInt(MOB(short, 0x7C));
    fields.String::operator<<(L",QuestItemID=").String::operator<<(MOB(short, 0x7C));

    if (MOB(unsigned char, 0x10)) {
        MOB(short, 0x84) = (short)spEdit.EditText::getInt(0);
        MOB(short, 0x86) = (short)mpEdit.EditText::getInt(0);
        MOB(short, 0x88) = (short)dexEdit.EditText::getInt(0);
        MOB(short, 0x8A) = (short)wisEdit.EditText::getInt(0);
        MOB(short, 0x8C) = (short)lucEdit.EditText::getInt(0);
        MOB(short, 0x92) = (short)defenseEdit.EditText::getInt(0);
        MOB(short, 0x94) = (short)magicEdit.EditText::getInt(0);
        MOB(unsigned char, 0xB9) = (unsigned char)chaseRange.EditText::getInt(0);
        MOB(short, 0x3E) = (short)expEdit.EditText::getInt(0);
        MOB(short, 0x42) = (short)money1Edit.EditText::getInt(0);
        MOB(short, 0x44) = (short)money2Edit.EditText::getInt(0);
    }
    {
        short money2  = (short)money2Edit.EditText::getInt(0);
        short money1  = (short)money1Edit.EditText::getInt(0);
        short exp     = (short)expEdit.EditText::getInt(0);
        int   range   = (int)chaseRange.EditText::getInt(0);
        short magic   = (short)magicEdit.EditText::getInt(0);
        short defense = (short)defenseEdit.EditText::getInt(0);
        short luc     = (short)lucEdit.EditText::getInt(0);
        short wis     = (short)wisEdit.EditText::getInt(0);
        short dex     = (short)dexEdit.EditText::getInt(0);
        short mp      = (short)mpEdit.EditText::getInt(0);
        short sp      = (short)spEdit.EditText::getInt(0);
        fields.String::operator<<(L",SP=").String::operator<<(sp)
              .String::operator<<(L",MP=").String::operator<<(mp)
              .String::operator<<(L",Dex=").String::operator<<(dex)
              .String::operator<<(L",Wis=").String::operator<<(wis)
              .String::operator<<(L",Luc=").String::operator<<(luc)
              .String::operator<<(L",Defense=").String::operator<<(defense)
              .String::operator<<(L",Magic=").String::operator<<(magic)
              .String::operator<<(L",ChaseRange=").String::operator<<(range)
              .String::operator<<(L",Exp=").String::operator<<(exp)
              .String::operator<<(L",Money1=").String::operator<<(money1)
              .String::operator<<(L",Money2=").String::operator<<(money2);
    }

    if (MOB(unsigned char, 0x10)) {
        for (int i = 0; i < 15; ++i)
            MOB(unsigned char, 0x96 + i) = RESIST_STATE(i).isChecked() != 0;
        for (int i = 0; i < 6; ++i)
            MOB(unsigned char, 0xA5 + i) = (unsigned char)RESIST_SKILL(i).EditText::getInt(0);
    }
    for (int i = 0; i < 15; ) {
        String& s = fields.String::operator<<(L",ResistState").String::operator<<(++i);
        String_putChar(s, L'=');
        s.String::operator<<(RESIST_STATE(i - 1).isChecked() != 0);
    }
    for (int i = 0; i < 6; ) {
        int value = (int)RESIST_SKILL(i).EditText::getInt(0);
        String& s = fields.String::operator<<(L",ResistSkill").String::operator<<(++i);
        String_putChar(s, L'=');
        s.String::operator<<(value);
    }

    if (MOB(unsigned char, 0x10)) {
        MOB(short, 0xBE) = ComboBox_curData(ATTACK_TYPE(0));
        MOB(int, 0xC0) = (int)ATTACK_TIME(0).EditText::getInt(0);
        MOB(unsigned char, 0xC4) = (unsigned char)ATTACK_RANGE(0).EditText::getInt(0);
        MOB(short, 0xC6) = (short)ATTACK(0).EditText::getInt(0);
        MOB(short, 0xC8) = (short)ATTACK_PLUS(0).EditText::getInt(0);
        MOB(unsigned char, 0xCA) = (unsigned char)ComboBox_curSel(ATTACK_ATTRIB(0));
    } else {
        MOB(short, 0xBE) = ComboBox_curData(ATTACK_TYPE(0));
    }
    MOB(unsigned char, 0xCC) = (unsigned char)ComboBox_curSel(ATTACK_OK(0));
    {
        int   special = (int)ATTACK_SPECIAL(0).EditText::getInt(0);
        int   attrib  = (int)ComboBox_curSel(ATTACK_ATTRIB(0));
        short plus    = (short)ATTACK_PLUS(0).EditText::getInt(0);
        short attack  = (short)ATTACK(0).EditText::getInt(0);
        int   range   = (int)ATTACK_RANGE(0).EditText::getInt(0);
        int   time    = (int)ATTACK_TIME(0).EditText::getInt(0);
        fields.String::operator<<(L",AttackType1=").String::operator<<(MOB(short, 0xBE))
              .String::operator<<(L",AttackTime1=").String::operator<<(time)
              .String::operator<<(L",AttackRange1=").String::operator<<(range)
              .String::operator<<(L",Attack1=").String::operator<<(attack)
              .String::operator<<(L",AttackPlus1=").String::operator<<(plus)
              .String::operator<<(L",AttackAttrib1=").String::operator<<(attrib)
              .String::operator<<(L",AttackSpecial1=").String::operator<<(special)
              .String::operator<<(L",AttackOK1=").String::operator<<(MOB(unsigned char, 0xCC));
    }

    if (MOB(unsigned char, 0x10)) {
        MOB(short, 0xD2) = ComboBox_curData(ATTACK_TYPE(1));
        MOB(int, 0xD4) = (int)ATTACK_TIME(1).EditText::getInt(0);
        MOB(unsigned char, 0xD8) = (unsigned char)ATTACK_RANGE(1).EditText::getInt(0);
        MOB(short, 0xDA) = (short)ATTACK(1).EditText::getInt(0);
        MOB(short, 0xDC) = (short)ATTACK_PLUS(1).EditText::getInt(0);
        MOB(unsigned char, 0xDE) = (unsigned char)ComboBox_curSel(ATTACK_ATTRIB(1));
        MOB(unsigned char, 0xE0) = (unsigned char)ComboBox_curSel(ATTACK_OK(1));
    } else {
        MOB(short, 0xD2) = ComboBox_curData(ATTACK_TYPE(1));
        MOB(unsigned char, 0xD8) = (unsigned char)ATTACK_RANGE(1).EditText::getInt(0);
    }
    {
        int   ok      = (int)ComboBox_curSel(ATTACK_OK(1));
        int   special = (int)ATTACK_SPECIAL(1).EditText::getInt(0);
        int   attrib  = (int)ComboBox_curSel(ATTACK_ATTRIB(1));
        short plus    = (short)ATTACK_PLUS(1).EditText::getInt(0);
        short attack  = (short)ATTACK(1).EditText::getInt(0);
        int   time    = (int)ATTACK_TIME(1).EditText::getInt(0);
        fields.String::operator<<(L",AttackType2=").String::operator<<(MOB(short, 0xD2))
              .String::operator<<(L",AttackTime2=").String::operator<<(time)
              .String::operator<<(L",AttackRange2=").String::operator<<(MOB(unsigned char, 0xD8))
              .String::operator<<(L",Attack2=").String::operator<<(attack)
              .String::operator<<(L",AttackPlus2=").String::operator<<(plus)
              .String::operator<<(L",AttackAttrib2=").String::operator<<(attrib)
              .String::operator<<(L",AttackSpecial2=").String::operator<<(special)
              .String::operator<<(L",AttackOK2=").String::operator<<(ok);
    }

    if (MOB(unsigned char, 0x10)) {
        MOB(short, 0xE6) = ComboBox_curData(ATTACK_TYPE(2));
        MOB(int, 0xE8) = (int)ATTACK_TIME(2).EditText::getInt(0);
        MOB(unsigned char, 0xEC) = (unsigned char)ATTACK_RANGE(2).EditText::getInt(0);
        MOB(short, 0xEE) = (short)ATTACK(2).EditText::getInt(0);
        MOB(short, 0xF0) = (short)ATTACK_PLUS(2).EditText::getInt(0);
        MOB(unsigned char, 0xF2) = (unsigned char)ComboBox_curSel(ATTACK_ATTRIB(2));
    } else {
        MOB(short, 0xE6) = ComboBox_curData(ATTACK_TYPE(2));
        MOB(short, 0xF0) = (short)ATTACK_PLUS(2).EditText::getInt(0);
    }
    MOB(unsigned char, 0xF4) = (unsigned char)ComboBox_curSel(ATTACK_OK(2));
    {
        int   special = (int)ATTACK_SPECIAL(2).EditText::getInt(0);
        int   attrib  = (int)ComboBox_curSel(ATTACK_ATTRIB(2));
        short plus    = (short)ATTACK_PLUS(2).EditText::getInt(0);
        short attack  = (short)ATTACK(2).EditText::getInt(0);
        int   range   = (int)ATTACK_RANGE(2).EditText::getInt(0);
        int   time    = (int)ATTACK_TIME(2).EditText::getInt(0);
        fields.String::operator<<(L",AttackType3=").String::operator<<(MOB(short, 0xE6))
              .String::operator<<(L",AttackTime3=").String::operator<<(time)
              .String::operator<<(L",AttackRange3=").String::operator<<(range)
              .String::operator<<(L",Attack3=").String::operator<<(attack)
              .String::operator<<(L",AttackPlus3=").String::operator<<(plus)
              .String::operator<<(L",AttackAttrib3=").String::operator<<(attrib)
              .String::operator<<(L",AttackSpecial3=").String::operator<<(special)
              .String::operator<<(L",AttackOK3=").String::operator<<(MOB(unsigned char, 0xF4));
    }

    String inserts;
    for (int i = 0; i < 9; ++i) {
        BagsList::Item* item = FIELD(BagsList::Items, this, 0x7F8).get(i);
        int grade;
        int rate;
        if (item) {
            grade = FIELD(int, item, 0x0C);
            if (i >= 6)
                rate = (int)(FIELD(float, item, 0x10) * 100000.0f);
            else
                rate = (int)(FIELD(float, item, 0x10) * 1.0f);
        } else {
            grade = 0;
            rate = 0;
        }
        MOB(short, 0x46 + 2 * i) = (short)grade;
        MOB(int, 0x58 + 4 * i) = rate;

        String& s = inserts.String::operator<<(L"INSERT INTO [PS_GameDefs].[dbo].[MobItems] values (")
                           .String::operator<<(MOB(int, 0x14));
        String_putChar(s, L',');
        s.String::operator<<(i + 1);
        String_putChar(s, L',');
        s.String::operator<<(grade);
        String_putChar(s, L',');
        s.String::operator<<(rate);
        String_putChar(s, L')');
        s.String::newline();
    }

    void* db = P08C2_VCALL(g_studioFrame, 0x180, void*)(g_studioFrame);
    int result;
    if (!db) {
        result = 1;
    } else {
        String sql;
        String_putString(sql.String::operator<<(L"UPDATE [PS_GameDefs].[dbo].[Mobs] SET "), fields)
            .String::operator<<(L" WHERE MobID=").String::operator<<(MOB(int, 0x14));
        if (P08C2_VCALL(db, 0x30, int, const wchar_t*)(db, FIELD(const wchar_t*, &sql, 0x04))) {
            MessageBoxW(m_hWnd, L"The application failed to update the monster in database.", 0, MB_ICONERROR);
            result = -1;
        } else {
            String_clear(sql);
            sql.String::operator<<(L"DELETE FROM PS_GameDefs.dbo.MobItems WHERE MobID=").String::operator<<(MOB(int, 0x14));
            if (P08C2_VCALL(db, 0x28, int, const wchar_t*)(db, FIELD(const wchar_t*, &sql, 0x04))
             || P08C2_VCALL(db, 0x28, int, const wchar_t*)(db, FIELD(const wchar_t*, &inserts, 0x04))) {
                MessageBoxW(m_hWnd, L"The application failed to update the monster-drops in database.", 0, MB_ICONERROR);
                result = -1;
            } else {
                result = 1;
            }
        }
    }
    return result;

#undef MOB
#undef ATTACK_ATTRIB
#undef ATTACK_SPECIAL
#undef ATTACK_TYPE
#undef ATTACK_RANGE
#undef ATTACK_TIME
#undef ATTACK_PLUS
#undef ATTACK
#undef ATTACK_OK
#undef RESIST_SKILL
#undef RESIST_STATE
}
