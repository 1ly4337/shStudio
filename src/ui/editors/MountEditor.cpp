#include "MountEditor.h"
#include "ctlhelpers_g5.h"
#include "ClientData.h"
#include "ContentProvider.h"
#include "String.hpp"
#include "_imports_p07.h"

MountEditor::MountEditor(Window* owner, Data::Items* items, Data::Item* item)
    : ItemEditor(owner, items, item)
{
}

MountEditor::~MountEditor()
{
}

bool MountEditor::createEditor()
{
    HWND parent = m_owner->m_hWnd;
    m_buttons = 1;
    return g4_createModal(this, parent, 430, 355, L"Mount Editor");
}

bool MountEditor::onCreate()
{
    ClientData* client = g4_clientData();
    {
        String folder = getContentFolder();
        if (!FIELD(int, client, 0x130 + 0x20))
            client->loadIconStrip(folder.m_buffer, L"icon_somo.dds",
                                  reinterpret_cast<ImageStrip*>(reinterpret_cast<BYTE*>(client) + 0x130), 1, 0);
    }
    ImageStrip* strip = reinterpret_cast<ImageStrip*>(reinterpret_cast<BYTE*>(client) + 0x130);
    if (strip)
    {
        m_icon.m_bitmaps = *strip;
        m_icon.m_cells = *strip;
    }
    m_icon.create(m_hWnd, (HMENU)101, 0, 23, 34, 34, 32, 0);
    g5_selectItemIcon(this);

    createNameRow(0, 0, 270);
    createColorRow(90, 45, 40, 120);
    int row = createDescriptionRow(0, 71, 380, 64);
    int y = row + 83;
    g4_createLabel(L"Can be used by:", 20, row + 88, 80, 18, m_hWnd, 4);
    createFactionRow(110, y, true, false);

    y += 32;
    g4_createLabel(L"Level min.:", 20, y + 2, 60, 18, m_hWnd, 4);
    m_level.createNumber(m_hWnd, (HMENU)122, 80, y, 40, 18, FIELD(short, m_item, 0x58), 0, 0);
    g4_createLabel(L"Range:", 130, y + 2, 40, 18, m_hWnd, 4);
    m_range.createNumber(m_hWnd, (HMENU)132, 170, y, 50, 18, FIELD(short, m_item, 0x76), 0, 0);
    g4_createLabel(L"Group:", 230, y + 2, 40, 18, m_hWnd, 4);
    m_group.createNumber(m_hWnd, (HMENU)133, 270, y, 50, 18, FIELD(unsigned char, m_item, 0x75), 0, 0);

    y += 30;
    g4_createLabel(L"Prices:", 20, y, 150, 18, m_hWnd, 1);
    createPriceRow(20, y + 18, this, 0);
    loadControls();
    return Modal::onCreate();
}

void MountEditor::saveControls()
{
    ItemEditor::saveControls();
    FIELD(WORD, m_item, 0x76) = (unsigned char)m_range.getInt(FIELD(short, m_item, 0x76));
    FIELD(BYTE, m_item, 0x75) = (BYTE)m_group.getInt(FIELD(unsigned char, m_item, 0x75));
}
