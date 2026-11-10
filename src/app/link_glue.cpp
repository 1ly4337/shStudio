#include <windows.h>
#include "Control32.h"
#include "Modal.h"
#include "AniFile.h"
#include "TacticsZones.h"
#include "MonsterModels.h"
#include "MonsterModel.h"
#include "ModelsMLX.h"
#include "CharActions.h"
#include "Faces.h"
#include "Face.h"
#include "oStream.hpp"
#include "String.hpp"
#include "BagsList.h"

int Control32::getFontId() { return m_fontId; }

Modal::~Modal() {}

bool AniFile::isEmpty() { return m_steps.size() == 0; }

void TacticsZones::clear()  { deleteAll(); }
void MonsterModels::clear() { ptrArray<MonsterModel*>::deleteAll(); }
void ModelsMLX::clear()     { deleteAll(); }
void CharActions::clear()   { deleteAll(); }

bool MonsterModels::toText(String& out, DWORD fmt) const
{
    bool ok = true;
    for (int i = 0; i < m_count && ok; ++i)
        ok = m_items[i]->toText(out, fmt);
    return ok;
}

void Faces::write(oStream& out, DWORD fmt) const
{
    out << (unsigned long)m_count;
    for (int i = 0; i < m_count; ++i)
        m_items[i]->write(out, fmt);
}

BagsList::Items::Items() : ptrArray<BagsList::Item*>(10) {}

char __cdecl sub_41A900(int format)
{
    if (format <= 0x64756D70)
    {
        if (format == 0x64756D70 || format == 0x2D2D2D2D  || format == 0x63737620 )
            return ';';
        return ',';
    }
    if (format == 0x73716C20)
        return ',';
    if (format == 0x74616273)
        return 0x09;
    return ',';
}

int __fastcall sub_41D430(short bcd)
{
    unsigned char lo = (unsigned char)bcd, hi = (unsigned char)((unsigned short)bcd >> 8);
    return (unsigned char)((lo & 0xF) + 10 * (lo >> 4)) + (unsigned short)(100 * (unsigned char)((hi & 0xF) + 10 * (hi >> 4)));
}

int dword_5A0120 = GetSystemMetrics(SM_CYMENU) + 5;
int dword_5A0124 = GetSystemMetrics(SM_CYMENUSIZE);

bool byte_59FC6A = false;

const wchar_t* off_59A770[2] = { L"SData File", L"*.sdata" };

