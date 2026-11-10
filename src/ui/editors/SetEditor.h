#pragma once
#include <windows.h>
#include "Modal.h"
#include "EditText.h"
#include "ListCtrl.h"

class Node;
namespace Data { class ItemSet; class Items; }

class SetEditor : public Modal
{
public:
    class SetEditorList : public ListCtrl
    {
    public:
        SetEditorList(SetEditor* editor, Data::Items* items, Data::ItemSet* set);
        virtual ~SetEditorList() {}

        DWORD getStyle() override { return 5; }
        bool onCreate() override;
        Node* getNode(int index) override;
        bool onActivateItem(int index, int keys) override;
        HRESULT onGetDispInfo(NMLVDISPINFOW* di) override;

    public:
        SetEditor*     m_editor;
        Data::Items*   m_items;
        Data::ItemSet* m_set;
    };

    explicit SetEditor(Data::ItemSet* set);
    virtual ~SetEditor();

    bool onCreate() override;

public:
    Data::ItemSet* m_set;
    EditText       m_name;
    SetEditorList  m_list;
    bool           m_modified;
};
static_assert(sizeof(SetEditor::SetEditorList) == 0x100, "SetEditorList size");
static_assert(sizeof(SetEditor) == 0x1C0, "SetEditor size");
