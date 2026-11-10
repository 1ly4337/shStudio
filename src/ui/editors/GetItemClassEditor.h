#pragma once
#include <windows.h>
#include "Modal.h"
#include "ComboBox.h"
#include "Button.h"

class ItemList;

class GetItemClassEditor : public Modal
{
public:
    GetItemClassEditor(ItemList* list, int faction);
    virtual ~GetItemClassEditor();

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    int mapCommandToResult(int id) override;

    bool create(HWND parent);

public:
    ItemList* m_list;
    int       m_faction;
    ComboBox  m_types;
    ComboBox  m_usedBy;
    Button    m_classes[6];
    int       m_type;
    int       m_usedByIndex;
    bool      m_classChecked[6];
};
