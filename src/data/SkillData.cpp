#include "FlntRows.h"
#include "SkillData.h"
#include "String.hpp"
#include "iStream.h"
#include "oStream.h"
#include "RecordSet.h"
#include "SkillText.h"
#include "_imports_w1.h"
#include <cstring>

namespace {

template <class T> inline void assignField(T& dst, int value) { dst = (T)value; }

template <class T> inline int cmpField(T a, T b)
{
    if (a == b)
        return 0;
    return a < b ? -1 : 1;
}

}

SkillData::SkillData()
{
    int keepId = skillId;
    memset(this, 0, 0xA4);
    skillId = keepId;
    version = 0x500;
}

int SkillData::getValue(DWORD tag) const
{
    switch (tag)
    {
    case 0x736B6964 : return skillId;
    case 0x6C76656C : return level;
    case 0x69636F6E : return icon;
    case 0x616E696D : return anim;
    case 0x65666374 : return effect;
    case 0x746F7470 : return toggleType;
    case 0x736F6E64 : return sound;
    case 0x726C766C : return reqLevel;
    case 0x636E7472 : return country;
    case 0x66696768 : return classes[0];
    case 0x64656673 : return classes[1];
    case 0x726E6772 : return classes[2];
    case 0x61726368 : return classes[3];
    case 0x6D616765 : return classes[4];
    case 0x70727374 : return classes[5];
    case 0x67726F77 : return grow;
    case 0x706E7473 : return points;
    case 0x74707368 : return typeShow;
    case 0x74706174 : return typeAttack;
    case 0x74706566 : return typeEffect;
    case 0x74706474 : return typeDetail;
    case 0x6E777031 : return needWeapon[0];
    case 0x6E777032 : return needWeapon[1];
    case 0x6E777033 : return needWeapon[2];
    case 0x6E777034 : return needWeapon[3];
    case 0x6E777035 : return needWeapon[4];
    case 0x6E777036 : return needWeapon[5];
    case 0x6E777037 : return needWeapon[6];
    case 0x6E777038 : return needWeapon[7];
    case 0x6E777039 : return needWeapon[8];
    case 0x6E777041 : return needWeapon[9];
    case 0x6E777042 : return needWeapon[10];
    case 0x6E777043 : return needWeapon[11];
    case 0x6E777044 : return needWeapon[12];
    case 0x6E777045 : return needWeapon[13];
    case 0x6E777046 : return needWeapon[14];
    case 0x73686C64 : return shield;
    case 0x63737370 : return sp;
    case 0x63736D70 : return mp;
    case 0x7264746D : return readyTime;
    case 0x7273746D : return resetTime;
    case 0x61747267 : return attackRange;
    case 0x73747470 : return stateType;
    case 0x61747470 : return attribType;
    case 0x64697361 : return disable;
    case 0x70727673 : return prevSkill;
    case 0x73637470 : return successType;
    case 0x7363766C : return successValue;
    case 0x74727470 : return targetType;
    case 0x61707267 : return applyRange;
    case 0x6D756C74 : return multiAttack;
    case 0x6B70746D : return keepTime;
    case 0x77706E31 : return weapon[0];
    case 0x77706E32 : return weapon[1];
    case 0x7776616C : return weaponValue;
    case 0x62616773 : return bag;
    case 0x61727277 : return arrow;
    case 0x646D7470 : return damageType;
    case 0x646D6870 : return damage[0];
    case 0x646D7370 : return damage[1];
    case 0x646D6D70 : return damage[2];
    case 0x74647470 : return timeDamageType;
    case 0x74646870 : return timeDamage[0];
    case 0x74647370 : return timeDamage[1];
    case 0x74646D70 : return timeDamage[2];
    case 0x61646870 : return addDamage[0];
    case 0x61647370 : return addDamage[1];
    case 0x61646D70 : return addDamage[2];
    case 0x61627431 : return ability[0].type;
    case 0x61627432 : return ability[1].type;
    case 0x61627433 : return ability[2].type;
    case 0x61627434 : return ability[3].type;
    case 0x61627435 : return ability[4].type;
    case 0x61627436 : return ability[5].type;
    case 0x61627437 : return ability[6].type;
    case 0x61627438 : return ability[7].type;
    case 0x61627439 : return ability[8].type;
    case 0x6162743A : return ability[9].type;
    case 0x61627631 : return ability[0].value;
    case 0x61627632 : return ability[1].value;
    case 0x61627633 : return ability[2].value;
    case 0x61627634 : return ability[3].value;
    case 0x61627635 : return ability[4].value;
    case 0x61627636 : return ability[5].value;
    case 0x61627637 : return ability[6].value;
    case 0x61627638 : return ability[7].value;
    case 0x61627639 : return ability[8].value;
    case 0x6162763A : return ability[9].value;
    case 0x686C6870 : return heal[0];
    case 0x686C7370 : return heal[1];
    case 0x686C6D70 : return heal[2];
    case 0x74686870 : return timeHeal[0];
    case 0x74687370 : return timeHeal[1];
    case 0x74686D70 : return timeHeal[2];
    case 0x64667470 : return defenceType;
    case 0x6466766C : return defenceValue;
    case 0x6C6D6870 : return limitHP;
    case 0x66787267 : return fixRange;
    case 0x63687470 : return changeType;
    case 0x63686C76 : return changeLevel;
    default:
        return 0;
    }
}

