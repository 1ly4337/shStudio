#pragma once
#include "WorldEditor.h"

namespace kWorld { class Spawn; }

class SpawnAreaEditor : public WorldEditor
{
public:
    SpawnAreaEditor(DocView* view, kWorld::Spawn* spawn);
    virtual ~SpawnAreaEditor();

    virtual int mapCommandToResult(int id) override;
    virtual bool create(HWND parent) override;
    virtual int createControls(int y) override;
    virtual void save() override;

public:
    kWorld::Spawn* m_spawn;
    EditText m_point1[3];
    EditText m_point2[3];
    EditText m_faction;
    EditText m_unknown[3];
};
static_assert(sizeof(SpawnAreaEditor) == 0x148, "SpawnAreaEditor size");
