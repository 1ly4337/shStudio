#pragma once
#include "OptionPanel.h"
#include "FolderPicker.h"
#include "Button.h"

class FolderOption : public OptionPanel
{
public:
    FolderOption();
    virtual ~FolderOption();

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void load(IniFile* prefs) override;
    void save(IniFile* prefs) override;

    static bool __stdcall isClientFolder(const wchar_t* path);

public:
    FolderPicker m_picker;
    Button       m_autoConnect;
};
static_assert(sizeof(FolderOption) == 0xF4, "FolderOption size");
