#pragma once
#include <windows.h>
#include "Modal.h"
#include "EditText.h"

class GradeEditor : public Modal
{
public:
    GradeEditor() : m_grade(0) {}
    virtual ~GradeEditor();

    bool onCreate() override;
    int mapCommandToResult(int id) override;

    bool create(HWND parent);

public:
    int      m_grade;
    EditText m_gradeEdit;
};
static_assert(sizeof(GradeEditor) == 0xBC, "GradeEditor size");
