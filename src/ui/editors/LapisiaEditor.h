#pragma once
#include "GemEditor.h"

class LapisiaEditor : public GemEditor
{
public:
    LapisiaEditor(Window* owner, Data::Items* items, Data::Item* item);
    virtual ~LapisiaEditor();

    virtual bool onCreate();
    virtual bool onCommand(UINT code, HWND ctrl, UINT id);
    virtual bool createEditor();
    virtual void loadControls();
    virtual void saveControls();

public:
    Button   m_usedOn[7];
    ComboBox m_cost;
    Button   m_levels;
    EditText m_levelMin;
    EditText m_levelMax;
};
static_assert(sizeof(LapisiaEditor) == 0x568, "LapisiaEditor size");
