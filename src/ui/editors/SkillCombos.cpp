#include "SkillCombos.h"
#include <cwchar>
#include "ComboBox.h"
#include "SkillData.h"
#include "SkillEditorUi.h"
#include "_imports_H.h"

const wchar_t* g_skillStatNames[] = {
    L"None",
    L"Health (base)",
    L"Mana (base)",
    L"Stamina (base)",
    L"Strength (STR)",
    L"Reaction (REC)",
    L"Intelligence (INT)",
    L"Wisdom (WIS)",
    L"Dexterity (DEX)",
    L"Luck (LUC)",
    L"Health (regain)",
    L"Mana (regain)",
    L"Stamina (regain)",
    L"<unknown> (13)",
    L"<unknown> (14)",
    L"Attack Range",
    L"Attack Speed",
    L"Movement Speed",
    L"Critical Attack Rate",
    L"Spell Cooldown Time",
    L"Physical Attack Rate",
    L"Shooting Attack Rate",
    L"Magic Attack Rate",
    L"Physical Attack Power",
    L"Shooting Attack Power",
    L"Magic Attack Power",
    L"Physical Defense",
    L"Shooting Defense",
    L"Magic Resistance",
    L"Physical Evasion Rate",
    L"Shooting Evasion Rate",
    L"<unknown> (31)",
    L"Disable Physical Attack",
    L"Disable Shooting Attack",
    L"Disable Magic Attack",
    L"Experience Gain Rate",
    L"Endurance",
    L"Inventory Item Drop",
    L"Experience Loss Rate",
    L"Warehouse Recall",
    L"Blue Dragon Charm",
    L"White Tiger Charm",
    L"Red Pheonix Charm",
    L"Warehouse Size",
    L"Gold Drop Rate",
    L"Equipped Item Drop",
    L"Ressurection",
    L"<unknown> (47)",
    L"Absorption Aura",
    L"Cure of Goddess",
    L"<unknown> (50)",
    L"<unknown> (51)",
    L"Increase Health by STR",
    L"Increase Health by REC",
    L"Increase Health by INT",
    L"Increase Health by WIS",
    L"Increase Health by DEX",
    L"Increase Health by LUC",
    0,
};

const wchar_t* g_weaponTypeNames[16] = {
    L" 1-Handed Sword",
    L" 2-Handed Sword",
    L" 1-Handed Axe",
    L" 2-Handed Axe",
    L" Double Sword",
    L" Spear",
    L" 1-Handed Blunt",
    L" 2-Handed Blunt",
    L" Reverse Sword",
    L" Dagger",
    L" Javelin",
    L" Staff",
    L" Bow",
    L" Crossbow",
    L" Knuckle",
    L" Shield",
};

void SkillCombo_fillDamageTypes(ComboBox& combo)
{
    skui_addString(combo, L"Deals fixed damage");
    skui_addString(combo, L"Attack or magic power plus extra damage");
    skui_addString(combo, L"Deals the attack or magic power times coeff");
    skui_addString(combo, L"Deals damage equals to toon's HP times coeff");
    skui_addString(combo, L"Reduces the target's HP,SP,MP by a percent");
    skui_addString(combo, L"Damage equals to a percent of the target HP");
    skui_addString(combo, L"Damage equals to toon's REC times coeff");
    skui_addString(combo, L"Damage equals to toon's REC plus extra");
}

void SkillCombo_fillElements(ComboBox& combo)
{
    skui_addString(combo, L"none");
    skui_addString(combo, L"Fire 1");
    skui_addString(combo, L"Water 1");
    skui_addString(combo, L"Earth 1");
    skui_addString(combo, L"Wind 1");
    skui_addString(combo, L"Fire 2");
    skui_addString(combo, L"Water 2");
    skui_addString(combo, L"Earth 2");
    skui_addString(combo, L"Wind 2");
}

