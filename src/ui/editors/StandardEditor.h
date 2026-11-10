#pragma once
#include <windows.h>
#include "NpcEditor.h"
#include "ComboBox.h"

class StandardNPC;
class Window;

class StandardEditor : public NpcEditor
{
public:
    StandardEditor(StandardNPC* npc, bool canChangeFamily);
    virtual ~StandardEditor();

    void createExtraControls() override;
    bool applyExtra() override;

public:
    ComboBox m_family;
    bool     m_canChangeFamily;
};

bool __fastcall editStandardNpc(Window* owner, StandardNPC* npc);
