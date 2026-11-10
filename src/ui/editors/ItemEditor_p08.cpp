#include <windows.h>
#include <commctrl.h>
#include <cwchar>
#include <cstring>
#include <cstdio>
#include "ida_compat.h"
#include "ItemEditor.h"
#include "Data_Items.h"
#include "Data_Item.h"
#include "SDataView.h"
#include "String.hpp"
#include "IniFile.h"
#include "StudioFrame.h"
#include "Data_Item_fields_B.h"
#include "_imports_B.h"
#include "AlertDial.h"

static HWND createLabel(HWND parent, LPCWSTR text, int x, int y, int w, int h, unsigned fontId)
{
    HWND hwnd = CreateWindowExW(0, L"Static", text, 0x50000000, x, y, w, h, parent, 0, 0, 0);
    if (hwnd)
        SendMessageW(hwnd, WM_SETFONT, (WPARAM)getStdFont(fontId), 0);
    return hwnd;
}

static void createCombo(ComboBox& cb, HWND parent, int x, int y, int w, int h, int id)
{
    cb.m_hWnd = CreateWindowExW(0x200, L"ComboBox", 0, (cb.m_dropLines & 3) | 0x50010000, x, y, w, h, parent,
                                (HMENU)(INT_PTR)id, 0, 0);
    if (cb.m_hWnd)
        cb.setFontId(cb.m_fontId);
    else
        GetLastError();
}

static void comboAddString(ComboBox& cb, const wchar_t* text)
{
    if (SendMessageW(cb.m_hWnd, CB_ADDSTRING, 0, (LPARAM)text) != -1)
    {
        LRESULT count = SendMessageW(cb.m_hWnd, CB_GETCOUNT, 0, 0);
        cb.m_items.StringTable::put((int)count, text);
    }
}

static void createCheck(Button& b, HWND parent, LPCWSTR text, DWORD style, int x, int y, int w, int h, int id)
{
    b.m_hWnd = CreateWindowExW(0, L"Button", text, style, x, y, w, h, parent, (HMENU)(INT_PTR)id, 0, 0);
    if (b.m_hWnd)
        b.setFontId(b.m_fontId);
    else
        GetLastError();
}

bool ItemEditor::onCreate()
{
    loadControls();
    return Modal::onCreate();
}

int ItemEditor::mapCommandToResult(int result)
{
    if (result == 1)
    {
        if (m_model.m_hWnd)
        {
            HWND hwnd = m_model.m_hWnd;
            if (hwnd)
            {
                LRESULT sel = SendMessageW(hwnd, CB_GETCURSEL, 0, 0);
                if ((unsigned char)SendMessageW(m_model.m_hWnd, CB_GETITEMDATA, sel, 0) == 0xFF)
                {
                    AlertDial::alert(m_hWnd, L"You must select a valid model.", 0, 16);
                    return -1;
                }
            }
        }
        saveControls();

        void* db = g_studioFrame->getSqlConnection();
        if (db && FIELD(bool, db, 0x18))
        {
            String sql;
            Data::Item* item = m_item;
            int itemId = DI_TypeId(item) + 1000 * DI_Type(item);
            sql << L"DELETE FROM [PS_GameDefs].[dbo].[Items] WHERE ItemID=" << (unsigned long)itemId << L"\r\n";
            static_cast<Streamable*>(item)->toText(sql, 'sql ');
            if (!P08B_VFN(db, 0x28, int, const wchar_t*)(db, sql.m_buffer))
            {
                sql.clear();
                Data::Item* rec = m_item;
                sql << L"The item " << DI_Type(rec) << '.' << DI_TypeId(rec)
                    << L" of the table PS_GameDefs.dbo.Items is updated.";
                static_cast<SDataView*>(m_owner)->setStatusText(sql.m_buffer, 0);
            }
        }
    }
    return result;
}

