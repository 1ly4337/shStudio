#pragma once
#include <windows.h>
#include "Modal.h"
#include "EditText.h"

class TacticsZone;

class ZoneEditor : public Modal
{
public:
    explicit ZoneEditor(TacticsZone* zone);
    virtual ~ZoneEditor();

    bool onCreate() override;
    int mapCommandToResult(int id) override;

    bool create(HWND parent);

public:
    TacticsZone* m_zone;
    EditText     m_map;
    EditText     m_x[4];
    EditText     m_z[4];
    EditText     m_params[2];
    EditText     m_desc;
};
