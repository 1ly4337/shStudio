#pragma once
#include "OptionPanel.h"
#include "Button.h"
#include "ComboBox.h"

class SDataFileOption : public OptionPanel
{
public:
    SDataFileOption();
    virtual ~SDataFileOption();

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void load(IniFile* prefs) override;
    void save(IniFile* prefs) override;

public:
    bool     m_cleanCR;
    bool     m_cleanMerchant;
    bool     m_dbPrimary;
    Button   m_cleanCRCheck;
    Button   m_cleanMerchantCheck;
    ComboBox m_languageCombo;
    Button   m_dbPrimaryCheck;
};
static_assert(sizeof(SDataFileOption) == 0xB8, "SDataFileOption size");
