#pragma once
#include <windows.h>
#include "Modal.h"
#include "String.hpp"
#include "vShort.h"
#include "IconPicker.h"
#include "EditText.h"
#include "ComboBox.h"
#include "Button.h"

class ToonSkill;

class ToonSkillEditor : public Modal
{
public:
    explicit ToonSkillEditor(ToonSkill* skill);
    virtual ~ToonSkillEditor();

    virtual bool onCreate();
    virtual bool onCommand(UINT code, HWND ctrl, UINT id);
    virtual int mapCommandToResult(int id);
    virtual bool createEditor(HWND parent);
    virtual int buildControls();
    virtual SIZE createTypeControls(int y, short type);
    virtual void loadControls();
    virtual bool saveControls();

    SIZE createWeaponControls(int y);
    SIZE createBagsControls(int y);

    SIZE createEnhanceControls(int y);
    SIZE createLevelDescriptionControls(int y);
    SIZE createHpLimitControls(int y);
    SIZE createAttackControls(int y, short type);
    int  createStatRows(int y, int* maxWidth, int count);
    void enableStatColumn(int column, bool enable);

public:
    ToonSkill*  m_skill;
    String      m_descriptions[15];
    vShort      m_statIds;
    IconPicker  m_icon;
    EditText    m_name;
    ComboBox    m_family;
    ComboBox    m_category;
    EditText    m_animation;
    EditText    m_description;
    Button      m_editDescription;
    Button      m_classes[6];
    ComboBox    m_faction;
    ComboBox    m_mode;
    ComboBox    m_type;
    ComboBox    m_effect;
    Button      m_weapons[16];
    ComboBox    m_stateType;
    ComboBox    m_damageType;
    ComboBox    m_requiredWeapon1;
    ComboBox    m_requiredWeapon2;
    EditText    m_weapon[15];
    EditText    m_bags[3];
    EditText    m_levelDescriptions[3];
    ComboBox    m_statTypes[10];
    EditText    m_statValues[15][10];
    EditText    m_reqLevel[15];
    EditText    m_points[15];
    EditText    m_costSP[15];
    EditText    m_costMP[15];
    EditText    m_castTime[15];
    EditText    m_resetTime[15];
    Button      m_chanceEnabled[15];
    EditText    m_chance[15];
    EditText    m_hpLimit[15];
    EditText    m_fixRange[15];
    ComboBox    m_element[15];
    ComboBox    m_target[15];
    EditText    m_range[15];
    EditText    m_distance[15];
    EditText    m_keepTime[15];
    EditText    m_hits[15];
    EditText    m_baseDmgHP[15];
    EditText    m_baseDmgSP[15];
    EditText    m_baseDmgMP[15];
    EditText    m_addDmgHP[15];
    EditText    m_addDmgSP[15];
    EditText    m_addDmgMP[15];
    EditText    m_dotHP[15];
    EditText    m_dotSP[15];
    EditText    m_dotMP[15];
    EditText    m_healHP[15];
    EditText    m_healSP[15];
    EditText    m_healMP[15];
    EditText    m_hotHP[15];
    EditText    m_hotSP[15];
    EditText    m_hotMP[15];
    ComboBox    m_defenceType;
    EditText    m_defence[15];
    EditText    m_aggro[15];
};
static_assert(sizeof(ToonSkillEditor) == 0x2AEC, "ToonSkillEditor size");
