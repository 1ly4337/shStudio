#include "GearEditor.h"
#include "ctlhelpers_g4.h"
#include "_imports_p07.h"
#include "String.hpp"
#include "ptrArray.h"
#include "Node.h"

bool GearEditor::createEditor()
{
    HWND parent = m_owner->m_hWnd;
    m_icon.m_columns = 6;
    m_buttons = 1;
    return g4_createModal(this, parent, 380, 580, L"Gear Editor");
}

GearEditor::~GearEditor()
{
}

GearEditor::GearEditor(Window* owner, Data::Items* items, Data::Item* item)
    : ItemEditor(owner, items, item)
    , m_maleView(Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0, 0)
    , m_femaleView(Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0, 0)
{
    m_classMask = 0;
    m_previewShown = false;
}

bool GearEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id == 103)
    {
        if (code == CBN_SELCHANGE)
        {
            if (m_classMask != -2)
            {
                int model = (int)m_model.getSelectedData();
                int type = FIELD(unsigned char, m_item, 0x54);
                m_maleView.setBitmap((HBITMAP)g4_clientData()->renderCharItem(m_classMask, type, 1, model, type, 256, 256));
                model = (int)m_model.getSelectedData();
                type = FIELD(unsigned char, m_item, 0x54);
                m_femaleView.setBitmap((HBITMAP)g4_clientData()->renderCharItem(m_classMask, type, 0, model, type, 256, 256));
            }
            return true;
        }
        return ItemEditor::onCommand(code, ctrl, id);
    }
    if (id != 200)
        return ItemEditor::onCommand(code, ctrl, id);
    if (m_previewButton.m_hWnd)
    {
        if (m_previewShown)
        {
            SetWindowPos(m_hWnd, 0, 0, 0, 380, 580, SWP_NOMOVE | SWP_NOZORDER);
            SetWindowTextW(m_previewButton.m_hWnd, L">>");
        }
        else
        {
            SetWindowPos(m_hWnd, 0, 0, 0, 660, 580, SWP_NOMOVE | SWP_NOZORDER);
            SetWindowTextW(m_previewButton.m_hWnd, L"<<");
        }
        m_previewShown = !m_previewShown;
    }
    return true;
}

