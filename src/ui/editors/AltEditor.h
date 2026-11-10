#pragma once
#include <windows.h>
#include "Modal.h"
#include "EditText.h"

class CharAction;

class AltEditor : public Modal
{
public:
    explicit AltEditor(CharAction* action);
    ~AltEditor() override;

    bool onCreate() override;
    int mapCommandToResult(int id) override;

    bool create(HWND parent);

public:
    CharAction* m_action;
    EditText    m_aniFile;
    EditText    m_mode;
    EditText    m_unk1;
    EditText    m_byte1;
    EditText    m_byte2;
    EditText    m_byte3;
    EditText    m_flt1;
    EditText    m_flt2;
    EditText    m_flt3;
};
static_assert(sizeof(AltEditor) == 0x11C, "AltEditor size");
