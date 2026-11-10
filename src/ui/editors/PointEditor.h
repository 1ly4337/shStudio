#pragma once
#include <windows.h>
#include "Modal.h"
#include "EditText.h"

class fPoint;

class PointEditor : public Modal
{
public:
    explicit PointEditor(fPoint* point);
    virtual ~PointEditor() {}

    virtual bool onCreate() override;
    virtual int mapCommandToResult(int id) override;

    bool create(HWND parent);

public:
    fPoint*  m_point;
    EditText m_x;
    EditText m_y;
    EditText m_z;
};
static_assert(sizeof(PointEditor) == 0xD4, "PointEditor size");
