#pragma once
#include <windows.h>
#include "Modal.h"
#include "vShort.h"
#include "IconPicker.h"
#include "EditText.h"
#include "ComboBox.h"
#include "Button.h"

class MobsSkill;

class MobSkillEditor : public Modal
{
public:
    explicit MobSkillEditor(MobsSkill* skill);
    virtual ~MobSkillEditor();

    virtual bool onCreate();
    virtual bool onCommand(UINT code, HWND ctrl, UINT id);
    virtual void reserved73();
    virtual int mapCommandToResult(int id);
    virtual bool buildControls();
    virtual void loadControls();
    virtual void saveControls();

    bool createEditor(HWND parent);

public:
    MobsSkill*  m_skill;
    vShort      m_abilityIds;
    IconPicker  m_icon;
    EditText    m_editName;
    EditText    m_editDescription;
    ComboBox    m_cbFamily;
    ComboBox    m_cbAttackType;
    ComboBox    m_cbEffect;
    ComboBox    m_cbState;
    ComboBox    m_cbDamageType;
    ComboBox    m_cbAbility[10];
    EditText    m_editAbilityValue[10];
    EditText    m_editSpellDelay;
    EditText    m_editResetTime;
    Button      m_chkSuccess;
    EditText    m_editSuccess;
    ComboBox    m_cbElement;
    ComboBox    m_cbTargets;
    EditText    m_editApplyRange;
    EditText    m_editAttackRange;
    EditText    m_editKeepTime;
    EditText    m_editMultiAttack;
    EditText    m_editDamage[3];
    EditText    m_editAddDamage[3];
    EditText    m_editTimeDamage[3];
    EditText    m_editHeal[3];
    EditText    m_editTimeHeal[3];
};
static_assert(sizeof(MobSkillEditor) == 0x658, "MobSkillEditor size");
