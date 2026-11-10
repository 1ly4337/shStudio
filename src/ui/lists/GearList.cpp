#include "GearList.h"
#include "GearEditor.h"
#include "ItemView.h"
#include "Data_Item.h"
#include "Data_Items.h"
#include "Preferences.h"
#include "_imports_A.h"

Node* GearList::findNode(Node* like)
{
    int selected = getNextSelected(0);
    if (selected < 0)
        return ItemList::findNode(like);
    return m_set.get(selected);
}

void GearList::editItem(Node* node)
{
    GearEditor editor(m_owner, m_items, static_cast<Data::Item*>(node));
    if (editor.createEditor())
    {
        HWND toggle = FIELD(HWND, &editor, 0x4CC);
        bool& expanded = FIELD(bool, &editor, 0x5C4);
        if (g_preferences.getInt(L"3D View", L"show gear", 0) == 1 && toggle)
        {
            if (expanded)
            {
                SetWindowPos(editor.m_hWnd, 0, 0, 0, 380, 580, SWP_NOMOVE | SWP_NOZORDER);
                SetWindowTextW(toggle, L">>");
            }
            else
            {
                SetWindowPos(editor.m_hWnd, 0, 0, 0, 660, 580, SWP_NOMOVE | SWP_NOZORDER);
                SetWindowTextW(toggle, L"<<");
            }
            expanded = !expanded;
        }
        if (editor.doModal() == 1)
            m_owner->setModified(true);
    }
}
