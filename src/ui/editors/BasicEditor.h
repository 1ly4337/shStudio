#pragma once
#include <windows.h>
#include "Modal.h"
#include "ComboBox.h"
#include "EditText.h"

namespace Data { class Item; class Items; }

class BasicEditor : public Modal
{
public:
    BasicEditor(Data::Items* items, Data::Item* item, int unused);
    virtual ~BasicEditor();

    virtual bool onCreate();
    virtual int mapCommandToResult(int result);

    bool create(HWND parent);

public:
    Data::Items* m_items;
    Data::Item*  m_item;
    EditText     m_type;
    EditText     m_typeId;
    EditText     m_name;
    EditText     m_description;
    EditText     m_level;
    ComboBox     m_faction;
    bool         m_isNew;
};
static_assert(sizeof(BasicEditor) == 0x11C, "BasicEditor size");
