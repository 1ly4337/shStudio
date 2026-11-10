#pragma once
#include "CoordsEditor.h"

class CoordsEditor2 : public CoordsEditor
{
public:
    CoordsEditor2(DocView* view, kWorld::Element::Coord* coord);
    virtual ~CoordsEditor2() {}

    virtual bool create(HWND parent) override;
    virtual int createControls(int y) override;
    virtual void save() override;

public:
    EditText m_index;
};
static_assert(sizeof(CoordsEditor2) == 0x148, "CoordsEditor2 size");
