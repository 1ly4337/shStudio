#pragma once
#include "WorldEditor.h"

namespace kWorld { class NamedArea; }

class NamedAreaEditor : public WorldEditor
{
public:
    NamedAreaEditor(DocView* view, kWorld::NamedArea* area);
    virtual ~NamedAreaEditor() {}

    virtual bool create(HWND parent) override;
    virtual int createControls(int y) override;
    virtual void save() override;

public:
    kWorld::NamedArea* m_area;
    EditText m_point1[3];
    EditText m_point2[3];
    EditText m_k1;
    EditText m_mode;
    EditText m_k3;
};
static_assert(sizeof(NamedAreaEditor) == 0x13C, "NamedAreaEditor size");
