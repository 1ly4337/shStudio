#pragma once
#include <windows.h>
#include "Modal.h"
#include "EditText.h"

class GradeSets;

class GetGradeDialog : public Modal
{
public:
    explicit GetGradeDialog(GradeSets* sets);
    virtual ~GetGradeDialog();

    virtual bool onCreate();
    virtual void onShowDialog();
    virtual int mapCommandToResult(int id);

    bool open(HWND parent);

public:
    GradeSets* m_sets;
    short      m_grade;
    EditText   m_edit;
};
static_assert(sizeof(GetGradeDialog) == 0xC0, "GetGradeDialog size");