bool ItemEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id - 110 > 5)
        return Modal::onCommand(code, ctrl, id);

    Button& check = m_classes[id - 110];
    unsigned char bit = (unsigned char)(id - 110);
    bool checked = check.isChecked() == 0;
    check.setCheck(checked);
    if (checked)
    {
        m_classMask |= 1 << bit;
        if (!SendMessageW(m_model.m_hWnd, CB_GETCOUNT, 0, 0))
        {
            refreshModels();
            return true;
        }
    }
    else
    {
        m_classMask &= ~(1 << bit);
        if (m_classMask == 0)
        {
            SendMessageW(m_model.m_hWnd, CB_RESETCONTENT, 0, 0);
            m_model.m_items.StringTable::deleteAll();
            return true;
        }
    }
    return true;
}

void ItemEditor::loadControls()
{
    if (m_icon.m_hWnd)
        m_icon.select(DI_Icon(m_item));
    if (m_model.m_hWnd)
        m_model.selectData(DI_Model(m_item));
    if (m_name.m_hWnd)
        m_name.setText(DI_ItemName(m_item).m_buffer);
    if (m_description.m_hWnd)
        m_description.setText(DI_Description(m_item).m_buffer);
    if (m_level.m_hWnd)
        m_level.setInt(DI_ReqLevel(m_item));

    HWND faction = m_faction.m_hWnd;
    if (faction)
    {
        WPARAM sel = DI_Country(m_item);
        if (SendMessageW(faction, CB_GETCOUNT, 0, 0) == 3)
        {
            switch (DI_Country(m_item))
            {
            case 2: sel = 0; break;
            case 5: sel = 1; break;
            case 6: sel = 2; break;
            }
        }
        SendMessageW(m_faction.m_hWnd, CB_SETCURSEL, sel, 0);
    }

    HWND sex = m_sex.m_hWnd;
    if (sex)
    {
        unsigned char special = DI_Special(m_item);
        if (special == 64 || special == 65)
            SendMessageW(sex, CB_SETCURSEL, special - 63, 0);
        else
            SendMessageW(sex, CB_SETCURSEL, 0, 0);
    }

    int bit = 1;
    for (int i = 0; i < 6; ++i)
    {
        if (m_classes[i].m_hWnd && DI_Class(m_item, i) == 1)
        {
            m_classes[i].setCheck(true);
            if (DI_Country(m_item) <= 2u)
                m_classMask |= bit;
            else
                m_classMask |= __ROL4__(bit, 8);
        }
        bit = __ROL4__(bit, 1);
    }

    if (m_mode.m_hWnd)
        SendMessageW(m_mode.m_hWnd, CB_SETCURSEL, DI_Grow(m_item), 0);
    if (m_color.m_hWnd)
        SendMessageW(m_color.m_hWnd, CB_SETCURSEL, DI_ReqDex(m_item), 0);
    if (m_max.m_hWnd)
        m_max.setInt(DI_ReqWis(m_item));
    if (m_enchantItem.m_hWnd)
        m_enchantItem.setInt(DI_ReqLuc(m_item));
    if (m_slots.m_hWnd)
        SendMessageW(m_slots.m_hWnd, CB_SETCURSEL, DI_Slot(m_item), 0);
    if (m_endurance.m_hWnd)
        m_endurance.setInt(DI_Quality(m_item));
    if (m_attackMin.m_hWnd)
        m_attackMin.setInt(DI_Effect1(m_item));
    if (m_attackMax.m_hWnd)
        m_attackMax.setInt(DI_Effect2(m_item));
    if (m_defense.m_hWnd)
        m_defense.setInt(DI_Effect3(m_item));
    if (m_resist.m_hWnd)
        m_resist.setInt(DI_Effect4(m_item));
    if (m_hp.m_hWnd)
        m_hp.setInt(DI_ConstHP(m_item));
    if (m_sp.m_hWnd)
        m_sp.setInt(DI_ConstSP(m_item));
    if (m_mp.m_hWnd)
        m_mp.setInt(DI_ConstMP(m_item));
    if (m_str.m_hWnd)
        m_str.setInt(DI_ConstStr(m_item));
    if (m_dex.m_hWnd)
        m_dex.setInt(DI_ConstDex(m_item));
    if (m_rec.m_hWnd)
        m_rec.setInt(DI_ConstRec(m_item));
    if (m_int.m_hWnd)
        m_int.setInt(DI_ConstInt(m_item));
    if (m_wis.m_hWnd)
        m_wis.setInt(DI_ConstWis(m_item));
    if (m_luc.m_hWnd)
        m_luc.setInt(DI_ConstLuc(m_item));
    if (m_absorb.m_hWnd)
        m_absorb.setInt(DI_Exp(m_item));
    if (m_buy.m_hWnd)
        m_buy.setInt(DI_Buy(m_item));
    if (m_sell.m_hWnd)
        m_sell.setInt(DI_Sell(m_item));
    if (m_restriction.m_hWnd)
        SendMessageW(m_restriction.m_hWnd, CB_SETCURSEL, DI_ReqOg(m_item), 0);
    if (m_dropGroup.m_hWnd)
        m_dropGroup.setInt(DI_Grade(m_item));
    if (m_stack.m_hWnd)
        m_stack.setInt(DI_Count(m_item));
    if (m_statCount.m_hWnd)
        m_statCount.setInt(DI_Server(m_item));
    if (m_element.m_hWnd)
        SendMessageW(m_element.m_hWnd, CB_SETCURSEL, 0, 0);
}

