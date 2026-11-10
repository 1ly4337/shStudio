#pragma once
#include <windows.h>
#include "Modal.h"
#include "String.hpp"
#include "IconPicker.h"
#include "EditText.h"
#include "ComboBox.h"
#include "Button.h"
#include "BmpCtrl.h"

class StandardNPC;

class NpcEditor : public Modal
{
public:
    explicit NpcEditor(StandardNPC* npc);
    virtual ~NpcEditor();

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void reserved73() override;
    int mapCommandToResult(int id) override;

    virtual bool showModel(int modelId);
    virtual void createExtraControls() {}
    virtual bool applyExtra() { return true; }

    bool create(HWND parent);

public:
    StandardNPC* m_npc;
    String     m_title;
    IconPicker m_icon;
    EditText   m_name;
    EditText   m_welcome;
    ComboBox   m_faction;
    ComboBox   m_model;
    Button     m_expand;
    BmpCtrl    m_preview;
    bool       m_expanded;
    bool       m_tallWelcome;
    int        m_height;
    int        m_width;
};
