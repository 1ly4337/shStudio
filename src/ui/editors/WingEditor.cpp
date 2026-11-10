#include "WingEditor.h"
#include "ctlhelpers_g5.h"
#include "ClientData.h"
#include "MonsterModels.h"
#include "String.hpp"
#include "_imports_p07.h"

WingEditor::WingEditor(Window* owner, Data::Items* items, Data::Item* item)
    : ItemEditor(owner, items, item)
    , m_expand()
    , m_preview(Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0, 0)
{
    m_expanded = false;
}

WingEditor::~WingEditor()
{
}

bool WingEditor::createEditor()
{
    HWND parent = m_owner->m_hWnd;
    m_icon.m_columns = 6;
    m_buttons = 1;
    return g4_createModal(this, parent, 380, 550, L"Wing Editor");
}

bool WingEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id == 101)
    {
        if (code != 1)
            return ItemEditor::onCommand(code, ctrl, id);
        m_preview.setBitmap((HBITMAP)g4_clientData()->renderWing(m_icon.m_selected, code, 300, 300));
        return true;
    }
    if (id == 103)
    {
        if (code != 1)
            return ItemEditor::onCommand(code, ctrl, id);
        int bitmap;
        if (m_model.m_hWnd)
        {
            LRESULT sel = SendMessageW(m_model.m_hWnd, CB_GETCURSEL, 0, 0);
            LRESULT model = SendMessageW(m_model.m_hWnd, CB_GETITEMDATA, sel, 0);
            bitmap = g4_clientData()->renderWing((int)model, code, 300, 300);
        }
        else
        {
            bitmap = g4_clientData()->renderWing(0, code, 300, 300);
        }
        m_preview.setBitmap((HBITMAP)bitmap);
        return true;
    }
    if (id != 200)
        return ItemEditor::onCommand(code, ctrl, id);
    if (!m_expand.m_hWnd)
        return true;
    if (m_expanded)
    {
        SetWindowPos(m_hWnd, 0, 0, 0, 380, 550, SWP_NOMOVE | SWP_NOZORDER);
        SetWindowTextW(m_expand.m_hWnd, L">>");
    }
    else
    {
        SetWindowPos(m_hWnd, 0, 0, 0, 700, 550, SWP_NOMOVE | SWP_NOZORDER);
        SetWindowTextW(m_expand.m_hWnd, L"<<");
    }
    m_expanded = !m_expanded;
    return true;
}

bool WingEditor::onCreate()
{
    ImageStrip* icons = g4_clientData()->getItemIcons(FIELD(unsigned char, m_item, 0x54));
    if (icons)
    {
        m_icon.m_bitmaps = *icons;
        m_icon.m_cells = *icons;
    }
    m_icon.create(m_hWnd, (HMENU)101, 0, 23, 34, 34, 32, 0);
    g5_selectItemIcon(this);

    createNameRow(0, 0, 220);
    g4_createLabel(L"Model:", 90, 47, 40, 18, m_hWnd, 4);
    MonsterModels* models = g4_clientData()->getWingModels(0);
    if (!models)
    {
        g4_createLabel(L"No Wing.mon file available", 130, 47, 180, 18, m_hWnd, 4);
    }
    else
    {
        g5_createComboEx(m_model, m_hWnd, 0x67, 130, 42, 180, 200);
        g4_createButton(m_expand, m_hWnd, 0xC8, L">>", 0x50000000, 320, 43, 30, 20);
        int count = models->size();
        String text;
        for (int i = 0; i < count; ++i)
        {
            text.clear();
            MonsterModel* model = models->m_items[i];
            text << L' ' << (long)FIELD(long, model, 0x10) << L" - " << FIELD(String, model, 0x14);
            m_model.addItem(text.m_buffer, FIELD(long, models->m_items[i], 0x10));
        }
        SetWindowPos(m_model.m_hWnd, 0, 0, 0, 180, 400, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    m_preview.m_bitmapSize.cx = 300;
    m_preview.m_bitmapSize.cy = 300;
    RECT rc;
    SetRect(&rc, 375, 20, 675, 320);
    m_preview.create(m_hWnd, (HMENU)0xD2, &rc, 0x50000000, 0, 0);

    int row = createDescriptionRow(0, 70, 330, 74);
    int y = createEquipRow(0, row + 76, true, true, false);
    g4_createLabel(L"Characteristics:", 20, y, 150, 18, m_hWnd, 1);
    y = createLevelSlotRow(0, y + 20, this);
    y = createStatsRows(20, y, this);
    y = createHpSpMpRow(20, y, this);
    y = createStatCountRow(20, y, this, 0);
    y = createDropStackRow(0, y, this);
    createPriceRow(20, y, this, 0);
    loadControls();
    return Modal::onCreate();
}