void ItemEditor::saveControls()
{
    int enchant = 0;
    if (m_enchantItem.m_hWnd)
    {
        enchant = m_enchantItem.getInt(DI_ReqLuc(m_item));
        DI_ReqLuc(m_item) = (unsigned short)enchant;
    }

    if (m_icon.m_hWnd)
    {
        int index = FIELD(int, &m_icon, 0x140);
        unsigned char icon;
        if (index < 0 || index >= FIELD(int, &m_icon, 0x84))
            icon = 0;
        else
            icon = (unsigned char)FIELD(int*, &m_icon, 0x7C)[index];
        DI_Icon(m_item) = icon;
    }

    if (m_model.m_hWnd)
    {
        HWND hwnd = m_model.m_hWnd;
        LRESULT data = (LRESULT)hwnd;
        if (hwnd)
        {
            LRESULT sel = SendMessageW(hwnd, CB_GETCURSEL, 0, 0);
            data = SendMessageW(m_model.m_hWnd, CB_GETITEMDATA, sel, 0);
        }
        DI_Model(m_item) = (unsigned char)data;
    }

    if (m_name.m_hWnd)
    {
        String text;
        DI_ItemName(m_item) = *m_name.getText(&text);
    }

    if (m_description.m_hWnd)
    {
        String text;
        DI_Description(m_item) = *m_description.getText(&text);
    }
    if (m_description.m_hWnd && enchant)
    {
        Data::Item* enchantItem = m_items->Data::Items::sub_42FC00(enchant / 1000, enchant % 1000);
        bool insert = g_preferences.getInt(L"Items", L"insert enhancement", 1) == 1;
        String pattern = g_preferences.getString(L"Items", L"enhancement pattern",
                                                L"{c5}[Enhancement: #]{/c}");

        int percents = 0;
        bool reset = false;
        for (unsigned i = 0; i < pattern.m_length; ++i)
            if (pattern.m_buffer[i] == L'%')
                ++percents;
        if (pattern.m_length && percents)
            reset = true;
        if (!reset)
        {
            int hashes = 0;
            if (!pattern.m_length)
                reset = true;
            else
            {
                for (unsigned i = 0; i < pattern.m_length; ++i)
                    if (pattern.m_buffer[i] == L'#')
                        ++hashes;
                if (hashes != 1)
                    reset = true;
            }
        }
        if (reset)
            pattern = L"{c5}[Enhancement: #]{/c}";
        pattern = pattern.hashToFormat();

        if (insert && enchantItem)
        {
            const wchar_t* enchantName = L"";
            if (DI_ItemName(enchantItem).m_buffer)
                enchantName = DI_ItemName(enchantItem).m_buffer;
            if (DI_Description(m_item).indexOf(enchantName) == -1)
            {
                if (DI_Description(m_item).m_length)
                    DI_Description(m_item) << L"\r\n";
                wchar_t buffer[256];
                memset(buffer, 0, 512);
                struct StringBits { DWORD d[8]; } nameBits;
                memcpy(&nameBits, &DI_ItemName(enchantItem), sizeof(nameBits));
                swprintf_s(buffer, 0x100, pattern.m_buffer, nameBits);
                if (wcslen(buffer) + DI_Description(m_item).m_length < 0x200)
                    DI_Description(m_item) << buffer;
            }
        }
    }

    HWND sex = m_sex.m_hWnd;
    if (sex)
    {
        LRESULT sel = SendMessageW(sex, CB_GETCURSEL, 0, 0);
        if (sel)
            DI_Special(m_item) = (unsigned char)(sel + 63);
        else
            DI_Special(m_item) = 0;
    }

    if (m_level.m_hWnd)
        DI_ReqLevel(m_item) = (short)m_level.getInt(DI_ReqLevel(m_item));

    HWND faction = m_faction.m_hWnd;
    if (faction)
    {
        LRESULT sel = SendMessageW(faction, CB_GETCURSEL, 0, 0);
        if (SendMessageW(m_faction.m_hWnd, CB_GETCOUNT, 0, 0) == 3)
        {
            if (sel)
            {
                if (sel == 1)
                    DI_Country(m_item) = 5;
                else
                    DI_Country(m_item) = 6;
            }
            else
            {
                DI_Country(m_item) = 2;
            }
        }
        else if (SendMessageW(m_faction.m_hWnd, CB_GETCOUNT, 0, 0) == 7)
        {
            DI_Country(m_item) = (unsigned char)sel;
        }
    }

    for (int i = 0; i < 6; ++i)
    {
        if (m_classes[i].m_hWnd)
            DI_Class(m_item, i) = m_classes[i].isChecked() != 0;
    }

    if (m_mode.m_hWnd)
        DI_Grow(m_item) = (unsigned char)SendMessageW(m_mode.m_hWnd, CB_GETCURSEL, 0, 0);
    if (m_color.m_hWnd)
        DI_ReqDex(m_item) = (unsigned short)SendMessageW(m_color.m_hWnd, CB_GETCURSEL, 0, 0);
    if (m_restriction.m_hWnd)
        DI_ReqOg(m_item) = (unsigned char)SendMessageW(m_restriction.m_hWnd, CB_GETCURSEL, 0, 0);
    if (m_slots.m_hWnd)
        DI_Slot(m_item) = (unsigned char)SendMessageW(m_slots.m_hWnd, CB_GETCURSEL, 0, 0);

    if (m_endurance.m_hWnd)
        DI_Quality(m_item) = (unsigned short)m_endurance.getInt(DI_Quality(m_item));
    if (m_attackMin.m_hWnd)
        DI_Effect1(m_item) = (unsigned short)m_attackMin.getInt(DI_Effect1(m_item));
    if (m_attackMax.m_hWnd)
        DI_Effect2(m_item) = (unsigned short)m_attackMax.getInt(DI_Effect2(m_item));
    if (m_defense.m_hWnd)
        DI_Effect3(m_item) = (unsigned short)m_defense.getInt(DI_Effect3(m_item));
    if (m_resist.m_hWnd)
        DI_Effect4(m_item) = (unsigned short)m_resist.getInt(DI_Effect4(m_item));
    if (m_hp.m_hWnd)
        DI_ConstHP(m_item) = (unsigned short)m_hp.getInt(DI_ConstHP(m_item));
    if (m_sp.m_hWnd)
        DI_ConstSP(m_item) = (unsigned short)m_sp.getInt(DI_ConstSP(m_item));
    if (m_mp.m_hWnd)
        DI_ConstMP(m_item) = (unsigned short)m_mp.getInt(DI_ConstMP(m_item));
    if (m_str.m_hWnd)
        DI_ConstStr(m_item) = (unsigned short)m_str.getInt(DI_ConstStr(m_item));
    if (m_dex.m_hWnd)
        DI_ConstDex(m_item) = (unsigned short)m_dex.getInt(DI_ConstDex(m_item));
    if (m_rec.m_hWnd)
        DI_ConstRec(m_item) = (unsigned short)m_rec.getInt(DI_ConstRec(m_item));
    if (m_int.m_hWnd)
        DI_ConstInt(m_item) = (unsigned short)m_int.getInt(DI_ConstInt(m_item));
    if (m_wis.m_hWnd)
        DI_ConstWis(m_item) = (unsigned short)m_wis.getInt(DI_ConstWis(m_item));
    if (m_luc.m_hWnd)
        DI_ConstLuc(m_item) = (unsigned short)m_luc.getInt(DI_ConstLuc(m_item));
    if (m_absorb.m_hWnd)
        DI_Exp(m_item) = (unsigned char)m_absorb.getInt(DI_Exp(m_item));
    if (m_buy.m_hWnd)
        DI_Buy(m_item) = (int)m_buy.getInt(DI_Buy(m_item));
    if (m_sell.m_hWnd)
        DI_Sell(m_item) = (int)m_sell.getInt(DI_Sell(m_item));
    if (m_dropGroup.m_hWnd)
        DI_Grade(m_item) = (short)m_dropGroup.getInt(DI_Grade(m_item));
    if (m_stack.m_hWnd)
        DI_Count(m_item) = (unsigned char)m_stack.getInt(DI_Count(m_item));
    if (m_statCount.m_hWnd)
        DI_Server(m_item) = (unsigned char)m_statCount.getInt(DI_Server(m_item));
    if (m_max.m_hWnd)
        DI_ReqWis(m_item) = (unsigned short)m_max.getInt(DI_ReqWis(m_item));
}

