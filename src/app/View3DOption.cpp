#include "View3DOption.h"
#include "IniFile.h"
#include "g2_inline.h"
#include "_imports_p07.h"

View3DOption::View3DOption()
{
}

View3DOption::~View3DOption()
{
}

void View3DOption::load(IniFile* prefs)
{
    m_showGear = prefs->getInt(L"3D View", L"show gear", 1) == 1;
    m_showWeapon = prefs->getInt(L"3D View", L"show weapon", 1) == 1;
    m_showMantle = prefs->getInt(L"3D View", L"show mantle", 1) == 1;
    m_showMonster = prefs->getInt(L"3D View", L"show monster", 1) == 1;
    m_showModels = prefs->getInt(L"3D View", L"show models", 1) == 1;
    m_gearCheck.setCheck(m_showGear);
    m_weaponCheck.setCheck(m_showWeapon);
    m_mantleCheck.setCheck(m_showMantle);
    m_monsterCheck.setCheck(m_showMonster);
    m_modelsCheck.setCheck(m_showModels);
}

void View3DOption::save(IniFile* prefs)
{
    prefs->writeInt(L"3D View", L"show gear", m_showGear != 0);
    prefs->writeInt(L"3D View", L"show weapon", m_showWeapon != 0);
    prefs->writeInt(L"3D View", L"show mantle", m_showMantle != 0);
    prefs->writeInt(L"3D View", L"show monster", m_showMonster != 0);
    prefs->writeInt(L"3D View", L"show models", m_showModels != 0);
}

bool View3DOption::onCreate()
{
    g2_createStatic(m_hWnd, L"Use the controls below to define if 3D views are shown when editors are opened.",
                    15, 10, 280, 32, 4);
    g2_createCheckBox(m_gearCheck, m_hWnd, L" Show 3D view of edited Gear", 15, 46, 200, 18, 0x65);
    g2_createCheckBox(m_weaponCheck, m_hWnd, L" Show 3D view of edited Weapon", 15, 66, 200, 18, 0x66);
    g2_createCheckBox(m_mantleCheck, m_hWnd, L" Show 3D view of edited Mantle", 15, 86, 200, 18, 0x67);
    g2_createCheckBox(m_monsterCheck, m_hWnd, L" Show 3D view of edited Monster", 15, 106, 200, 18, 0x68);
    g2_createCheckBox(m_modelsCheck, m_hWnd, L" Show a 3D view with models list", 15, 126, 200, 18, 0x69);
    return true;
}

bool View3DOption::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code)
        return Container::onCommand(code, ctrl, id);
    switch (id)
    {
    case 'e':
        m_showGear = !m_showGear;
        m_gearCheck.setCheck(m_showGear);
        return true;
    case 'f':
        m_showWeapon = !m_showWeapon;
        m_weaponCheck.setCheck(m_showWeapon);
        return true;
    case 'g':
        m_showMantle = !m_showMantle;
        m_mantleCheck.setCheck(m_showMantle);
        return true;
    case 'h':
        m_showMonster = !m_showMonster;
        m_monsterCheck.setCheck(m_showMonster);
        return true;
    }
    if (id != 105)
        return Container::onCommand(code, ctrl, id);
    m_showModels = !m_showModels;
    m_modelsCheck.setCheck(m_showModels);
    return true;
}
