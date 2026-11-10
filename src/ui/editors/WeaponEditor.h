#pragma once
#include "ItemEditor.h"
#include "Container.h"
#include "ToggleButton.h"
#include "BmpCtrl.h"

class WeaponEditor : public ItemEditor
{
public:
    WeaponEditor(Window* owner, Data::Items* items, Data::Item* item);
    virtual ~WeaponEditor();

    virtual bool onCreate();
    virtual bool onCommand(UINT code, HWND ctrl, UINT id);
    virtual bool createEditor();
    virtual void loadControls();
    virtual void saveControls();

public:
    EditText     m_range;
    EditText     m_glow;
    ComboBox     m_speed;
    ToggleButton m_more;
    Container    m_extra;
    Container    m_last;
    int          m_extraY;
    Button       m_expand;
    BmpCtrl      m_preview;
    bool         m_expanded;
};
static_assert(sizeof(WeaponEditor) == 0x67C, "WeaponEditor size");