bool SkillData::setValue(DWORD tag, int value)
{
    switch (tag)
    {
    case 0x736B6964 : assignField(skillId, value); return true;
    case 0x6C76656C : assignField(level, value); return true;
    case 0x69636F6E : assignField(icon, value); return true;
    case 0x616E696D : assignField(anim, value); return true;
    case 0x65666374 : assignField(effect, value); return true;
    case 0x746F7470 : assignField(toggleType, value); return true;
    case 0x736F6E64 : assignField(sound, value); return true;
    case 0x726C766C : assignField(reqLevel, value); return true;
    case 0x636E7472 : assignField(country, value); return true;
    case 0x66696768 : assignField(classes[0], value); return true;
    case 0x64656673 : assignField(classes[1], value); return true;
    case 0x726E6772 : assignField(classes[2], value); return true;
    case 0x61726368 : assignField(classes[3], value); return true;
    case 0x6D616765 : assignField(classes[4], value); return true;
    case 0x70727374 : assignField(classes[5], value); return true;
    case 0x67726F77 : assignField(grow, value); return true;
    case 0x706E7473 : assignField(points, value); return true;
    case 0x74707368 : assignField(typeShow, value); return true;
    case 0x74706174 : assignField(typeAttack, value); return true;
    case 0x74706566 : assignField(typeEffect, value); return true;
    case 0x74706474 : assignField(typeDetail, value); return true;
    case 0x6E777031 : assignField(needWeapon[0], value); return true;
    case 0x6E777032 : assignField(needWeapon[1], value); return true;
    case 0x6E777033 : assignField(needWeapon[2], value); return true;
    case 0x6E777034 : assignField(needWeapon[3], value); return true;
    case 0x6E777035 : assignField(needWeapon[4], value); return true;
    case 0x6E777036 : assignField(needWeapon[5], value); return true;
    case 0x6E777037 : assignField(needWeapon[6], value); return true;
    case 0x6E777038 : assignField(needWeapon[7], value); return true;
    case 0x6E777039 : assignField(needWeapon[8], value); return true;
    case 0x6E777041 : assignField(needWeapon[9], value); return true;
    case 0x6E777042 : assignField(needWeapon[10], value); return true;
    case 0x6E777043 : assignField(needWeapon[11], value); return true;
    case 0x6E777044 : assignField(needWeapon[12], value); return true;
    case 0x6E777045 : assignField(needWeapon[13], value); return true;
    case 0x6E777046 : assignField(needWeapon[14], value); return true;
    case 0x73686C64 : assignField(shield, value); return true;
    case 0x63737370 : assignField(sp, value); return true;
    case 0x63736D70 : assignField(mp, value); return true;
    case 0x7264746D : assignField(readyTime, value); return true;
    case 0x7273746D : assignField(resetTime, value); return true;
    case 0x61747267 : assignField(attackRange, value); return true;
    case 0x73747470 : assignField(stateType, value); return true;
    case 0x61747470 : assignField(attribType, value); return true;
    case 0x64697361 : assignField(disable, value); return true;
    case 0x70727673 : assignField(prevSkill, value); return true;
    case 0x73637470 : assignField(successType, value); return true;
    case 0x7363766C : assignField(successValue, value); return true;
    case 0x74727470 : assignField(targetType, value); return true;
    case 0x61707267 : assignField(applyRange, value); return true;
    case 0x6D756C74 : assignField(multiAttack, value); return true;
    case 0x6B70746D : assignField(keepTime, value); return true;
    case 0x77706E31 : assignField(weapon[0], value); return true;
    case 0x77706E32 : assignField(weapon[1], value); return true;
    case 0x7776616C : assignField(weaponValue, value); return true;
    case 0x62616773 : assignField(bag, value); return true;
    case 0x61727277 : assignField(arrow, value); return true;
    case 0x646D7470 : assignField(damageType, value); return true;
    case 0x646D6870 : assignField(damage[0], value); return true;
    case 0x646D7370 : assignField(damage[1], value); return true;
    case 0x646D6D70 : assignField(damage[2], value); return true;
    case 0x74647470 : assignField(timeDamageType, value); return true;
    case 0x74646870 : assignField(timeDamage[0], value); return true;
    case 0x74647370 : assignField(timeDamage[1], value); return true;
    case 0x74646D70 : assignField(timeDamage[2], value); return true;
    case 0x61646870 : assignField(addDamage[0], value); return true;
    case 0x61647370 : assignField(addDamage[1], value); return true;
    case 0x61646D70 : assignField(addDamage[2], value); return true;
    case 0x61627431 : assignField(ability[0].type, value); return true;
    case 0x61627432 : assignField(ability[1].type, value); return true;
    case 0x61627433 : assignField(ability[2].type, value); return true;
    case 0x61627434 : assignField(ability[3].type, value); return true;
    case 0x61627435 : assignField(ability[4].type, value); return true;
    case 0x61627436 : assignField(ability[5].type, value); return true;
    case 0x61627437 : assignField(ability[6].type, value); return true;
    case 0x61627438 : assignField(ability[7].type, value); return true;
    case 0x61627439 : assignField(ability[8].type, value); return true;
    case 0x6162743A : assignField(ability[9].type, value); return true;
    case 0x61627631 : assignField(ability[0].value, value); return true;
    case 0x61627632 : assignField(ability[1].value, value); return true;
    case 0x61627633 : assignField(ability[2].value, value); return true;
    case 0x61627634 : assignField(ability[3].value, value); return true;
    case 0x61627635 : assignField(ability[4].value, value); return true;
    case 0x61627636 : assignField(ability[5].value, value); return true;
    case 0x61627637 : assignField(ability[6].value, value); return true;
    case 0x61627638 : assignField(ability[7].value, value); return true;
    case 0x61627639 : assignField(ability[8].value, value); return true;
    case 0x6162763A : assignField(ability[9].value, value); return true;
    case 0x686C6870 : assignField(heal[0], value); return true;
    case 0x686C7370 : assignField(heal[1], value); return true;
    case 0x686C6D70 : assignField(heal[2], value); return true;
    case 0x74686870 : assignField(timeHeal[0], value); return true;
    case 0x74687370 : assignField(timeHeal[1], value); return true;
    case 0x74686D70 : assignField(timeHeal[2], value); return true;
    case 0x64667470 : assignField(defenceType, value); return true;
    case 0x6466766C : assignField(defenceValue, value); return true;
    case 0x6C6D6870 : assignField(limitHP, value); return true;
    case 0x66787267 : assignField(fixRange, value); return true;
    case 0x63687470 : assignField(changeType, value); return true;
    case 0x63686C76 : assignField(changeLevel, value); return true;
    default:
        return false;
    }
}

