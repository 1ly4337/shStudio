#include "SetEditor.h"
#include "ItemSetEditor.h"
#include "ComboBox.h"
#include "StudioFrame.h"
#include "Data_Items.h"
#include "String.hpp"
#include "cash_g3.h"
#include "_imports_p07.h"

SetEditor::SetEditorList::SetEditorList(SetEditor* editor, Data::Items* items, Data::ItemSet* set)
    : ListCtrl()
{
    m_editor = editor;
    m_items = items;
    m_set = set;
}

bool SetEditor::SetEditorList::onCreate()
{
    bool ok = ListCtrl::onCreate();
    if (!ok)
        return ok;
    addColumn(L"Type", 50, 16, 'type');
    addColumn(L"TypeID", 50, 16, 'tpid');
    addColumn(L"[Item's Name]", 160, 0, 'name');
    addColumn(L"Bonus", 320, 0, 'bn01');
    return true;
}

Node* SetEditor::SetEditorList::getNode(int index)
{
    ptrArray<Data::Item*>* table = m_items;
    return reinterpret_cast<Node*>(table->get(index));
}

bool SetEditor::SetEditorList::onActivateItem(int index, int keys)
{
    ItemSetEditor dlg(m_set, index);
    bool result;
    if (dlg.createDialog(m_hWnd, 270, 260, 1, L"Item Editor")) {
        if (dlg.doModal() == 1)
            m_editor->m_modified = true;
        result = true;
    } else {
        result = false;
    }
    return result;
}

HRESULT SetEditor::SetEditorList::onGetDispInfo(NMLVDISPINFOW* di)
{
    bool text = (di->item.mask & LVIF_TEXT) != 0;
    di->item.pszText = (LPWSTR)L"";
    int row = di->item.iItem;
    if (text) {
        switch (di->item.iSubItem) {
        case 0:
            sub_43CBF0(g_setEditorNumBuffer, L"%d", itemSet_type(m_set, row));
            di->item.pszText = g_setEditorNumBuffer;
            return 0;
        case 1:
            sub_43CBF0(g_setEditorNumBuffer, L"%d", itemSet_typeId(m_set, row));
            di->item.pszText = g_setEditorNumBuffer;
            return 0;
        case 2: {
            Data::Item* item = m_items->sub_42FC00(itemSet_type(m_set, row), itemSet_typeId(m_set, row));
            if (item) {
                di->item.pszText = FIELD(LPWSTR, item, 0x18);
                return 0;
            }
            break;
        }
        case 3:
            di->item.pszText = itemSet_bonus(m_set, row).m_buffer;
            break;
        }
    }
    return 0;
}

SetEditor::SetEditor(Data::ItemSet* set)
    : Modal()
    , m_name()
    , m_list(this, &g_studioFrame->m_client.m_clientData.m_items, set)
{
    m_set = set;
    ClientData& data = g_studioFrame->m_client.m_clientData;
    data.loadSData(&data.m_items, L"Item\\item.sdata", L"BinarySData\\DBItemData.sdata", 0);
    m_modified = false;
}

SetEditor::~SetEditor()
{
}

bool SetEditor::onCreate()
{
    g3_createLabel(m_hWnd, L"Name:", 20, 22, 50, 16, 1);
    g3_createEdit(m_name, m_hWnd, itemSet_name(m_set).m_buffer, 0x50010080u, 70, 20, 320, 18, 0x64);
    int height = 13 * dword_5A0124 + dword_5A0120 + 8;
    g3_createLabel(m_hWnd, L"Items:", 20, 46, 50, 16, 1);
    m_list.create(m_hWnd, (HMENU)0x65, 20, 62, 610, height, 0x10800000u, 0, 0);
    for (int i = 13; i; --i)
        m_list.insertTextItem(LPSTR_TEXTCALLBACKW, -1, -1);
    return Modal::onCreate();
}
