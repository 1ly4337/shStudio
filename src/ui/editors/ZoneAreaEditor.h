#pragma once
#include "WorldEditor.h"
#include "kWorld_Sounds.h"

class ZoneAreaEditor : public WorldEditor
{
public:
    ZoneAreaEditor(DocView* view, kWorld::Sounds::ZoneArea* area);
    virtual ~ZoneAreaEditor();

    virtual bool create(HWND parent) override;
    virtual int createControls(int y) override;
    virtual void save() override;

public:
    kWorld::Sounds::ZoneArea* m_area;
    EditText m_point1[3];
    EditText m_point2[3];
    EditText m_param1;
    EditText m_param2;
};
static_assert(sizeof(ZoneAreaEditor) == 0x130, "ZoneAreaEditor size");
