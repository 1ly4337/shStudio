#pragma once
#include "WorldEditor.h"
#include "kWorld_Sounds.h"

class SpotAreaEditor : public WorldEditor
{
public:
    SpotAreaEditor(DocView* view, kWorld::Sounds::SpotArea* area);
    virtual ~SpotAreaEditor();

    virtual bool create(HWND parent) override;
    virtual int createControls(int y) override;
    virtual void save() override;

public:
    kWorld::Sounds::SpotArea* m_area;
    EditText m_center[3];
    EditText m_radius;
};
static_assert(sizeof(SpotAreaEditor) == 0x100, "SpotAreaEditor size");
