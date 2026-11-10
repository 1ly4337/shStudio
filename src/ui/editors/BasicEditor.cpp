#include "BasicEditor.h"
#include "ctlhelpers_g4.h"
#include "_imports_p07.h"
#include "String.hpp"
#include "Data_Items.h"

BasicEditor::BasicEditor(Data::Items* items, Data::Item* item, int )
    : Modal()
    , m_items(items)
    , m_item(item)
{
    m_isNew = false;
}

BasicEditor::~BasicEditor()
{
}

bool BasicEditor::create(HWND parent)
{
    const wchar_t* title = L"Item Creation";
    if (!m_isNew)
        title = L"Item Definition";
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 330, 280);
    if (!Window::create(parent, 0, &rc, 0x80C80000, 0x10101, title))
        return false;
    HWND top = Modal_getTopLevelWindow(parent);
    m_owner = top;
    Window_centerOn(m_hWnd, top);
    show(true);
    return true;
}

bool BasicEditor::onCreate()
{
    DWORD readOnly = ES_READONLY;
    if (m_isNew)
        readOnly = 0;

    g4_createLabel(L"Type:", 20, 22, 40, 20, m_hWnd, 4);
    m_type.createNumber(m_hWnd, (HMENU)0x74797065 , 65, 20, 45, 18,
                      FIELD(unsigned char, m_item, 0x54), readOnly, 0);
    g4_createLabel(L"TypeID:", 130, 22, 40, 18, m_hWnd, 4);
    m_typeId.createNumber(m_hWnd, (HMENU)0x74706964 , 180, 20, 50, 18,
                        FIELD(unsigned char, m_item, 0x55), readOnly, 0);
    g4_createLabel(L"Level:", 20, 45, 45, 18, m_hWnd, 4);
    m_level.createNumber(m_hWnd, (HMENU)0x6C65766C , 65, 43, 45, 18,
                       FIELD(short, m_item, 0x58), 0, 0);

    g4_createLabel(L"Used by:", 130, 45, 45, 18, m_hWnd, 4);
    g4_createCombo(m_faction, m_hWnd, 0x636E7472 , 180, 40, 100, 200);
    g4_addComboString(m_faction, L"Human");
    g4_addComboString(m_faction, L"Elf");
    g4_addComboString(m_faction, L"Light");
    g4_addComboString(m_faction, L"Deatheater");
    g4_addComboString(m_faction, L"Vail");
    g4_addComboString(m_faction, L"Fury");
    g4_addComboString(m_faction, L"All factions");
    SendMessageW(m_faction.m_hWnd, CB_SETCURSEL, FIELD(unsigned char, m_item, 0x5A), 0);

    g4_createLabel(L"Name:", 20, 74, 40, 18, m_hWnd, 4);
    g4_createEdit(m_name, m_hWnd, 0x6E616D65 , L"", 0x50010080, 65, 72, 235, 18);
    SendMessageW(m_name.m_hWnd, EM_LIMITTEXT, 30, 0);
    m_name.setFont((HFONT)getStdFont(6));

    g4_createLabel(L"Description:", 20, 96, 100, 16, m_hWnd, 4);
    g4_createEdit(m_description, m_hWnd, 0x64657363 , L"", 0x50211044, 20, 112, 280, 72);
    SendMessageW(m_description.m_hWnd, EM_LIMITTEXT, 0x200, 0);
    m_description.setFont((HFONT)getStdFont(6));

    m_name.setText(FIELD(String, m_item, 0x14).m_buffer);
    m_description.setText(FIELD(String, m_item, 0x34).m_buffer);
    return Modal::onCreate();
}

int BasicEditor::mapCommandToResult(int result)
{
    if (result != 1)
        return result;

    int type = m_type.getInt(0);
    int typeId = m_typeId.getInt(0);
    short level = (short)m_level.getInt(0);
    LRESULT faction = SendMessageW(m_faction.m_hWnd, CB_GETCURSEL, 0, 0);

    if ((unsigned)(type - 1) > 0xFE)
    {
        MessageBoxW(m_hWnd, L"Type shall be defined in range 1 up to 250", L"Item Creation", MB_ICONHAND);
        return -1;
    }
    if ((unsigned)(typeId - 1) > 0xFE)
    {
        MessageBoxW(m_hWnd, L"TypeID shall be defined in range 1 up to 255", L"Item Creation", MB_ICONHAND);
        return -1;
    }
    if (m_isNew && m_items->sub_42FC00(type, typeId))
    {
        MessageBoxW(m_hWnd, L"An item with same Type, TypeID already exist", L"Item Creation", MB_ICONHAND);
        return -1;
    }
    if (faction == -1)
    {
        MessageBoxW(m_hWnd, L"A country shall be defined", L"Item Creation", MB_ICONHAND);
        return -1;
    }

    FIELD(unsigned char, m_item, 0x54) = (unsigned char)type;
    FIELD(unsigned char, m_item, 0x55) = (unsigned char)typeId;
    FIELD(unsigned char, m_item, 0x5A) = (unsigned char)faction;
    FIELD(short, m_item, 0x58) = level;
    {
        String text;
        FIELD(String, m_item, 0x14) = *m_name.getText(&text);
    }
    {
        String text;
        FIELD(String, m_item, 0x34) = *m_description.getText(&text);
    }
    return result;
}