void ItemEditor::createNameRow(int x, int y, int width)
{
    createLabel(m_hWnd, L"Name:", 90, 22, 40, 16, 4);
    m_name.m_hWnd = CreateWindowExW(0x200, L"Edit", L"", 0x50010080, 130, 20, width, 18, m_hWnd, (HMENU)0x66, 0, 0);
    if (m_name.m_hWnd)
    {
        m_name.setFontId(m_name.m_fontId);
        SetWindowSubclass(m_name.m_hWnd, tabSubclassProc, 'edit', (DWORD_PTR)&m_name);
    }
    SendMessageW(m_name.m_hWnd, EM_LIMITTEXT, 30, 0);
    m_name.setText(DI_ItemName(m_item).m_buffer);
}

int ItemEditor::createDescriptionRow(int x, int y, int width, int height)
{
    createLabel(m_hWnd, L"Description:", 20, y + 2, 100, 16, 4);
    m_description.m_hWnd = CreateWindowExW(0x200, L"Edit", L"", 0x50211044, 20, y + 18, width, height, m_hWnd,
                                           (HMENU)0x68, 0, 0);
    if (m_description.m_hWnd)
    {
        m_description.setFontId(m_description.m_fontId);
        SetWindowSubclass(m_description.m_hWnd, tabSubclassProc, 'edit', (DWORD_PTR)&m_description);
    }
    SendMessageW(m_description.m_hWnd, EM_LIMITTEXT, 0x200, 0);
    m_description.setText(DI_Description(m_item).m_buffer);
    return height + 18;
}

