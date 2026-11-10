#pragma once
#include <windows.h>
#include "Modal.h"
#include "EditText.h"

class fPoint;

class ScaleEditor : public Modal
{
public:
    explicit ScaleEditor(fPoint* factors);
    virtual ~ScaleEditor();

    bool onCreate() override;
    int mapCommandToResult(int id) override;

    bool create(HWND parent);

public:
    fPoint*  m_factors;
    EditText m_editX;
    EditText m_editY;
    EditText m_editZ;
};
static_assert(sizeof(ScaleEditor) == 0xD4, "ScaleEditor size");