bool SkillData::loadFromRecord(RecordSet* rs)
{
    auto col = [rs](const wchar_t* name) -> int { return rs->getInt(name); };
    auto has = [rs](const wchar_t* name) -> bool { return rs->hasColumn(name); };

    level      = (unsigned char)col(L"SkillLevel");
    country    = (unsigned char)col(L"Country");
    classes[0] = (unsigned char)col(L"Attackfighter");
    classes[1] = (unsigned char)col(L"Defensefighter");
    classes[2] = (unsigned char)col(L"Patrolrogue");
    classes[3] = (unsigned char)col(L"Shootrogue");
    classes[4] = (unsigned char)col(L"Attackmage");
    classes[5] = (unsigned char)col(L"Defensemage");
    prevSkill  = (short)col(L"PrevSkillID");
    reqLevel   = (short)col(L"ReqLevel");
    grow       = (unsigned char)col(L"Grow");
    points     = (unsigned char)col(L"SkillPoint");
    typeShow   = (unsigned char)col(L"TypeShow");
    typeAttack = (unsigned char)col(L"TypeAttack");
    typeEffect = (unsigned char)col(L"TypeEffect");
    typeDetail = (short)col(L"TypeDetail");
    needWeapon[0]  = (unsigned char)col(L"NeedWeapon1");
    needWeapon[1]  = (unsigned char)col(L"NeedWeapon2");
    needWeapon[2]  = (unsigned char)col(L"NeedWeapon3");
    needWeapon[3]  = (unsigned char)col(L"NeedWeapon4");
    needWeapon[4]  = (unsigned char)col(L"NeedWeapon5");
    needWeapon[5]  = (unsigned char)col(L"NeedWeapon6");
    needWeapon[6]  = (unsigned char)col(L"NeedWeapon7");
    needWeapon[7]  = (unsigned char)col(L"NeedWeapon8");
    needWeapon[8]  = (unsigned char)col(L"NeedWeapon9");
    needWeapon[9]  = (unsigned char)col(L"NeedWeapon10");
    needWeapon[10] = (unsigned char)col(L"NeedWeapon11");
    needWeapon[11] = (unsigned char)col(L"NeedWeapon12");
    needWeapon[12] = (unsigned char)col(L"NeedWeapon13");
    needWeapon[13] = (unsigned char)col(L"NeedWeapon14");
    needWeapon[14] = (unsigned char)col(L"NeedWeapon15");
    shield       = (unsigned char)col(L"Shield");
    sp           = (short)col(L"SP");
    mp           = (short)col(L"MP");
    readyTime    = (unsigned char)col(L"ReadyTime");
    resetTime    = (short)col(L"ResetTime");
    attackRange  = (unsigned char)col(L"AttackRange");
    stateType    = (unsigned char)col(L"StateType");
    attribType   = (unsigned char)col(L"AttrType");
    disable      = (short)col(L"Disable");
    successType  = (unsigned char)col(L"SuccessType");
    successValue = (unsigned char)col(L"SuccessValue");
    targetType   = (unsigned char)col(L"TargetType");
    applyRange   = (unsigned char)col(L"ApplyRange");
    multiAttack  = (unsigned char)col(L"MultiAttack");
    keepTime     = (unsigned short)col(L"KeepTime");
    weapon[0]    = (unsigned char)col(L"Weapon1");
    weapon[1]    = (unsigned char)col(L"Weapon2");
    weaponValue  = (unsigned char)col(L"Weaponvalue");
    bag          = (unsigned char)col(L"Bag");
    if (has(L"Arrow"))
        arrow = (short)col(L"Arrow");
    else if (has(L"Aggro"))
        arrow = (short)col(L"Aggro");
    damageType     = (unsigned char)col(L"DamageType");
    damage[0]      = (short)col(L"DamageHP");
    damage[1]      = (short)col(L"DamageSP");
    damage[2]      = (short)col(L"DamageMP");
    timeDamageType = (unsigned char)col(L"TimeDamageType");
    timeDamage[0]  = (short)col(L"TimeDamageHP");
    timeDamage[1]  = (short)col(L"TimeDamageSP");
    timeDamage[2]  = (short)col(L"TimeDamageMP");
    addDamage[0]   = (short)col(L"AddDamageHP");
    addDamage[1]   = (short)col(L"AddDamageSP");
    addDamage[2]   = (short)col(L"AddDamageMP");
    ability[0].type  = (unsigned char)col(L"AbilityType1");
    ability[0].value = (short)col(L"AbilityValue1");
    ability[1].type  = (unsigned char)col(L"AbilityType2");
    ability[1].value = (short)col(L"AbilityValue2");
    ability[2].type  = (unsigned char)col(L"AbilityType3");
    ability[2].value = (short)col(L"AbilityValue3");
    if ((version & 0xFFF) >= 0x600)
    {
        ability[3].type  = (unsigned char)col(L"AbilityType4");
        ability[3].value = (short)col(L"AbilityValue4");
        ability[4].type  = (unsigned char)col(L"AbilityType5");
        ability[4].value = (short)col(L"AbilityValue5");
        ability[5].type  = (unsigned char)col(L"AbilityType6");
        ability[5].value = (short)col(L"AbilityValue6");
        ability[6].type  = (unsigned char)col(L"AbilityType7");
        ability[6].value = (short)col(L"AbilityValue7");
        ability[7].type  = (unsigned char)col(L"AbilityType8");
        ability[7].value = (short)col(L"AbilityValue8");
        ability[8].type  = (unsigned char)col(L"AbilityType9");
        ability[8].value = (short)col(L"AbilityValue9");
        ability[9].type  = (unsigned char)col(L"AbilityType10");
        ability[9].value = (short)col(L"AbilityValue10");
    }
    heal[0]     = (unsigned short)col(L"HealHP");
    heal[1]     = (unsigned short)col(L"HealSP");
    heal[2]     = (unsigned short)col(L"HealMP");
    timeHeal[0] = (unsigned short)col(L"TimeHealHP");
    timeHeal[1] = (unsigned short)col(L"TimeHealSP");
    timeHeal[2] = (unsigned short)col(L"TimeHealMP");
    if (has(L"DefenseType"))
        defenceType = (unsigned char)col(L"DefenseType");
    else
        defenceType = (unsigned char)col(L"DefenceType");
    if (has(L"DefenseValue"))
        defenceValue = (unsigned char)col(L"DefenseValue");
    else
        defenceValue = (unsigned char)col(L"DefenceValue");
    limitHP     = (unsigned char)col(L"LimitHP");
    fixRange    = (unsigned char)col(L"FixRange");
    changeType  = (short)col(L"ChangeType");
    changeLevel = (short)col(L"ChangeLevel");
    return true;
}

