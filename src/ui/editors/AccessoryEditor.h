#pragma once
#include "ItemEditor.h"

class AccessoryEditor : public ItemEditor
{
public:
    AccessoryEditor(Window* owner, Data::Items* items, Data::Item* item);
    virtual ~AccessoryEditor();

    virtual bool onCreate();
    virtual bool createEditor();
    virtual void saveControls();

public:
    ComboBox m_atkSpeed;
    ComboBox m_moveSpeed;
};
static_assert(sizeof(AccessoryEditor) == 0x518, "AccessoryEditor size");