wchar_t* g_skillColumnNames[100] = {
    (wchar_t*)L"Fighter", (wchar_t*)L"Defenser", (wchar_t*)L"Ranger", (wchar_t*)L"Archer", (wchar_t*)L"Mage",
    (wchar_t*)L"Priest", (wchar_t*)L"PrevSkill", (wchar_t*)L"ReqLevel", (wchar_t*)L"Grow", (wchar_t*)L"SkillPoint",
    (wchar_t*)L"TypeShow", (wchar_t*)L"TypeAttack", (wchar_t*)L"TypeEffect", (wchar_t*)L"TypeDetail",
    (wchar_t*)L"Need Wp 1", (wchar_t*)L"Need Wp 2", (wchar_t*)L"Need Wp 3", (wchar_t*)L"Need Wp 4", (wchar_t*)L"Need Wp 5",
    (wchar_t*)L"Need Wp 6", (wchar_t*)L"Need Wp 7", (wchar_t*)L"Need Wp 8", (wchar_t*)L"Need Wp 9", (wchar_t*)L"Need Wp 10",
    (wchar_t*)L"Need Wp 11", (wchar_t*)L"Need Wp 12", (wchar_t*)L"Need Wp 13", (wchar_t*)L"Need Wp 14", (wchar_t*)L"Need Wp 15",
    (wchar_t*)L"Shield", (wchar_t*)L"SP", (wchar_t*)L"MP", (wchar_t*)L"ReadyTime", (wchar_t*)L"ResetTime",
    (wchar_t*)L"AttackRange", (wchar_t*)L"StateType", (wchar_t*)L"AttrType", (wchar_t*)L"Disable", (wchar_t*)L"SuccessType",
    (wchar_t*)L"SuccessValue", (wchar_t*)L"TargetType", (wchar_t*)L"ApplyRange", (wchar_t*)L"MultiAttack", (wchar_t*)L"KeepTime",
    (wchar_t*)L"Weapon1", (wchar_t*)L"Weapon2", (wchar_t*)L"WeaponValue", (wchar_t*)L"Bag", (wchar_t*)L"Aggro",
    (wchar_t*)L"DamageType", (wchar_t*)L"DamageHP", (wchar_t*)L"DamageSP", (wchar_t*)L"DamageMP", (wchar_t*)L"TimeDamageType",
    (wchar_t*)L"TimeDamageHP", (wchar_t*)L"TimeDamageSP", (wchar_t*)L"TimeDamageMP", (wchar_t*)L"AddDamageHP",
    (wchar_t*)L"AddDamageSP", (wchar_t*)L"AddDamageMP",
    (wchar_t*)L"AT1", (wchar_t*)L"AV1", (wchar_t*)L"AT2", (wchar_t*)L"AV2", (wchar_t*)L"AT3", (wchar_t*)L"AV3",
    (wchar_t*)L"AT4", (wchar_t*)L"AV4", (wchar_t*)L"AT5", (wchar_t*)L"AV5", (wchar_t*)L"AT6", (wchar_t*)L"AV6",
    (wchar_t*)L"AT7", (wchar_t*)L"AV7", (wchar_t*)L"AT8", (wchar_t*)L"AV8", (wchar_t*)L"AT9", (wchar_t*)L"AV9",
    (wchar_t*)L"AT10", (wchar_t*)L"AV10",
    (wchar_t*)L"HealHP", (wchar_t*)L"HealSP", (wchar_t*)L"HealMP", (wchar_t*)L"TimeHealHP", (wchar_t*)L"TimeHealSP",
    (wchar_t*)L"TimeHealMP", (wchar_t*)L"DefenceType", (wchar_t*)L"DefenceValue", (wchar_t*)L"LimitHP", (wchar_t*)L"FixRange",
    (wchar_t*)L"ChangeType", (wchar_t*)L"ChangeLevel", 0
};
DWORD g_skillColumnTags[100] = {
    0x66696768, 0x64656673, 0x726E6772, 0x61726368, 0x6D616765, 0x70727374, 0x70727673, 0x726C766C, 0x67726F77, 0x706E7473,
    0x74707368, 0x74706174, 0x74706566, 0x74706474, 0x6E777031, 0x6E777032, 0x6E777033, 0x6E777034, 0x6E777035, 0x6E777036,
    0x6E777037, 0x6E777038, 0x6E777039, 0x6E777041, 0x6E777042, 0x6E777043, 0x6E777044, 0x6E777045, 0x6E777046, 0x73686C64,
    0x63737370, 0x63736D70, 0x7264746D, 0x7273746D, 0x61747267, 0x73747470, 0x61747470, 0x64697361, 0x73637470, 0x7363766C,
    0x74727470, 0x61707267, 0x6D756C74, 0x6B70746D, 0x77706E31, 0x77706E32, 0x7776616C, 0x62616773, 0x61727277, 0x646D7470,
    0x646D6870, 0x646D7370, 0x646D6D70, 0x74647470, 0x74646870, 0x74647370, 0x74646D70, 0x61646870, 0x61647370, 0x61646D70,
    0x61627431, 0x61627631, 0x61627432, 0x61627632, 0x61627433, 0x61627633, 0x61627434, 0x61627634, 0x61627435, 0x61627635,
    0x61627436, 0x61627636, 0x61627437, 0x61627637, 0x61627438, 0x61627638, 0x61627439, 0x61627639, 0x6162743A, 0x6162763A,
    0x686C6870, 0x686C7370, 0x686C6D70, 0x74686870, 0x74687370, 0x74686D70, 0x64667470, 0x6466766C, 0x6C6D6870, 0x66787267,
    0x63687470, 0x63686C76, 0
};