void SkillData::read(iStream& s, DWORD tag)
{
    FlntRowRead<SkillData> flntRow(this, s, tag);
    if (tag == 0x66696C65 )
    {
        level      = s.readByte();
        icon       = s.readShort();
        anim       = s.readByte();
        effect     = s.readShort();
        toggleType = s.readShort();
        if ((version & 0xFFF) < 0x600)
            sound = 0;
        else
            sound = s.readByte();
        reqLevel   = s.readShort();
        country    = s.readByte();
        classes[0] = s.readByte();
        classes[1] = s.readByte();
        classes[2] = s.readByte();
        classes[3] = s.readByte();
        classes[4] = s.readByte();
        classes[5] = s.readByte();
        grow       = s.readByte();
        points     = s.readByte();
        typeShow   = s.readByte();
        typeAttack = s.readByte();
        typeEffect = s.readByte();
        typeDetail = s.readShort();
        for (int i = 0; i < 15; ++i)
            needWeapon[i] = s.readByte();
        shield       = s.readByte();
        sp           = s.readShort();
        mp           = s.readShort();
        readyTime    = s.readByte();
        resetTime    = s.readShort();
        attackRange  = s.readByte();
        stateType    = s.readByte();
        attribType   = s.readByte();
        disable      = s.readShort();
        prevSkill    = s.readShort();
        successType  = s.readByte();
        successValue = s.readByte();
        targetType   = s.readByte();
        applyRange   = s.readByte();
        multiAttack  = s.readByte();
        keepTime     = s.readShort();
        weapon[0]    = s.readByte();
        weapon[1]    = s.readByte();
        weaponValue  = s.readByte();
        bag          = s.readByte();
        arrow        = s.readShort();
        damageType   = s.readByte();
        damage[0]    = s.readShort();
        damage[1]    = s.readShort();
        damage[2]    = s.readShort();
        timeDamageType = s.readByte();
        timeDamage[0]  = s.readShort();
        timeDamage[1]  = s.readShort();
        timeDamage[2]  = s.readShort();
        addDamage[0]   = s.readShort();
        addDamage[1]   = s.readShort();
        addDamage[2]   = s.readShort();
        for (int i = 0; i < 3; ++i)
        {
            ability[i].type  = s.readByte();
            ability[i].value = s.readShort();
        }
        if ((version & 0xFFF) >= 0x600)
        {
            for (int i = 3; i < 10; ++i)
            {
                ability[i].type  = s.readByte();
                ability[i].value = s.readShort();
            }
        }
        heal[0]      = s.readShort();
        heal[1]      = s.readShort();
        heal[2]      = s.readShort();
        timeHeal[0]  = s.readShort();
        timeHeal[1]  = s.readShort();
        timeHeal[2]  = s.readShort();
        defenceType  = s.readByte();
        defenceValue = s.readByte();
        limitHP      = s.readByte();
        fixRange     = s.readByte();
        changeType   = s.readShort();
        changeLevel  = s.readShort();
    }
    else if (tag == 0x666C6E74 )
    {
        skillId    = (short)s.readInt64();
        level      = (unsigned char)s.readInt64();
        icon       = (short)s.readInt64();
        anim       = (unsigned char)s.readInt64();
        effect     = (short)s.readInt64();
        toggleType = (short)s.readInt64();
        sound      = (unsigned char)s.readInt64();
        reqLevel   = (short)s.readInt64();
        country    = (unsigned char)s.readInt64();
        for (int j = 0; j < 6; ++j)
            classes[j] = (unsigned char)s.readInt64();
        grow       = (unsigned char)s.readInt64();
        points     = (unsigned char)s.readInt64();
        typeShow   = (unsigned char)s.readInt64();
        typeAttack = (unsigned char)s.readInt64();
        typeEffect = (unsigned char)s.readInt64();
        typeDetail = (short)s.readInt64();
        for (int k = 0; k < 15; ++k)
            needWeapon[k] = (unsigned char)s.readInt64();
        shield       = (unsigned char)s.readInt64();
        sp           = (short)s.readInt64();
        mp           = (short)s.readInt64();
        readyTime    = (unsigned char)s.readInt64();
        resetTime    = (short)s.readInt64();
        attackRange  = (unsigned char)s.readInt64();
        stateType    = (unsigned char)s.readInt64();
        attribType   = (unsigned char)s.readInt64();
        disable      = (short)s.readInt64();
        prevSkill    = (short)s.readInt64();
        successType  = (unsigned char)s.readInt64();
        successValue = (unsigned char)s.readInt64();
        targetType   = (unsigned char)s.readInt64();
        applyRange   = (unsigned char)s.readInt64();
        multiAttack  = (unsigned char)s.readInt64();
        keepTime     = (unsigned char)s.readInt64();
        weapon[0]    = (unsigned char)s.readInt64();
        weapon[1]    = (unsigned char)s.readInt64();
        weaponValue  = (unsigned char)s.readInt64();
        bag          = (unsigned char)s.readInt64();
        arrow        = (short)s.readInt64();
        damageType   = (unsigned char)s.readInt64();
        for (int i = 0; i < 3; ++i)
            damage[i] = (short)s.readInt64();
        timeDamageType = (unsigned char)s.readInt64();
        for (int i = 0; i < 3; ++i)
            timeDamage[i] = (short)s.readInt64();
        for (int i = 0; i < 3; ++i)
            addDamage[i] = (short)s.readInt64();
        for (int i = 0; i < 10; ++i)
        {
            ability[i].type  = (unsigned char)s.readInt64();
            ability[i].value = (short)s.readInt64();
        }
        for (int i = 0; i < 3; ++i)
            heal[i] = (unsigned short)s.readInt64();
        for (int i = 0; i < 3; ++i)
            timeHeal[i] = (unsigned short)s.readInt64();
        defenceType  = (unsigned char)s.readInt64();
        defenceValue = (unsigned char)s.readInt64();
        limitHP      = (unsigned char)s.readInt64();
        fixRange     = (unsigned char)s.readInt64();
        changeType   = (short)s.readInt64();
        changeLevel  = (short)s.readInt64();
    }
}

