#pragma once
#include <windows.h>
#include "NpcEditor.h"
#include "EditText.h"

class GateKeeper;

class GateKeeperEditor : public NpcEditor
{
public:
    struct Destination
    {
        EditText name;
        EditText mapId;
        EditText x;
        EditText y;
        EditText z;
        EditText cost;
    };

    explicit GateKeeperEditor(GateKeeper* gateKeeper);
    virtual ~GateKeeperEditor();

    void createExtraControls() override;
    bool applyExtra() override;

    static bool edit(HWND parent, GateKeeper* gateKeeper);

public:
    Destination m_dest[3];
};