int ItemEditor::createEquipRow(int x, int y, bool a4, bool a5, bool a6)
{
    bool lightNames = a4;
    bool classCombo = a5;
    bool withSex = a6;

    createLabel(m_hWnd, L"Can be equipped by:", 20, y + 2, 150, 18, 1);
    int row1 = y + 20;
    int col1 = (withSex ? 10 : 30) + 20;
    createCheck(m_classes[0], m_hWnd, lightNames ? L" Fighter" : L" Warrior", 0x50010002, col1, row1, 75, 18, 0x6E);
    int row2 = row1 + 18;
    createCheck(m_classes[1], m_hWnd, lightNames ? L" Defender" : L" Guardian", 0x50000002, col1, row2, 75, 18, 0x6F);
    if (m_classes[1].m_hWnd)
        SetWindowSubclass(m_classes[1].m_hWnd, noEraseSubclassProc, 'chck', (DWORD_PTR)&m_classes[1]);
    int col2 = (withSex ? 90 : 130) + 20;
    createCheck(m_classes[2], m_hWnd, lightNames ? L" Ranger" : L" Assassin", 0x50000002, col2, row1, 60, 18, 0x70);
    if (m_classes[2].m_hWnd)
        SetWindowSubclass(m_classes[2].m_hWnd, noEraseSubclassProc, 'chck', (DWORD_PTR)&m_classes[2]);
    createCheck(m_classes[3], m_hWnd, lightNames ? L" Archer" : L" Hunter", 0x50000002, col2, row2, 60, 18, 0x71);
    if (m_classes[3].m_hWnd)
        SetWindowSubclass(m_classes[3].m_hWnd, noEraseSubclassProc, 'chck', (DWORD_PTR)&m_classes[3]);
    int col3 = (withSex ? 170 : 230) + 20;
    createCheck(m_classes[4], m_hWnd, lightNames ? L" Mage" : L" Pagan", 0x50000002, col3, row1, 60, 18, 0x72);
    if (m_classes[4].m_hWnd)
        SetWindowSubclass(m_classes[4].m_hWnd, noEraseSubclassProc, 'chck', (DWORD_PTR)&m_classes[4]);
    createCheck(m_classes[5], m_hWnd, lightNames ? L" Priest" : L" Oracle", 0x50000002, col3, row2, 60, 18, 0x73);
    if (m_classes[5].m_hWnd)
        SetWindowSubclass(m_classes[5].m_hWnd, noEraseSubclassProc, 'chck', (DWORD_PTR)&m_classes[5]);

    if (withSex)
    {
        createLabel(m_hWnd, L"Sex:", 260, row1, 50, 18, 4);
        createCombo(m_sex, m_hWnd, 260, row1 + 14, 65, 100, 0x9A);
        comboAddString(m_sex, L"Both");
        comboAddString(m_sex, L"Male");
        comboAddString(m_sex, L"Female");
    }
    return createFactionRow(20, row1 + 45, classCombo, true);
}