void SkillData::write(oStream& s, DWORD tag) const
{
    FlntRowWrite flntRow(this, s, tag);
    if (tag == 0x66696C65 )
    {
        s << (char)(level) << (unsigned short)(icon) << (char)(anim) << (unsigned short)(effect) << (unsigned short)(toggleType);
        if ((version & 0xFFF) >= 0x600)
            s << (char)(sound);
        s << (unsigned short)(reqLevel)
          << (char)(country)
          << (char)(classes[0]) << (char)(classes[1]) << (char)(classes[2])
          << (char)(classes[3]) << (char)(classes[4]) << (char)(classes[5])
          << (char)(grow) << (char)(points) << (char)(typeShow) << (char)(typeAttack) << (char)(typeEffect)
          << (unsigned short)(typeDetail);
        for (int i = 0; i < 15; ++i)
            s << (char)(needWeapon[i]);
        s << (char)(shield);
        s << (unsigned short)(sp) << (unsigned short)(mp) << (char)(readyTime);
        s << (unsigned short)(resetTime) << (char)(attackRange) << (char)(stateType) << (char)(attribType);
        s << (unsigned short)(disable) << (unsigned short)(prevSkill)
          << (char)(successType) << (char)(successValue) << (char)(targetType) << (char)(applyRange)
          << (char)(multiAttack);
        s << (short)(keepTime) << (char)(weapon[0]) << (char)(weapon[1]) << (char)(weaponValue) << (char)(bag);
        s << (unsigned short)(arrow) << (char)(damageType);
        s << (unsigned short)(damage[0]) << (unsigned short)(damage[1]) << (unsigned short)(damage[2]) << (char)(timeDamageType);
        s << (unsigned short)(timeDamage[0]) << (unsigned short)(timeDamage[1]) << (unsigned short)(timeDamage[2])
          << (unsigned short)(addDamage[0]) << (unsigned short)(addDamage[1]) << (unsigned short)(addDamage[2]);
        for (int i = 0; i < 3; ++i)
        {
            s << (char)(ability[i].type);
            s << (unsigned short)(ability[i].value);
        }
        if ((version & 0xFFF) >= 0x600)
        {
            for (int i = 3; i < 10; ++i)
            {
                s << (char)(ability[i].type);
                s << (unsigned short)(ability[i].value);
            }
        }
        s << (unsigned short)((short)heal[0]) << (unsigned short)((short)heal[1]) << (unsigned short)((short)heal[2])
          << (unsigned short)((short)timeHeal[0]) << (unsigned short)((short)timeHeal[1]) << (unsigned short)((short)timeHeal[2])
          << (char)(defenceType) << (char)(defenceValue) << (char)(limitHP) << (char)(fixRange)
          << (unsigned short)(changeType) << (unsigned short)(changeLevel);
    }
    else if (tag == 0x666C6E74 )
    {
        s << ((__int64)skillId);
        s << ((__int64)level);
        s << ((__int64)icon);
        s << ((__int64)anim);
        s << ((__int64)effect);
        s << ((__int64)toggleType);
        s << ((__int64)sound);
        s << ((__int64)reqLevel);
        s << ((__int64)country);
        for (int i = 0; i < 6; ++i)
            s << ((__int64)classes[i]);
        s << ((__int64)grow);
        s << ((__int64)points);
        s << ((__int64)typeShow);
        s << ((__int64)typeAttack);
        s << ((__int64)typeEffect);
        s << ((__int64)typeDetail);
        for (int j = 0; j < 15; ++j)
            s << ((__int64)needWeapon[j]);
        s << ((__int64)shield);
        s << ((__int64)sp);
        s << ((__int64)mp);
        s << ((__int64)readyTime);
        s << ((__int64)resetTime);
        s << ((__int64)attackRange);
        s << ((__int64)stateType);
        s << ((__int64)attribType);
        s << ((__int64)disable);
        s << ((__int64)prevSkill);
        s << ((__int64)successType);
        s << ((__int64)successValue);
        s << ((__int64)targetType);
        s << ((__int64)applyRange);
        s << ((__int64)multiAttack);
        s << ((__int64)keepTime);
        s << ((__int64)weapon[0]);
        s << ((__int64)weapon[1]);
        s << ((__int64)weaponValue);
        s << ((__int64)bag);
        s << ((__int64)arrow);
        s << ((__int64)damageType);
        for (int i = 0; i < 3; ++i)
            s << ((__int64)damage[i]);
        s << ((__int64)timeDamageType);
        for (int i = 0; i < 3; ++i)
            s << ((__int64)timeDamage[i]);
        for (int i = 0; i < 3; ++i)
            s << ((__int64)addDamage[i]);
        for (int i = 0; i < 10; ++i)
        {
            s << ((__int64)ability[i].type);
            s << ((__int64)ability[i].value);
        }
        for (int i = 0; i < 3; ++i)
            s << ((__int64)heal[i]);
        for (int i = 0; i < 3; ++i)
            s << ((__int64)timeHeal[i]);
        s << ((__int64)defenceType);
        s << ((__int64)defenceValue);
        s << ((__int64)limitHP);
        s << ((__int64)fixRange);
        s << ((__int64)changeType);
        s << ((__int64)changeLevel);
    }
}

