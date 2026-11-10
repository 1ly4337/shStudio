#pragma once
#include "ItemEditor.h"

class MountEditor : public ItemEditor
{
public:
    MountEditor(Window* owner, Data::Items* items, Data::Item* item);
    virtual ~MountEditor();

    virtual bool onCreate();
    virtual bool createEditor();
    virtual void saveControls();

public:
    EditText m_range;
    EditText m_group;
};
static_assert(sizeof(MountEditor) == 0x4E0, "MountEditor size");
