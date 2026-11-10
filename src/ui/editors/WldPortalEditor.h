#pragma once
#include "WorldEditor.h"

namespace kWorld { class Portal; }

class WldPortalEditor : public WorldEditor
{
public:
    WldPortalEditor(DocView* view, kWorld::Portal* portal);
    virtual ~WldPortalEditor();

    virtual bool create(HWND parent) override;
    virtual int createControls(int y) override;
    virtual void save() override;

public:
    kWorld::Portal* m_portal;
    EditText m_point1[3];
    EditText m_point2[3];
    EditText m_location[3];
    EditText m_map;
    EditText m_faction;
    EditText m_unknown;
};
static_assert(sizeof(WldPortalEditor) == 0x160, "WldPortalEditor size");