int SkillData::compare(const SkillData& other, DWORD tag) const
{
    switch (tag)
    {
    case 0x736B6964 : return cmpField(skillId, other.skillId);
    case 0x6C76656C : return cmpField(level, other.level);
    case 0x69636F6E : return cmpField(icon, other.icon);
    case 0x616E696D : return cmpField(anim, other.anim);
    case 0x726C766C : return cmpField(reqLevel, other.reqLevel);
    case 0x636E7472 : return cmpField(country, other.country);
    case 0x66696768 : return cmpField(classes[0], other.classes[0]);
    case 0x64656673 : return cmpField(classes[1], other.classes[1]);
    case 0x726E6772 : return cmpField(classes[2], other.classes[2]);
    case 0x61726368 : return cmpField(classes[3], other.classes[3]);
    case 0x6D616765 : return cmpField(classes[4], other.classes[4]);
    case 0x70727374 : return cmpField(classes[5], other.classes[5]);
    case 0x67726F77 : return cmpField(grow, other.grow);
    case 0x706E7473 : return cmpField(points, other.points);
    case 0x74707368 : return cmpField(typeShow, other.typeShow);
    case 0x74706174 : return cmpField(typeAttack, other.typeAttack);
    case 0x74706566 : return cmpField(typeEffect, other.typeEffect);
    case 0x74706474 : return cmpField(typeDetail, other.typeDetail);
    case 0x73686C64 : return cmpField(shield, other.shield);
    case 0x63737370 : return cmpField(sp, other.sp);
    case 0x63736D70 : return cmpField(mp, other.mp);
    case 0x7264746D : return cmpField(readyTime, other.readyTime);
    case 0x7273746D : return cmpField(resetTime, other.resetTime);
    case 0x61747267 : return cmpField(attackRange, other.attackRange);
    case 0x73747470 : return cmpField(stateType, other.stateType);
    case 0x61747470 : return cmpField(attribType, other.attribType);
    case 0x64697361 : return cmpField(disable, other.disable);
    case 0x70727673 : return cmpField(prevSkill, other.prevSkill);
    case 0x73637470 : return cmpField(successType, other.successType);
    case 0x7363766C : return cmpField(successValue, other.successValue);
    case 0x74727470 : return cmpField(targetType, other.targetType);
    case 0x61707267 : return cmpField(applyRange, other.applyRange);
    case 0x6D756C74 : return cmpField(multiAttack, other.multiAttack);
    case 0x6B70746D : return cmpField(keepTime, other.keepTime);
    case 0x77706E31 : return cmpField(weapon[0], other.weapon[0]);
    case 0x77706E32 : return cmpField(weapon[1], other.weapon[1]);
    case 0x7776616C : return cmpField(weaponValue, other.weaponValue);
    case 0x62616773 : return cmpField(bag, other.bag);
    case 0x61727277 : return cmpField(arrow, other.arrow);
    case 0x646D7470 : return cmpField(damageType, other.damageType);
    case 0x646D6870 : return cmpField(damage[0], other.damage[0]);
    case 0x646D7370 : return cmpField(damage[1], other.damage[1]);
    case 0x646D6D70 : return cmpField(damage[2], other.damage[2]);
    case 0x74647470 : return cmpField(timeDamageType, other.timeDamageType);
    case 0x74646870 : return cmpField(timeDamage[0], other.timeDamage[0]);
    case 0x74647370 : return cmpField(timeDamage[1], other.timeDamage[1]);
    case 0x74646D70 : return cmpField(timeDamage[2], other.timeDamage[2]);
    case 0x61646870 : return cmpField(addDamage[0], other.addDamage[0]);
    case 0x61647370 : return cmpField(addDamage[1], other.addDamage[1]);
    case 0x61646D70 : return cmpField(addDamage[2], other.addDamage[2]);
    case 0x61627431 : return cmpField(ability[0].type, other.ability[0].type);
    case 0x61627432 : return cmpField(ability[1].type, other.ability[1].type);
    case 0x61627433 : return cmpField(ability[2].type, other.ability[2].type);
    case 0x61627434 : return cmpField(ability[3].type, other.ability[3].type);
    case 0x61627435 : return cmpField(ability[4].type, other.ability[4].type);
    case 0x61627436 : return cmpField(ability[5].type, other.ability[5].type);
    case 0x61627437 : return cmpField(ability[6].type, other.ability[6].type);
    case 0x61627438 : return cmpField(ability[7].type, other.ability[7].type);
    case 0x61627439 : return cmpField(ability[8].type, other.ability[8].type);
    case 0x6162743A : return cmpField(ability[9].type, other.ability[9].type);
    case 0x61627631 : return cmpField(ability[0].value, other.ability[0].value);
    case 0x61627632 : return cmpField(ability[1].value, other.ability[1].value);
    case 0x61627633 : return cmpField(ability[2].value, other.ability[2].value);
    case 0x61627634 : return cmpField(ability[3].value, other.ability[3].value);
    case 0x61627635 : return cmpField(ability[4].value, other.ability[4].value);
    case 0x61627636 : return cmpField(ability[5].value, other.ability[5].value);
    case 0x61627637 : return cmpField(ability[6].value, other.ability[6].value);
    case 0x61627638 : return cmpField(ability[7].value, other.ability[7].value);
    case 0x61627639 : return cmpField(ability[8].value, other.ability[8].value);
    case 0x6162763A : return cmpField(ability[9].value, other.ability[9].value);
    case 0x686C6870 : return cmpField(heal[0], other.heal[0]);
    case 0x686C7370 : return cmpField(heal[1], other.heal[1]);
    case 0x686C6D70 : return cmpField(heal[2], other.heal[2]);
    case 0x74686870 : return cmpField(timeHeal[0], other.timeHeal[0]);
    case 0x74687370 : return cmpField(timeHeal[1], other.timeHeal[1]);
    case 0x74686D70 : return cmpField(timeHeal[2], other.timeHeal[2]);
    case 0x64667470 : return cmpField(defenceType, other.defenceType);
    case 0x6466766C : return cmpField(defenceValue, other.defenceValue);
    case 0x6C6D6870 : return cmpField(limitHP, other.limitHP);
    case 0x66787267 : return cmpField(fixRange, other.fixRange);
    case 0x63687470 : return cmpField(changeType, other.changeType);
    case 0x63686C76 : return cmpField(changeLevel, other.changeLevel);
    default:
        return 0;
    }
}

