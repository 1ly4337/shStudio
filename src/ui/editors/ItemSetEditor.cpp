#include "ItemSetEditor.h"
#include "String.hpp"
#include "cash_g3.h"
#include "_imports_p07.h"

ItemSetEditor::ItemSetEditor(Data::ItemSet* set, int index)
    : Modal()
    , m_type()
    , m_typeId()
    , m_bonus()
{
    m_set = set;
    m_index = index;
}

bool ItemSetEditor::onCreate()
{
    g3_createLabel(m_hWnd, L"Additional Item:", 20, 15, 200, 16, 5);
    g3_createLabel(m_hWnd, L"Type:", 20, 37, 40, 16, 4);
    m_type.createNumber(m_hWnd,(HMENU) (HMENU)101, 60, 35, 40, 18, itemSet_type(m_set, m_index), 0, 0);
    g3_createLabel(m_hWnd, L"TypeID:", 110, 37, 50, 16, 4);
    m_typeId.createNumber(m_hWnd,(HMENU) (HMENU)102, 160, 35, 40, 18, itemSet_typeId(m_set, m_index), 0, 0);
    g3_createLabel(m_hWnd, L"Bonus description:", 20, 61, 200, 16, 5);
    g3_createEdit(m_bonus, m_hWnd, itemSet_bonus(m_set, m_index).m_buffer, 0x50211044u, 20, 77, 210, 90, 0x67);
    return Modal::onCreate();
}

int ItemSetEditor::mapCommandToResult(int id)
{
    if (m_buttons == 1) {
        itemSet_type(m_set, m_index) = (short)m_type.getInt(-1);
        itemSet_typeId(m_set, m_index) = (short)m_typeId.getInt(-1);
        String text;
        String* bonus = m_bonus.getText(&text);
        if (bonus != &itemSet_bonus(m_set, m_index))
            itemSet_bonus(m_set, m_index) = *bonus;
    }
    return m_buttons;
}
