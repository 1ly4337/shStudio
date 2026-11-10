#include <windows.h>
#include <cstring>
#include "ida_compat.h"
#include "BagsList.h"
#include "MonsterEditor.h"
#include "EditText.h"
#include "Button.h"
#include "_imports_B.h"

int BagsList::Item::getInt(DWORD tag) const
{
    if (tag == 'drop')
        return m_grade;
    if (tag == 'rate')
    {
        int bits;
        memcpy(&bits, &m_rate, sizeof(bits));
        return bits;
    }
    return 0;
}

bool BagsList::Item::setInt(DWORD tag, int value)
{
    if (tag == 'drop')
    {
        m_grade = value;
        return true;
    }
    if (tag == 'rate')
    {
        memcpy(&m_rate, &value, sizeof(value));
        return true;
    }
    return false;
}

void BagsList::Item::reset()
{
    m_grade = 0;
    m_rate = 0;
}

BagsList::BagsList(MonsterEditor* editor, Monster* monster)
    : m_editor(editor)
{
    const unsigned short* grade = &FIELD(unsigned short, monster, 0x46);
    const int* rate = &FIELD(int, monster, 0x58);
    for (int i = 6; i; --i)
    {
        Item* item = new Item(*grade, (float)*rate);
        m_items.add(item);
        ++rate;
        ++grade;
    }
    grade = &FIELD(unsigned short, monster, 0x52);
    rate = &FIELD(int, monster, 0x70);
    for (int i = 3; i; --i)
    {
        Item* item = new Item(*grade, (float)*rate * 0.0000099999997f);
        m_items.add(item);
        ++rate;
        ++grade;
    }
}

bool BagsList::onCreate()
{
    bool result = ListCtrlOD::onCreate();
    if (result)
    {
        addColumn(L"Bag", 40, 18, 'drop');
        addColumn(L"%", 60, 130, 'rate');
        setItemCount(m_items.size());
        return true;
    }
    return result;
}

void BagsList::setSelectedBag(int grade, float rate)
{
    int index = getNextSelected(0);
    Item* item = m_items.get(index);
    if (item)
    {
        bool changed = item->m_grade != grade;
        item->m_grade = (unsigned short)grade;
        item->m_rate = rate;
        if (index >= 6)
            item->m_rate = rate * 0.0000099999997f;
        updateItem(-2);
        if (changed)
            FIELD(MonsterEditor::ItemsList, m_editor, 0x840).showGrade(grade);
    }
}

void BagsList::onItemChanged(int index, UINT newState)
{
    Item* item = m_items.get(index);
    if (item)
    {
        FIELD(EditText, m_editor, 0x810).setInt(item->m_grade);
        FIELD(EditText, m_editor, 0x81C).EditText::setFloat(item->m_rate);
        FIELD(Button, m_editor, 0x828).enable(true);
        if (newState & 2)
            FIELD(MonsterEditor::ItemsList, m_editor, 0x840).showGrade(item->m_grade);
    }
    else
    {
        FIELD(EditText, m_editor, 0x810).setText(L"");
        FIELD(EditText, m_editor, 0x81C).setText(L"");
        FIELD(Button, m_editor, 0x828).enable(false);
    }
}