void SkillCombo_fillStateTypes(ComboBox& combo)
{
    skui_addString(combo, L"none");
    skui_addString(combo, L"Sleep");
    skui_addString(combo, L"Stun");
    skui_addString(combo, L"Darkness");
    skui_addString(combo, L"Silence");
    skui_addString(combo, L"Immobilize");
    skui_addString(combo, L"Slow");
    skui_addString(combo, L"Flat Damage");
    skui_addString(combo, L"Death's Touch");
    skui_addString(combo, L"HP damage over time");
    skui_addString(combo, L"SP damage over time");
    skui_addString(combo, L"MP damage over time");
    skui_addString(combo, L"Mental Smasher");
    skui_addString(combo, L"Lower Attack or Defense");
    skui_addString(combo, L"Lower Ability - Slump Shot");
    skui_addString(combo, L"Lower Ability - Misfortunate");
}

void SkillCombo_fillToonTargets(ComboBox& combo, bool selectedEnemy)
{
    skui_addString(combo, L"Any ennemy");
    skui_addString(combo, L"Caster");
    if (selectedEnemy)
        skui_addString(combo, L"Selected ennemy");
    else
        skui_addString(combo, L"Selected ally");
    skui_addString(combo, L"Allies near caster");
    skui_addString(combo, L"Allies but caster");
    skui_addString(combo, L"Ennemies near caster");
    skui_addString(combo, L"Ennemies near target");
    skui_addString(combo, L"Party members");
}

void SkillCombo_fillMobTargets(ComboBox& combo)
{
    combo.ComboBox::addItem(L"Any ennemy", 1);
    combo.ComboBox::addItem(L"Caster", 2);
    combo.ComboBox::addItem(L"Selected Character", 3);
    combo.ComboBox::addItem(L"Characters near caster", 6);
    combo.ComboBox::addItem(L"Characters near target", 7);
    combo.ComboBox::addItem(L"Type 10 (unknown)", 10);
    combo.ComboBox::addItem(L"Type 11 (unknown)", 11);
}

