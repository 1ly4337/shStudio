#pragma once
#include "WorldEditor.h"
#include "kWorld_Element.h"

class CoordsEditor : public WorldEditor
{
public:
    CoordsEditor(DocView* view, kWorld::Element::Coord* coord);
    virtual ~CoordsEditor();

    virtual bool create(HWND parent) override;
    virtual int createControls(int y) override;
    virtual void save() override;

public:
    kWorld::Element::Coord* m_coord;
    EditText m_location[3];
    EditText m_rotation[3];
    EditText m_unknown[3];
};
static_assert(sizeof(CoordsEditor) == 0x13C, "CoordsEditor size");