int ItemEditor::createFactionRow(int x, int y, bool a4, bool a5)
{
    bool classes = a4;
    bool withLabel = a5;
    int cx = x;
    if (classes)
    {
        if (withLabel)
        {
            createLabel(m_hWnd, L"Class:", x, y + 4, 30, 18, 4);
            cx = x + 35;
        }
        createCombo(m_faction, m_hWnd, cx, y, 90, 200, 0x78);
        comboAddString(m_faction, L"Human");
        comboAddString(m_faction, L"Elf");
        comboAddString(m_faction, L"All Lights");
        comboAddString(m_faction, L"Deatheater");
        comboAddString(m_faction, L"Vail");
        comboAddString(m_faction, L"All Fury");
        comboAddString(m_faction, L"All factions");
    }
    else
    {
        if (withLabel)
        {
            createLabel(m_hWnd, L"From:", x, y + 4, 30, 18, 4);
            cx = x + 35;
        }
        createCombo(m_faction, m_hWnd, cx, y, 90, 150, 0x78);
        comboAddString(m_faction, L"Light Faction");
        comboAddString(m_faction, L"Fury Faction");
        comboAddString(m_faction, L"Both Faction");
    }

    int mx = cx + 100;
    createLabel(m_hWnd, L"in mode:", mx, y + 4, 48, 18, 4);
    createCombo(m_mode, m_hWnd, mx + 50, y, 120, 150, 0x79);
    comboAddString(m_mode, L"Easy or higher");
    comboAddString(m_mode, L"Normal or higher");
    comboAddString(m_mode, L"Difficult or higher");
    comboAddString(m_mode, L"Ultimate");
    return y + 35;
}

LRESULT ItemEditor::createColorRow(int x, int y, int labelWidth, int comboWidth)
{
    createLabel(m_hWnd, L"Color:", x, y + 2, labelWidth, 18, 4);
    createCombo(m_color, m_hWnd, x + labelWidth, y - 2, comboWidth, 200, 0x636C6F72);
    comboAddString(m_color, L"No color");
    comboAddString(m_color, L"Light Blue");
    comboAddString(m_color, L"Dark Blue");
    comboAddString(m_color, L"Green");
    comboAddString(m_color, L"Yellow");
    comboAddString(m_color, L"Orange");
    comboAddString(m_color, L"Light Purple");
    comboAddString(m_color, L"Purple");
    comboAddString(m_color, L"Dark Purple");
    return SendMessageW(m_color.m_hWnd, CB_SETCURSEL, DI_ReqDex(m_item), 0);
}

