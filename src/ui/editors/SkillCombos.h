#pragma once
#include <windows.h>

class ComboBox;
struct SkillData;

extern const wchar_t* g_skillStatNames[];
extern const wchar_t* g_weaponTypeNames[16];

void SkillCombo_fillDamageTypes(ComboBox& combo);
void SkillCombo_fillElements(ComboBox& combo);
void SkillCombo_fillStateTypes(ComboBox& combo);
void SkillCombo_fillToonTargets(ComboBox& combo, bool selectedEnemy);
void SkillCombo_fillMobTargets(ComboBox& combo);
void SkillCombo_fillFamilies(ComboBox& combo);
void SkillCombo_selectStat(ComboBox& combo, SkillData* levels, int levelCount, int stat);
