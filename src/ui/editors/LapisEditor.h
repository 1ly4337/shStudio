#pragma once
#include "GemEditor.h"

class LapisEditor : public GemEditor
{
public:
    LapisEditor(Window* owner, Data::Items* items, Data::Item* item);
    virtual ~LapisEditor();

    virtual bool onCreate();
    virtual bool onCommand(UINT code, HWND ctrl, UINT id);
    virtual bool createEditor();
    virtual void loadControls();
    virtual void saveControls();

    void setSpellValue(int value);

public:
    Button   m_links[10];
    EditText m_minLevel;
    ComboBox m_cost;
    ComboBox m_atkRange;
    ComboBox m_moveSpeed;
    ComboBox m_atkSpeed;
    ComboBox m_spell;
    EditText m_spellValue;
};
static_assert(sizeof(LapisEditor) == 0x620, "LapisEditor size");
