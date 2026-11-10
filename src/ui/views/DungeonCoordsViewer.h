#pragma once
#include "Modal.h"
#include "fPoint.h"
#include "EditText.h"

class DungeonCoordsViewer : public Modal {
public:
    DungeonCoordsViewer(fPoint p1, fPoint p2, fPoint p3);
    ~DungeonCoordsViewer() override;
    bool create(HWND parent);

    bool onCreate() override;
    int  mapCommandToResult(int id) override;
    virtual int createControls(int y);

    fPoint   m_pt1;
    fPoint   m_pt2;
    fPoint   m_pt3;
    EditText m_edits[9];
};
static_assert(sizeof(DungeonCoordsViewer) == 0x148, "DungeonCoordsViewer size");