int ItemEditor::createLevelSlotRow(int x, int y, Window* parent)
{
    createLabel(parent->m_hWnd, L"Level:", 20, y + 2, 30, 18, 4);
    m_level.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)122, 55, y, 45, 18, DI_ReqLevel(m_item), 0, 0);
    createLabel(parent->m_hWnd, L"Slot:", 120, y + 2, 30, 18, 4);
    createCombo(m_slots, parent->m_hWnd, 155, y - 2, 45, 18, 0x7D);
    comboAddString(m_slots, L" 0 ");
    comboAddString(m_slots, L" 1 ");
    comboAddString(m_slots, L" 2 ");
    comboAddString(m_slots, L" 3 ");
    comboAddString(m_slots, L" 4 ");
    comboAddString(m_slots, L" 5 ");
    comboAddString(m_slots, L" 6 ");
    createColorRow(220, y, 35, 85);
    return y + 25;
}

int ItemEditor::createDefenseRow(int x, int y, Window* parent, WPARAM elementSel)
{
    createLabel(parent->m_hWnd, L"Def:", x, y + 2, 30, 18, 4);
    m_defense.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)129, x + 35, y, 45, 18, DI_Effect3(m_item), 0, 0);
    createLabel(parent->m_hWnd, L"Resist:", x + 100, y + 2, 30, 18, 4);
    m_resist.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)130, x + 135, y, 45, 18, DI_Effect4(m_item), 0, 0);
    if (elementSel == (WPARAM)-1)
    {
        createLabel(parent->m_hWnd, L"Abs°:", x + 200, y + 2, 30, 18, 4);
        m_absorb.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)131, x + 235, y, 45, 18, DI_Exp(m_item), 0, 0);
        return y + 25;
    }

    createLabel(parent->m_hWnd, L"Elem:", x + 200, y + 2, 30, 18, 4);
    createCombo(m_element, parent->m_hWnd, x + 235, y - 2, 85, 200, 0x86);
    comboAddString(m_element, L"None");
    if (elementSel)
    {
        if (elementSel == 1)
        {
            m_element.addString(L"Fire");
            m_element.addString(L"Water");
            m_element.addString(L"Earth");
            m_element.addString(L"Wind");
        }
        else if (elementSel == 2)
        {
            m_element.addString(L"Fire 1");
            m_element.addString(L"Water 1");
            m_element.addString(L"Earth 1");
            m_element.addString(L"Wind 1");
            m_element.addString(L"Fire 2");
            m_element.addString(L"Water 2");
            m_element.addString(L"Earth 2");
            m_element.addString(L"Wind 2");
        }
        return y + 25;
    }
    SendMessageW(m_element.m_hWnd, CB_SETCURSEL, 0, 0);
    m_element.enable(false);
    return y + 25;
}

int ItemEditor::createStatsRows(int x, int y, Window* parent)
{
    createLabel(parent->m_hWnd, L"STR:", x, y + 2, 30, 18, 4);
    int x1 = x + 35;
    m_str.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)140, x + 35, y, 45, 18, DI_ConstStr(m_item), 0, 0);
    int xl2 = x + 100;
    createLabel(parent->m_hWnd, L"DEX:", x + 100, y + 2, 30, 18, 4);
    int x2 = x + 135;
    m_dex.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)141, x + 135, y, 45, 18, DI_ConstDex(m_item), 0, 0);
    int xl3 = x + 200;
    createLabel(parent->m_hWnd, L"REC:", x + 200, y + 2, 30, 18, 4);
    int x3 = x + 235;
    m_rec.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)142, x + 235, y, 45, 18, DI_ConstRec(m_item), 0, 0);

    int y2 = y + 25;
    createLabel(parent->m_hWnd, L"LUC:", x, y2 + 2, 30, 18, 4);
    m_luc.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)145, x1, y2, 45, 18, DI_ConstLuc(m_item), 0, 0);
    createLabel(parent->m_hWnd, L"INT:", xl2, y2 + 2, 30, 18, 4);
    m_int.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)143, x2, y2, 45, 18, DI_ConstInt(m_item), 0, 0);
    createLabel(parent->m_hWnd, L"WIS:", xl3, y2 + 2, 30, 18, 4);
    m_wis.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)144, x3, y2, 45, 18, DI_ConstWis(m_item), 0, 0);
    return y2 + 25;
}

