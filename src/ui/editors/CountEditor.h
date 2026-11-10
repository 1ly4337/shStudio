#pragma once
#include <windows.h>
#include "Modal.h"
#include "EditText.h"

class CountEditor : public Modal
{
public:
    CountEditor();
    virtual ~CountEditor();

    bool onCreate() override;
    int mapCommandToResult(int id) override;

    bool create(HWND parent);

public:
    int      m_type;
    int      m_typeId;
    int      m_count;
    EditText m_typeEdit;
    EditText m_typeIdEdit;
    EditText m_countEdit;
};
static_assert(sizeof(CountEditor) == 0xDC, "CountEditor size");
