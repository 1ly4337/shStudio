#pragma once
#include "OptionPanel.h"
#include "Button.h"

class View3DOption : public OptionPanel
{
public:
    View3DOption();
    virtual ~View3DOption();

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void load(IniFile* prefs) override;
    void save(IniFile* prefs) override;

public:
    bool   m_showGear;
    bool   m_showWeapon;
    bool   m_showMantle;
    bool   m_showMonster;
    bool   m_showModels;
    Button m_gearCheck;
    Button m_weaponCheck;
    Button m_mantleCheck;
    Button m_monsterCheck;
    Button m_modelsCheck;
};
static_assert(sizeof(View3DOption) == 0xAC, "View3DOption size");
