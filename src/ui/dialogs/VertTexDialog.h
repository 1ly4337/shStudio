#pragma once
#include <windows.h>
#include "Modal.h"
#include "String.hpp"
#include "EditText.h"
#include "Button.h"

class VertTexDialog : public Modal
{
public:
    explicit VertTexDialog(const wchar_t* folder);
    ~VertTexDialog() override;

    bool create(HWND parent);

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    int mapCommandToResult(int id) override;

public:
    String   m_folder;
    String   m_vertFile;
    String   m_texFile;
    EditText m_vertEdit;
    EditText m_texEdit;
    Button   m_vertBrowse;
    Button   m_texBrowse;
};
static_assert(sizeof(VertTexDialog) == 0x13C, "VertTexDialog size");