int ItemEditor::createHpSpMpRow(int x, int y, Window* parent)
{
    createLabel(parent->m_hWnd, L"HP:", x, y + 2, 30, 18, 4);
    m_hp.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)137, x + 35, y, 45, 18, DI_ConstHP(m_item), 0, 0);
    createLabel(parent->m_hWnd, L"SP:", x + 100, y + 2, 30, 18, 4);
    m_sp.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)138, x + 135, y, 45, 18, DI_ConstSP(m_item), 0, 0);
    createLabel(parent->m_hWnd, L"MP:", x + 200, y + 2, 30, 18, 4);
    m_mp.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)139, x + 235, y, 45, 18, DI_ConstMP(m_item), 0, 0);
    return y + 25;
}

int ItemEditor::createStatCountRow(int x, int y, Window* parent, int a5)
{
    createLabel(parent->m_hWnd, L"# stats:", x - 8, y + 2, 43, 18, 4);
    m_statCount.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)152, x + 35, y, 45, 18, DI_Server(m_item), 0, 0);
    createLabel(parent->m_hWnd, L"Max:", x + 100, y + 2, 30, 18, 4);
    m_max.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)150, x + 135, y, 45, 18, DI_ReqWis(m_item), 0, 0);
    if ((unsigned char)a5)
    {
        createLabel(parent->m_hWnd, L"Ench. Item:", x + 200, y - 3, 30, 26, 4);
        m_enchantItem.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)151, x + 235, y, 60, 18, DI_ReqLuc(m_item), 0, 0);
    }
    return y + 27;
}

int ItemEditor::createDropStackRow(int x, int y, Window* parent)
{
    createLabel(parent->m_hWnd, L"Drop group:", 15, y - 5, 35, 26, 4);
    m_dropGroup.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)148, 55, y, 45, 18, DI_Grade(m_item), 0, 0);
    createLabel(parent->m_hWnd, L"Stack:", 120, y + 2, 30, 18, 4);
    m_stack.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)149, 155, y, 45, 18, DI_Count(m_item), 0, 0);
    createLabel(parent->m_hWnd, L"Endu:", 220, y + 2, 30, 18, 4);
    m_endurance.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)126, 255, y, 60, 18, DI_Quality(m_item), 0, 0);
    return y + 27;
}

int ItemEditor::createPriceRow(int x, int y, Window* parent, int a5)
{
    int comboWidth;
    createLabel(parent->m_hWnd, L"Buy:", x, y + 2, 25, 18, 4);
    if (!(unsigned char)a5)
    {
        m_buy.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)146, x + 25, y, 65, 18, DI_Buy(m_item), 0, 0);
        createLabel(parent->m_hWnd, L"Sell:", x + 100, y + 2, 25, 18, 4);
        m_sell.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)147, x + 125, y, 60, 18, DI_Sell(m_item), 0, 0);
        createLabel(parent->m_hWnd, L"Restr:", x + 200, y + 2, 30, 18, 4);
        comboWidth = 80;
    }
    else
    {
        m_buy.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)146, x + 25, y, 65, 18, DI_Buy(m_item), 0, 0);
        createLabel(parent->m_hWnd, L"Sell:", x + 100, y + 2, 30, 18, 4);
        m_sell.createNumber(parent->m_hWnd, (HMENU)(INT_PTR)147, x + 130, y, 60, 18, DI_Sell(m_item), 0, 0);
        createLabel(parent->m_hWnd, L"Restr:", x + 200, y + 2, 35, 18, 4);
        comboWidth = 95;
    }
    createCombo(m_restriction, parent->m_hWnd, x + 235, y - 2, comboWidth, 18, 0x7C);
    comboAddString(m_restriction, L"None");
    comboAddString(m_restriction, L"Account");
    comboAddString(m_restriction, L"Character");
    return y + 25;
}
