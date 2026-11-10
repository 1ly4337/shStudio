#pragma once
#include "OptionPanel.h"
#include "String.hpp"
#include "Button.h"
#include "EditText.h"

class ItemOption : public OptionPanel
{
public:
    ItemOption();
    virtual ~ItemOption();

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void load(IniFile* prefs) override;
    void save(IniFile* prefs) override;
    bool validate() override;

public:
    bool     m_insertEnhancement;
    String   m_pattern;
    Button   m_insertCheck;
    EditText m_patternEdit;
};
static_assert(sizeof(ItemOption) == 0xA4, "ItemOption size");