bool SkillData::exportText(String& s, DWORD tag) const
{
    wchar_t sep = skillExportSeparator(tag);
    if (tag == 0x63737620 )
    {
        s << (short)(icon) << sep;
        s << (unsigned char)(anim) << sep;
    }
    s << (unsigned char)(country);
    for (int i = 0; i < 6; ++i)
    {
        s << sep;
        s << (unsigned char)(classes[i]);
    }
    s << sep;
    s << (short)(prevSkill) << sep;
    s << (short)(reqLevel) << sep;
    s << (unsigned char)(grow) << sep;
    s << (unsigned char)(points) << sep;
    s << (unsigned char)(typeShow) << sep;
    s << (unsigned char)(typeAttack) << sep;
    s << (unsigned char)(typeEffect) << sep;
    s << (short)(typeDetail);
    for (int j = 0; j < 15; ++j)
    {
        s << sep;
        s << (unsigned char)(needWeapon[j]);
    }
    s << sep;
    s << (unsigned char)(shield) << sep;
    s << (short)(sp) << sep;
    s << (short)(mp) << sep;
    s << (unsigned char)(readyTime) << sep;
    s << (short)(resetTime) << sep;
    s << (unsigned char)(attackRange) << sep;
    s << (unsigned char)(stateType) << sep;
    s << (unsigned char)(attribType) << sep;
    s << (short)(disable) << sep;
    s << (unsigned char)(successType) << sep;
    s << (unsigned char)(successValue) << sep;
    s << (unsigned char)(targetType) << sep;
    s << (unsigned char)(applyRange) << sep;
    s << (unsigned char)(multiAttack) << sep;
    s << (short)((short)keepTime) << sep;
    s << (unsigned char)(weapon[0]) << sep;
    s << (unsigned char)(weapon[1]) << sep;
    s << (unsigned char)(weaponValue) << sep;
    s << (unsigned char)(bag) << sep;
    s << (short)(arrow) << sep;
    s << (unsigned char)(damageType) << sep;
    s << (short)(damage[0]) << sep;
    s << (short)(damage[1]) << sep;
    s << (short)(damage[2]) << sep;
    s << (unsigned char)(timeDamageType) << sep;
    s << (short)(timeDamage[0]) << sep;
    s << (short)(timeDamage[1]) << sep;
    s << (short)(timeDamage[2]) << sep;
    s << (short)(addDamage[0]) << sep;
    s << (short)(addDamage[1]) << sep;
    s << (short)(addDamage[2]);
    for (int i = 0; i < 3; ++i)
    {
        s << sep;
        s << (unsigned char)(ability[i].type) << sep;
        s << (short)(ability[i].value);
    }
    if ((version & 0xFFF) >= 0x600)
    {
        for (int i = 3; i < 10; ++i)
        {
            s << sep;
            s << (unsigned char)(ability[i].type) << sep;
            s << (short)(ability[i].value);
        }
    }
    s << sep;
    s << (unsigned short)(heal[0]) << sep;
    s << (unsigned short)(heal[1]) << sep;
    s << (unsigned short)(heal[2]) << sep;
    s << (unsigned short)(timeHeal[0]) << sep;
    s << (unsigned short)(timeHeal[1]) << sep;
    s << (unsigned short)(timeHeal[2]) << sep;
    s << (unsigned char)(defenceType) << sep;
    s << (unsigned char)(defenceValue) << sep;
    s << (unsigned char)(limitHP) << sep;
    s << (unsigned char)(fixRange) << sep;
    s << (short)(changeType) << sep;
    s << (short)(changeLevel);
    if (tag == 0x73716C20 )
    {
        s << sep;
        s << L"GetDate()";
    }
    return true;
}
