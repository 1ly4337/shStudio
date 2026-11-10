#pragma once
#include "ItemEditor.h"
#include "BmpCtrl.h"

class GearEditor : public ItemEditor
{
public:
    GearEditor(Window* owner, Data::Items* items, Data::Item* item);
    virtual ~GearEditor();

    virtual bool onCreate();
    virtual bool onCommand(UINT code, HWND ctrl, UINT id);
    virtual bool createEditor();
    virtual void loadControls();
    virtual void saveControls();
    virtual void refreshModels();

public:
    Button  m_previewButton;
    BmpCtrl m_maleView;
    BmpCtrl m_femaleView;
    bool    m_previewShown;
};
static_assert(sizeof(GearEditor) == 0x5C8, "GearEditor size");
