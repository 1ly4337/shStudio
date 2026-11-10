#pragma once
#include "CoordsEditor.h"

namespace kWorld { class Vertice; }

class VerticeEditor : public CoordsEditor
{
public:
    VerticeEditor(DocView* view, kWorld::Vertice* vertice);
    virtual ~VerticeEditor();

    virtual bool create(HWND parent) override;
    virtual int createControls(int y) override;
    virtual void save() override;

public:
    kWorld::Element::Coord m_localCoord;
    kWorld::Vertice*       m_vertice;
    EditText               m_effect;
};
static_assert(sizeof(VerticeEditor) == 0x1A0, "VerticeEditor size");
