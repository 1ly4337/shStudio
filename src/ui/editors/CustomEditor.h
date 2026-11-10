#pragma once
#include "ItemEditor.h"

class CustomEditor : public ItemEditor
{
public:
    CustomEditor(Window* owner, Data::Items* items, Data::Item* item);
    virtual ~CustomEditor();

    virtual bool onCreate();
    virtual bool createEditor();
    virtual void saveControls();

public:
    ComboBox m_atkSpeed;
    ComboBox m_moveSpeed;
};
static_assert(sizeof(CustomEditor) == 0x518, "CustomEditor size");
