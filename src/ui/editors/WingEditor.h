#pragma once
#include "ItemEditor.h"
#include "BmpCtrl.h"

class WingEditor : public ItemEditor
{
public:
    WingEditor(Window* owner, Data::Items* items, Data::Item* item);
    virtual ~WingEditor();

    virtual bool onCreate();
    virtual bool onCommand(UINT code, HWND ctrl, UINT id);
    virtual bool createEditor();
    virtual void refreshModels() { refreshModelsImpl(); }

public:
    Button  m_expand;
    BmpCtrl m_preview;
    bool    m_expanded;
};
static_assert(sizeof(WingEditor) == 0x550, "WingEditor size");