bool GearEditor::onCreate()
{
    Data::Item* item = m_item;
    bool light = FIELD(unsigned char, item, 0x5A) <= 2;
    if (light)
    {
        if (FIELD(unsigned char, item, 0x5B)) m_classMask |= 0x01;
        if (FIELD(unsigned char, item, 0x5C)) m_classMask |= 0x02;
        if (FIELD(unsigned char, item, 0x5D)) m_classMask |= 0x04;
        if (FIELD(unsigned char, item, 0x5E)) m_classMask |= 0x08;
        if (FIELD(unsigned char, item, 0x5F)) m_classMask |= 0x20;
        if (FIELD(unsigned char, item, 0x60)) m_classMask |= 0x10;
    }
    else
    {
        if (FIELD(unsigned char, item, 0x5B)) m_classMask |= 0x100;
        if (FIELD(unsigned char, item, 0x5C)) m_classMask |= 0x200;
        if (FIELD(unsigned char, item, 0x5D)) m_classMask |= 0x400;
        if (FIELD(unsigned char, item, 0x5E)) m_classMask |= 0x800;
        if (FIELD(unsigned char, item, 0x5F)) m_classMask |= 0x2000;
        if (FIELD(unsigned char, item, 0x60)) m_classMask |= 0x1000;
    }

    ImageStrip* icons = g4_clientData()->getItemIcons(FIELD(unsigned char, item, 0x54));
    if (icons)
    {
        m_icon.m_columns = (FIELD(int, icons, 0x20) > 27) * 2 + 6;
        m_icon.m_bitmaps = *(icons);
        m_icon.m_cells = *(icons);
    }
    g4_createIconPicker(this);
    createNameRow(0, 0, 220);
    g4_createLabel(L"Model:", 90, 47, 40, 18, m_hWnd, 4);

    ptrArray<Node*>* models = reinterpret_cast<ptrArray<Node*>*>(
        g4_clientData()->getCharModels(m_classMask, FIELD(unsigned char, m_item, 0x54), true, 0));
    if (models)
    {
        m_model.m_hWnd = CreateWindowExW(0, L"ComboBoxEx32", 0, (m_model.m_dropLines & 3) | 0x50010000,
                                         130, 42, 180, 200, m_hWnd, (HMENU)0x67, 0, 0);
        if (!m_model.m_hWnd)
        {
            GetLastError();
        }
        else
        {
            m_model.setFontId(m_model.m_fontId);
            SendMessageW(m_model.m_hWnd, CB_SETEXTENDEDUI, 1, 0);
            m_model.m_flags = 1;
        }
        g4_createButton(m_previewButton, m_hWnd, 0xC8, L">>", 0x50000000, 320, 43, 30, 20);

        int count = models->size();
        String text;
        for (int i = 0; i < count; ++i)
        {
            text.clear();
            Node* model = models->m_items[i];
            text << L' ' << (long)FIELD(long, model, 0x0C) << L" - " << FIELD(String, model, 0x18);
            m_model.addItem(text.m_buffer, FIELD(long, models->m_items[i], 0x0C));
        }
        SetWindowPos(m_model.m_hWnd, 0, 0, 0, 180, 400, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    else
    {
        g4_createLabel(L"No .MLT files available", 130, 47, 180, 18, m_hWnd, 4);
    }

    RECT rc;
    m_maleView.m_bitmapSize.cx = 256;
    m_maleView.m_bitmapSize.cy = 256;
    SetRect(&rc, 375, 20, 631, 276);
    m_maleView.create(m_hWnd, (HMENU)0xD2, &rc, WS_CHILD | WS_VISIBLE, 0, 0);
    m_femaleView.m_bitmapSize.cx = 256;
    m_femaleView.m_bitmapSize.cy = 256;
    SetRect(&rc, 375, 280, 631, 536);
    m_femaleView.create(m_hWnd, (HMENU)0xD3, &rc, WS_CHILD | WS_VISIBLE, 0, 0);

    HBITMAP female;
    if (models && m_classMask)
    {
        int model = FIELD(unsigned char, m_item, 0x56);
        m_maleView.setBitmap((HBITMAP)g4_clientData()->renderCharItem(
            m_classMask, FIELD(unsigned char, m_item, 0x54), 1, model, model, 256, 256));
        model = FIELD(unsigned char, m_item, 0x56);
        female = (HBITMAP)g4_clientData()->renderCharItem(
            m_classMask, FIELD(unsigned char, m_item, 0x54), 0, model, model, 256, 256);
    }
    else
    {
        COLORREF color = Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0;
        m_maleView.setBitmap(createSolidBitmap(256, 256, color));
        female = createSolidBitmap(256, 256, color);
    }
    m_femaleView.setBitmap(female);

    int y = createDescriptionRow(0, 70, 330, 74);
    y = createEquipRow(0, y + 76, light, true, false);
    g4_createLabel(L"Characteristics:", 20, y, 150, 18, m_hWnd, 1);
    int y2 = createLevelSlotRow(0, y + 20, this);
    unsigned char type = FIELD(unsigned char, m_item, 0x54);
    bool body = type == 0x11 || type == 0x20 || type == 0x43 || type == 0x52 || type == 0x49 || type == 0x58;
    y2 = createDefenseRow(20, y2, this, body);
    y2 = createStatsRows(20, y2, this);
    y2 = createHpSpMpRow(20, y2, this);
    y2 = createStatCountRow(20, y2 + 3, this, 1);
    y2 = createDropStackRow(0, y2, this);
    createPriceRow(20, y2, this, 0);

    loadControls();
    return Modal::onCreate();
}

void GearEditor::saveControls()
{
    ItemEditor::saveControls();
    LRESULT sel = SendMessageW(m_element.m_hWnd, CB_GETCURSEL, 0, 0);
    if (sel)
        FIELD(unsigned char, m_item, 0x79) = (unsigned char)(sel + 4);
    else
        FIELD(unsigned char, m_item, 0x79) = 0;
}

void GearEditor::loadControls()
{
    ItemEditor::loadControls();
    unsigned char element = FIELD(unsigned char, m_item, 0x79);
    if (element)
        SendMessageW(m_element.m_hWnd, CB_SETCURSEL, element - 4, 0);
}

void GearEditor::refreshModels()
{
    refreshModelsImpl();
}
