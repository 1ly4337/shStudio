#include "ItemOption.h"
#include "IniFile.h"
#include "AlertDial.h"
#include "g2_inline.h"
#include "_imports_p07.h"
#include <new>

ItemOption::ItemOption()
{
}

ItemOption::~ItemOption()
{
}

void ItemOption::load(IniFile* prefs)
{
    m_insertEnhancement = prefs->getInt(L"Items", L"insert enhancement", 1) == 1;
    m_pattern = prefs->getString(L"Items", L"enhancement pattern", L"{c5}[Enhancement: #]{/c}");
    m_insertCheck.setCheck(m_insertEnhancement);
    m_patternEdit.setText(m_pattern.m_buffer);
    m_patternEdit.enable(m_insertEnhancement);
}

void ItemOption::save(IniFile* prefs)
{
    prefs->writeInt(L"Items", L"insert enhancement", m_insertEnhancement != 0);
    if (m_insertEnhancement)
        prefs->writeString(L"Items", L"enhancement pattern", m_pattern.m_buffer);
}

bool ItemOption::validate()
{
    {
        alignas(String) unsigned char storage[sizeof(String)];
        String* text = m_patternEdit.getText(reinterpret_cast<String*>(storage));
        m_pattern = *text;
        text->~String();
    }
    unsigned length = m_pattern.m_length;
    int percents = 0;
    if (length)
    {
        for (unsigned i = 0; i < length; i++)
            if (m_pattern.m_buffer[i] == L'%')
                ++percents;
    }
    if (!percents)
    {
        int hashes = 0;
        if (length)
        {
            for (unsigned i = 0; i < length; i++)
                if (m_pattern.m_buffer[i] == L'#')
                    ++hashes;
            if (hashes == 1)
                return true;
        }
    }
    AlertDial::alert(m_hWnd, L"The enhancement pattern is invalid", 0, 16);
    return false;
}

bool ItemOption::onCreate()
{
    g2_createStatic(m_hWnd, L"Options applied to Items.sdata", 15, 10, 250, 16, 5);
    g2_createCheckBox(m_insertCheck, m_hWnd, L" Insert enhancement item in description", 15, 32, 225, 18, 0x65);
    g2_createStatic(m_hWnd, L"Pattern:", 15, 57, 100, 16, 4);
    g2_createEdit(m_patternEdit, m_hWnd, 0x50010080, 15, 75, 270, 20, 0x66);
    SendMessageW(m_patternEdit.m_hWnd, EM_SETLIMITTEXT, 0x40, 0);
    g2_createStatic(m_hWnd,
                    L"The name of the good item used to enhance the edited item will be inserted at end of its "
                    L"description using the above template format. The character '#' must be present once and is "
                    L"replaced by the actual name.\r\nThe character '%' is forbidden and cannot be present.",
                    15, 105, 270, 68, 4);
    return true;
}

bool ItemOption::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code || id != 101)
        return Container::onCommand(code, ctrl, id);
    m_insertEnhancement = !m_insertEnhancement;
    m_insertCheck.setCheck(m_insertEnhancement);
    m_patternEdit.enable(m_insertEnhancement);
    return true;
}