void SkillCombo_fillFamilies(ComboBox& combo)
{
    combo.ComboBox::addItem(L"0 - None", 0);
    combo.ComboBox::addItem(L"1 - Weapon Power Up", 1);
    combo.ComboBox::addItem(L"2 - Backpack", 2);
    combo.ComboBox::addItem(L"4 - Passive Defense", 4);
    combo.ComboBox::addItem(L"5 - Interpretation", 5);
    combo.ComboBox::addItem(L"6 - Potential Defense", 6);
    combo.ComboBox::addItem(L"8 - Weapon Mastery", 8);
    combo.ComboBox::addItem(L"10 - Defense Power Up", 10);
    combo.ComboBox::addItem(L"21 - Unique Hit Attack", 21);
    combo.ComboBox::addItem(L"22 - Multiple Hits Attack", 22);
    combo.ComboBox::addItem(L"23 - Eraser", 23);
    combo.ComboBox::addItem(L"25 - HP, MP, SP Reduction", 25);
    combo.ComboBox::addItem(L"27 - Energy Drain", 27);
    combo.ComboBox::addItem(L"41 - Healing", 41);
    combo.ComboBox::addItem(L"42 - Dispel", 42);
    combo.ComboBox::addItem(L"43 - Sacrifice", 43);
    combo.ComboBox::addItem(L"44 - Unknown Ep.6", 44);
    combo.ComboBox::addItem(L"61 - Periodical Debuff", 61);
    combo.ComboBox::addItem(L"62 - Subtracting Debuff", 62);
    combo.ComboBox::addItem(L"63 - Flite", 63);
    combo.ComboBox::addItem(L"64 - Stun", 64);
    combo.ComboBox::addItem(L"65 - Immobilize", 65);
    combo.ComboBox::addItem(L"66 - Sleep", 66);
    combo.ComboBox::addItem(L"67 - Prevent Attack", 67);
    combo.ComboBox::addItem(L"68 - Remove Attribute", 68);
    combo.ComboBox::addItem(L"69 - Taunt", 69);
    combo.ComboBox::addItem(L"72 - Provoke", 72);
    combo.ComboBox::addItem(L"74 - Energy Blackhole", 74);
    combo.ComboBox::addItem(L"75 - Mental Storm, Confusion", 75);
    combo.ComboBox::addItem(L"76 - Soul Menace", 76);
    combo.ComboBox::addItem(L"77 - Mental Storm, Distortion", 77);
    combo.ComboBox::addItem(L"78 - unknow (mobs)", 78);
    combo.ComboBox::addItem(L"81 - Periodical Heal", 81);
    combo.ComboBox::addItem(L"82 - Buff, Noss, …", 82);
    combo.ComboBox::addItem(L"83 - Damage Reflection", 83);
    combo.ComboBox::addItem(L"84 - Fire thorn", 84);
    combo.ComboBox::addItem(L"85 - Block Magic Attack", 85);
    combo.ComboBox::addItem(L"86 - Block Shooting Attack", 86);
    combo.ComboBox::addItem(L"87 - Health Assistant", 87);
    combo.ComboBox::addItem(L"88 - Stealth", 88);
    combo.ComboBox::addItem(L"90 - Evolution", 90);
    combo.ComboBox::addItem(L"91 - Ability Exchange", 91);
    combo.ComboBox::addItem(L"92 - Untouchable", 92);
    combo.ComboBox::addItem(L"93 - Detection", 93);
    combo.ComboBox::addItem(L"94 - Elemental Attack", 94);
    combo.ComboBox::addItem(L"95 - Elemental Protection", 95);
    combo.ComboBox::addItem(L"96 - Dance of Death", 96);
    combo.ComboBox::addItem(L"97 - Mass Ambush", 97);
    combo.ComboBox::addItem(L"98 - Etain Shield", 98);
    combo.ComboBox::addItem(L"99 - undefined", 99);
    combo.ComboBox::addItem(L"100 - undefined", 100);
    combo.ComboBox::addItem(L"101 - Trap", 101);
    combo.ComboBox::addItem(L"102 - Firebush", 102);
    combo.ComboBox::addItem(L"121 - Resurrection", 121);
    combo.ComboBox::addItem(L"122 - Soul Burn", 122);
    combo.ComboBox::addItem(L"123 - Scouting", 123);
    combo.ComboBox::addItem(L"125 - Town Portal", 125);
    combo.ComboBox::addItem(L"126 - Mental Impact", 126);
    combo.ComboBox::addItem(L"130 - Unknown Ep.6", 130);
    combo.ComboBox::addItem(L"131 - Unknown Ep.6", 131);
    combo.ComboBox::addItem(L"150 - Unknown Ep.6", 150);
    combo.ComboBox::addItem(L"1062 - Cure of Goddess", 1062);
    combo.ComboBox::addItem(L"1099 - Unknown Ep.6", 1099);
}

void SkillCombo_selectStat(ComboBox& combo, SkillData* levels, int levelCount, int stat)
{
    int level = 0;
    if (levelCount <= 0)
    {
        SendMessageW(combo.m_hWnd, CB_SETCURSEL, 0, 0);
        return;
    }
    const unsigned char* type = &levels[0].ability[stat].type;
    while (!*type)
    {
        if (++level >= levelCount)
        {
            SendMessageW(combo.m_hWnd, CB_SETCURSEL, 0, 0);
            return;
        }
        type += sizeof(SkillData);
    }
    WPARAM id = *type;
    if (!combo.ComboBox::selectData(id))
    {
        wchar_t buffer[32];
        swprintf_s(buffer, 32, L"Unknown %i", (int)id);
        combo.ComboBox::addItem(buffer, (int)id);
        combo.ComboBox::selectData(id);
    }
}
