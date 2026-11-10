#include <windows.h>
#include <cwchar>
#include "ida_compat.h"
#include "MonsterEditor.h"
#include "ComboBox.h"
#include "Data_Items.h"
#include "String.hpp"
#include "ptrArray.h"
#include "Window.h"
#include "_imports_B.h"
#include "StudioFrame.h"

void MonsterEditor::ItemsList::showGrade(int grade)
{
    SendMessageW(m_hWnd, LVM_DELETEALLITEMS, 0, 0);
    ptrArray<Data::Item*>& items = FIELD(ptrArray<Data::Item*>, this, 0xF4);
    for (int i = 0; i < items.m_count; )
    {
        ++i;
        items.m_items[i - 1] = 0;
    }
    items.m_count = 0;
    if (grade)
    {
        char* app = (char*)g_studioFrame;
        ClientData& client = g_studioFrame->m_client.m_clientData;
        client.loadSData(&client.m_items, L"Item\\item.sdata", L"BinarySData\\DBItemData.sdata", 0);
        int count = ((Data::Items*)(app + 0xBD0))->Data::Items::sub_42FC50(&items, grade);
        setItemCount(count);
    }
}

void MonsterEditor::fillNpcSkillCombo(ComboBox* combo)
{
    combo->addItem(L"Melee Attack", 0);

    char* app = (char*)g_studioFrame;
    char* table = app + 0xC7C;
    ClientData& client = g_studioFrame->m_client.m_clientData;
    client.loadSData(&client.m_mobsSkills, L"Monster\\NpcSkill.sdata", L"BinarySData\\DBNpcSkillData.sdata", 0);

    void* skills = app + 0xC7C;
    int count = P08B_VFN(skills, 0x04, int)(skills);
    for (int i = 0; i < count; ++i)
    {
        char* skill = FIELD(char**, skills, 0x04)[i];
        if (!skill)
            continue;
        String name(FIELD(String, skill, 0x0C));
        if (name.m_length >= 3 && wcsncmp(name.m_buffer, L"???", 3) == 0)
            name = FIELD(String, skill, 0x2C).left(25);
        combo->addItem(name.m_buffer, FIELD(int, skill, 0x50));
    }
}

bool MonsterEditor::open(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, (int)0x80000000, (int)0x80000000, 580, 575);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Monster Editor");
    if (result)
    {
        m_owner = Modal_getTopLevelWindow(parent);
        Window_centerOn(m_hWnd, m_owner);
        show(true);
        return true;
    }
    return result;
}
