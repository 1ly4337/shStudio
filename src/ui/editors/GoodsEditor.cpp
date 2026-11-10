#include "GoodsEditor.h"
#include "ctlhelpers_g4.h"
#include "_imports_p07.h"

GoodsEditor::GoodsEditor(Window* owner, Data::Items* items, Data::Item* item)
    : ItemEditor(owner, items, item)
{
}

GoodsEditor::~GoodsEditor()
{
}

bool GoodsEditor::createEditor()
{
    HWND parent = m_owner->m_hWnd;
    m_icon.m_columns = 6;
    m_buttons = 1;
    return g4_createModal(this, parent, 370, 560, L"Goods Editor");
}

bool GoodsEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id != 301)
        return ItemEditor::onCommand(code, ctrl, id);
    m_questItem.setCheck(m_questItem.isChecked() == 0);
    return true;
}

bool GoodsEditor::onCreate()
{
    g4_setupIconPicker(this);
    createNameRow(0, 0, 210);
    createColorRow(90, 45, 40, 120);
    int y = createDescriptionRow(0, 70, 320, 74) + 82;

    g4_createLabel(L"Used by:", 20, y + 6, 50, 18, m_hWnd, 5);
    createFactionRow(70, y, false, false);
    y += 28;
    g4_createLabel(L"Level:", 20, y + 3, 50, 18, m_hWnd, 4);
    m_level.createNumber(m_hWnd, (HMENU)122, 70, y, 45, 18, FIELD(short, m_item, 0x58), 0, 0);
    y += 26;
    g4_createLabel(L"Stats boost:", 20, y, 150, 18, m_hWnd, 5);
    int y2 = createHpSpMpRow(20, y + 16, this);
    y2 = createStatsRows(20, y2, this);

    int y3 = y2 + 4;
    g4_createLabel(L"Quest Info:", 20, y2 + 8, 80, 18, m_hWnd, 5);
    g4_createButton(m_questItem, m_hWnd, 0x12D, L" Quest Item", 0x50000002, 110, y3, 75, 18);
    if (m_questItem.m_hWnd)
        SetWindowSubclass(m_questItem.m_hWnd, noEraseSubclassProc, 0x6368636B , (DWORD_PTR)&m_questItem);
    m_questItem.setCheck(FIELD(unsigned char, m_item, 0x7B) == 1);
    g4_createLabel(L"Drop Rate:", 195, y3 + 2, 60, 16, m_hWnd, 4);
    m_endurance.createNumber(m_hWnd, (HMENU)126, 255, y3, 45, 18, FIELD(unsigned short, m_item, 0x7C), 0, 0);

    y3 += 25;
    g4_createLabel(L"Skill Info:", 20, y3 + 4, 100, 18, m_hWnd, 5);
    g4_createLabel(L"Skill ID:", 110, y3 + 2, 40, 18, m_hWnd, 4);
    m_skillId.createNumber(m_hWnd, (HMENU)302, 155, y3, 45, 18, FIELD(short, m_item, 0x76), 0, 0);
    g4_createLabel(L"Level:", 210, y3 + 2, 40, 18, m_hWnd, 4);
    m_skillLevel.createNumber(m_hWnd, (HMENU)303, 255, y3, 45, 18, FIELD(unsigned char, m_item, 0x78), 0, 0);

    y3 += 25;
    g4_createLabel(L"Special Info:", 20, y3 + 4, 100, 18, m_hWnd, 5);
    g4_createLabel(L"Effect:", 110, y3 + 2, 40, 26, m_hWnd, 4);
    m_effect.createNumber(m_hWnd, (HMENU)156, 155, y3, 45, 18, FIELD(unsigned char, m_item, 0x7A), 0, 0);
    g4_createLabel(L"Variant:", 210, y3 + 2, 40, 18, m_hWnd, 4);
    m_variant.createNumber(m_hWnd, (HMENU)304, 255, y3, 45, 18, FIELD(short, m_item, 0x70), 0, 0);

    y3 += 30;
    g4_createLabel(L"Other Info:", 20, y3, 100, 18, m_hWnd, 5);
    y3 += 16;
    g4_createLabel(L"Spell group:", 20, y3 - 2, 40, 26, m_hWnd, 4);
    m_spellGroup.createNumber(m_hWnd, (HMENU)306, 55, y3, 45, 18, FIELD(unsigned char, m_item, 0x75), 0, 0);
    g4_createLabel(L"Drop group:", 110, y3 - 2, 40, 26, m_hWnd, 4);
    m_dropGroup.createNumber(m_hWnd, (HMENU)148, 155, y3, 45, 18, FIELD(short, m_item, 0xA4), 0, 0);
    g4_createLabel(L"Stack:", 210, y3 + 3, 40, 18, m_hWnd, 4);
    m_stack.createNumber(m_hWnd, (HMENU)149, 255, y3, 45, 18, FIELD(unsigned char, m_item, 0xA9), 0, 0);
    createPriceRow(20, y3 + 30, this, 0);

    loadControls();
    return Modal::onCreate();
}

void GoodsEditor::saveControls()
{
    ItemEditor::saveControls();
    FIELD(short, m_item, 0x70) = (short)m_variant.getInt(FIELD(short, m_item, 0x70));
    FIELD(unsigned char, m_item, 0x75) = (unsigned char)m_spellGroup.getInt(FIELD(unsigned char, m_item, 0x75));
    FIELD(short, m_item, 0x76) = (short)m_skillId.getInt(FIELD(short, m_item, 0x76));
    FIELD(unsigned char, m_item, 0x78) = (unsigned char)m_skillLevel.getInt(FIELD(unsigned char, m_item, 0x78));
    FIELD(unsigned char, m_item, 0x7A) = (unsigned char)m_effect.getInt(FIELD(unsigned char, m_item, 0x7A));
    FIELD(unsigned char, m_item, 0x7B) = m_questItem.isChecked() != 0;
}

const wchar_t* goodsTypeName(unsigned type)
{
    const wchar_t* result;
    switch (type)
    {
    case 0x19: result = L"Goods Items, 1st set"; break;
    case 0x1B: result = L"Quest Items, 1st set"; break;
    case 0x1C: result = L"Quest Items, 2nd set"; break;
    case 0x1D: result = L"Quest Items, 3rd set"; break;
    case 0x26: result = L"Goods Items, 2nd set"; break;
    case 0x29: result = L"Noos"; break;
    case 0x2B: result = L"Etins"; break;
    case 0x2C: result = L"Goods Items, 3rd set"; break;
    case 0x4E: result = L"Goods Items, 4th set"; break;
    case 0x4F: result = L"Goods Items, 5th set"; break;
    case 0x50: result = L"Goods Items, 6th set"; break;
    case 0x5E: result = L"Metal Bars"; break;
    case 0x63: result = L"Quest Items, 4th set"; break;
    case 0x64: result = L"Goods Items, 7th set"; break;
    case 0x65: result = L"Goods Items, 8th set"; break;
    case 0x66: result = L"Goods Items, 9th set"; break;
    case 0x67: result = L"Goods Items, 10th set"; break;
    default:   result = 0; break;
    }
    return result;
}
