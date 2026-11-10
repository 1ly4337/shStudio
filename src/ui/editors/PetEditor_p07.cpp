#include "PetEditor.h"
#include "ctlhelpers_g5.h"
#include "ClientData.h"
#include "_imports_p07.h"

bool PetEditor::createEditor()
{
    HWND parent = m_owner->m_hWnd;
    m_icon.m_columns = 6;
    m_buttons = 1;
    return g4_createModal(this, parent, 380, 530, L"Pet Editor");
}

bool PetEditor::onCreate()
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
    int row = createDescriptionRow(0, 70, 330, 74);
    int y = createEquipRow(0, row + 76, true, true, false);
    g4_createLabel(L"Characteristics:", 20, y, 150, 18, m_hWnd, 1);
    y = createLevelSlotRow(0, y + 20, this);
    y = createStatsRows(20, y, this);
    y = createHpSpMpRow(20, y, this);
    y = createDropStackRow(0, y, this);
    y = createPriceRow(20, y, this, 0);
    g4_createLabel(L"Srvr:", 20, y + 2, 30, 18, m_hWnd, 4);
    m_statCount.createNumber(m_hWnd, (HMENU)152, 50, y, 60, 18, FIELD(unsigned char, m_item, 0xA8), 0, 0);
    loadControls();
    return Modal::onCreate();
}
