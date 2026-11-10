#pragma once
#include "WorldEditor.h"
#include "ListCtrl.h"
#include "Button.h"

namespace kWorld { class NPC; }

class WldNpcEditor : public WorldEditor
{
public:
    WldNpcEditor(DocView* view, kWorld::NPC* npc);
    virtual ~WldNpcEditor();

    virtual bool onCommand(UINT code, HWND ctrl, UINT id) override;
    virtual bool onNotify(NMHDR* hdr, LRESULT* result) override;
    virtual bool create(HWND parent) override;
    virtual int createControls(int y) override;
    virtual void save() override;

public:
    kWorld::NPC* m_npc;
    EditText m_type;
    EditText m_id;
    EditText m_position[3];
    EditText m_orientation;
    EditText m_unused[3];
    ListCtrl m_positions;
    Button   m_add;
    Button   m_del;
};
static_assert(sizeof(WldNpcEditor) == 0x248, "WldNpcEditor size");
