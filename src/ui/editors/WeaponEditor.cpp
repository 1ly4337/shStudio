#include "WeaponEditor.h"
#include "ctlhelpers_g5.h"
#include "ClientData.h"
#include "ItemModels.h"
#include "String.hpp"
#include "_imports_p07.h"
#include "StudioFrame.h"

WeaponEditor::WeaponEditor(Window* owner, Data::Items* items, Data::Item* item)
    : ItemEditor(owner, items, item)
    , m_range()
    , m_glow()
    , m_speed()
    , m_more()
    , m_extra()
    , m_last()
    , m_expand()
    , m_preview(Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0, 0)
{
    m_expanded = false;
    m_rightAlignButtons = false;
}

WeaponEditor::~WeaponEditor()
{
}

bool WeaponEditor::createEditor()
{
    HWND parent = m_owner->m_hWnd;
    m_icon.m_columns = 6;
    m_buttons = 1;
    return g4_createModal(this, parent, 380, 580, L"Weapon Editor");
}

bool WeaponEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id == 201)
    {
        m_more.setCheck(m_more.isChecked() == 0);
        int extraWidth = 0;
        if (m_expanded)
            extraWidth = 280;
        int cx = extraWidth + 380;
        int cy = m_more.isChecked() != 0 ? 635 : 580;
        if (m_more.isChecked())
        {
            RECT rc;
            RECT* r = m_extra.getClientRect(&rc);
            SetWindowPos(m_last.m_hWnd, 0, 0, r->bottom + m_extraY - r->top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            ShowWindow(m_extra.m_hWnd, SW_SHOW);
        }
        else
        {
            SetWindowPos(m_last.m_hWnd, 0, 0, m_extraY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            ShowWindow(m_extra.m_hWnd, SW_HIDE);
        }
        resize(cx, cy);
        return true;
    }
    if (id == 200)
    {
        if (m_expand.m_hWnd)
        {
            int cy = m_more.isChecked() != 0 ? 635 : 580;
            if (m_expanded)
            {
                SetWindowPos(m_hWnd, 0, 0, 0, 380, cy, SWP_NOMOVE | SWP_NOZORDER);
                SetWindowTextW(m_expand.m_hWnd, L">>");
            }
            else
            {
                SetWindowPos(m_hWnd, 0, 0, 0, 650, cy, SWP_NOMOVE | SWP_NOZORDER);
                SetWindowTextW(m_expand.m_hWnd, L"<<");
            }
            m_expanded = m_expanded == 0;
        }
        return true;
    }
    if (id == 103 && code == 1)
    {
        int model = (int)m_model.getSelectedData();
        HBITMAP bitmap = (HBITMAP)studioFillBackground(FIELD(unsigned char, m_item, 0x54), model, 256, 512);
        m_preview.setBitmap(bitmap);
        return true;
    }
    return ItemEditor::onCommand(code, ctrl, id);
}

bool WeaponEditor::onCreate()
{
    BYTE faction = FIELD(BYTE, m_item, 0x5A);
    bool light = faction == 2 || faction == 6;

    ImageStrip* icons = g4_clientData()->getItemIcons(FIELD(unsigned char, m_item, 0x54));
    if (icons)
    {
        m_icon.m_bitmaps = *icons;
        m_icon.m_cells = *icons;
    }
    m_icon.create(m_hWnd, (HMENU)101, 0, 23, 34, 34, 32, 0);
    g5_selectItemIcon(this);

    createNameRow(0, 0, 210);
    g4_createLabel(L"Model:", 90, 50, 40, 16, m_hWnd, 4);
    ItemModels* models = g4_clientData()->getItemModels(FIELD(char, m_item, 0x54), 0);
    if (models)
    {
        g5_createComboEx(m_model, m_hWnd, 0x67, 130, 45, 160, 240);
        g4_createButton(m_expand, m_hWnd, 0xC8, L">>", 0x50000000, 310, 45, 30, 20);
        int count = models->size();
        for (int i = 0; i < count; ++i)
        {
            String text;
            auto model = models->get(i);
            text << (long)FIELD(long, model, 0x0C) << " (";
            String file(FIELD(String, model, 0x3C));
            file.removeExtension();
            text << file;
            switch (FIELD(int, model, 0x1C))
            {
            case 0: text << L" - Blue glow"; break;
            case 1: text << L" - Red glow"; break;
            case 2: text << L" - Green glow"; break;
            case 3: text << L" - White glow"; break;
            default: break;
            }
            text << L')';
            m_model.addItem(text.m_buffer, FIELD(long, model, 0x0C));
        }
        SetWindowPos(m_model.m_hWnd, 0, 0, 0, 170, 240, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    else
    {
        g4_createLabel(L"No .ITM files available", 130, 50, 180, 18, m_hWnd, 4);
    }

    m_preview.m_bitmapSize.cx = 256;
    m_preview.m_bitmapSize.cy = 512;
    RECT rc;
    SetRect(&rc, 375, 20, 631, 532);
    m_preview.create(m_hWnd, (HMENU)0xD2, &rc, 0x50000000, 0, 0);
    {
        int modelIndex = FIELD(unsigned char, m_item, 0x56);
        char type = FIELD(char, m_item, 0x54);
        COLORREF bk = Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0;
        m_preview.setBitmap((HBITMAP)g4_clientData()->renderItem(type, modelIndex, bk, 256, 512));
    }

    int row = createDescriptionRow(0, 70, 320, 74);
    int y = createEquipRow(0, row + 76, light, false, false);
    g4_createLabel(L"Characteristics:", 20, y, 150, 18, m_hWnd, 1);
    int y2 = createLevelSlotRow(0, y + 20, this);

    g4_createLabel(L"Attack:", 20, y2 + 2, 34, 18, m_hWnd, 4);
    m_attackMin.createNumber(m_hWnd, (HMENU)127, 55, y2, 45, 18, FIELD(unsigned short, m_item, 0x7E), 0, 0);
    g4_createLabel(L"up to", 120, y2 + 2, 35, 18, m_hWnd, 4);
    m_attackMax.createNumber(m_hWnd, (HMENU)128, 155, y2, 45, 18,
                           FIELD(unsigned short, m_item, 0x7E) + FIELD(unsigned short, m_item, 0x80), 0, 0);
    g4_createLabel(L"Elem:", 220, y2 + 2, 30, 18, m_hWnd, 4);
    g4_createCombo(m_element, m_hWnd, 0x86, 255, y2 - 2, 85, 200);
    g4_addComboString(m_element, L"None");
    g4_addComboString(m_element, L"Fire 1");
    g4_addComboString(m_element, L"Water 1");
    g4_addComboString(m_element, L"Earth 1");
    g4_addComboString(m_element, L"Wind 1");
    g4_addComboString(m_element, L"Fire 2");
    g4_addComboString(m_element, L"Water 2");
    g4_addComboString(m_element, L"Earth 2");
    g4_addComboString(m_element, L"Wind 2");

    int y3 = y2 + 25;
    g4_createLabel(L"Range:", 20, y3 + 2, 30, 18, m_hWnd, 4);
    m_range.createNumber(m_hWnd, (HMENU)132, 55, y3, 45, 18, FIELD(short, m_item, 0x76), 0, 0);
    g4_createLabel(L"Glow:", 120, y3 + 2, 35, 18, m_hWnd, 4);
    m_glow.createNumber(m_hWnd, (HMENU)153, 155, y3, 45, 18, FIELD(unsigned short, m_item, 0x68), 0, 0);
    g4_createLabel(L"Speed:", 220, y3 + 2, 30, 18, m_hWnd, 4);
    g4_createCombo(m_speed, m_hWnd, 0x87, 255, y3 - 2, 85, 200);
    g4_addComboString(m_speed, L"N/A");
    g4_addComboString(m_speed, L"Very Slow");
    g4_addComboString(m_speed, L"Slow");
    g4_addComboString(m_speed, L"A Bit Slow");
    g4_addComboString(m_speed, L"Normal");
    g4_addComboString(m_speed, L"A Bit Fast");
    g4_addComboString(m_speed, L"Fast");
    g4_addComboString(m_speed, L"Very Fast");
    g4_addComboString(m_speed, L"Extr. Fast");
    SendMessageW(m_speed.m_hWnd, CB_SETCURSEL, FIELD(unsigned char, m_item, 0x78) - 1, 0);
    BYTE type = FIELD(BYTE, m_item, 0x54);
    if (type == 19 || type == 69 || type == 34 || type == 84)
    {
        SendMessageW(m_speed.m_hWnd, CB_SETCURSEL, 0, 0);
        m_speed.enable(false);
    }

    createStatsRows(20, y3 + 25, this);
    m_more.create(m_hWnd, 0, 0, y3 + 50, 0);
    m_more.setCheck(false);

    int y4 = y3 + 75;
    m_extra.m_bkColor = Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0;
    RECT* client = getClientRect(&rc);
    m_extra.createChildAt(m_hWnd, (HMENU)0x78747261 , 0, y4, client->right - client->left, 55,
                          0, 0, 0);
    m_extraY = y4;
    int y5 = createDefenseRow(20, 0, &m_extra, (WPARAM)-1);
    createHpSpMpRow(20, y5, &m_extra);

    int lastY = m_extraY;
    m_last.m_bkColor = Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0;
    RECT rc2;
    client = getClientRect(&rc2);
    m_last.createChildAt(m_hWnd, (HMENU)0x6C617374 , 0, lastY, client->right - client->left, 80,
                         WS_VISIBLE, 0, 0);
    int y6 = createStatCountRow(20, 2, &m_last, 1);
    int y7 = createDropStackRow(0, y6, &m_last);
    createPriceRow(20, y7, &m_last, 0);

    BYTE type2 = FIELD(BYTE, m_item, 0x54);
    if (type2 == 19 || type2 == 34 || type2 == 69 || type2 == 84)
        onCommand(0, m_more.m_hWnd, 201);
    loadControls();
    return Modal::onCreate();
}

void WeaponEditor::loadControls()
{
    ItemEditor::loadControls();
    m_attackMax.setInt(FIELD(unsigned short, m_item, 0x7E) + FIELD(unsigned short, m_item, 0x80));
    SendMessageW(m_element.m_hWnd, CB_SETCURSEL, FIELD(unsigned char, m_item, 0x79), 0);
}

void WeaponEditor::saveControls()
{
    ItemEditor::saveControls();
    FIELD(WORD, m_item, 0x68) = (WORD)m_glow.getInt(FIELD(unsigned short, m_item, 0x68));
    FIELD(WORD, m_item, 0x76) = (unsigned char)m_range.getInt(FIELD(short, m_item, 0x76));
    FIELD(WORD, m_item, 0x7E) = (WORD)m_attackMin.getInt(FIELD(unsigned short, m_item, 0x7E));
    BYTE* item = (BYTE*)m_item;
    FIELD(WORD, item, 0x80) = (WORD)(m_attackMax.getInt(FIELD(unsigned short, item, 0x7E)
                                                            + FIELD(unsigned short, item, 0x80))
                                     - FIELD(WORD, item, 0x7E));
    FIELD(BYTE, m_item, 0x79) = (BYTE)SendMessageW(m_element.m_hWnd, CB_GETCURSEL, 0, 0);
    LRESULT speed = SendMessageW(m_speed.m_hWnd, CB_GETCURSEL, 0, 0);
    if (speed)
        FIELD(BYTE, m_item, 0x78) = (BYTE)(speed + 1);
    else
        FIELD(BYTE, m_item, 0x78) = 0;
}
