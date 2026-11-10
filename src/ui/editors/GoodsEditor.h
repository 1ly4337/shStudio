#pragma once
#include "ItemEditor.h"

class GoodsEditor : public ItemEditor
{
public:
    GoodsEditor(Window* owner, Data::Items* items, Data::Item* item);
    virtual ~GoodsEditor();

    virtual bool onCreate();
    virtual bool onCommand(UINT code, HWND ctrl, UINT id);
    virtual bool createEditor();
    virtual void saveControls();

public:
    Button   m_questItem;
    EditText m_skillId;
    EditText m_skillLevel;
    EditText m_effect;
    EditText m_variant;
    EditText m_spellGroup;
};
static_assert(sizeof(GoodsEditor) == 0x510, "GoodsEditor size");

#include "ItemTypeNames_g4.h"
