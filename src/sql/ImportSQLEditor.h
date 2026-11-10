#pragma once
#include <windows.h>
#include "Modal.h"
#include "Button.h"

class ImportSQLEditor : public Modal
{
public:
    explicit ImportSQLEditor(const wchar_t* table);
    virtual ~ImportSQLEditor() {}

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    int mapCommandToResult(int id) override;

public:
    const wchar_t* m_table;
    Button         m_captions;
    bool           m_importCaptions;
};
