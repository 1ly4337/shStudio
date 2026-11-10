#pragma once
#include <windows.h>

class String;
class iStream;
class oStream;
class RecordSet;

#pragma pack(push, 4)
struct SkillData
{
    struct Ability
    {
        unsigned char type;
        unsigned char _pad;
        short         value;
    };

    int            version;
    int            skillId;
    unsigned char  level;
    unsigned char  _pad09;
    short          icon;
    unsigned char  anim;
    unsigned char  _pad0D;
    short          effect;
    short          toggleType;
    unsigned char  sound;
    unsigned char  _pad13;
    short          reqLevel;
    unsigned char  country;
    unsigned char  classes[6];
    unsigned char  grow;
    unsigned char  points;
    unsigned char  typeShow;
    unsigned char  typeAttack;
    unsigned char  typeEffect;
    short          typeDetail;
    unsigned char  needWeapon[15];
    unsigned char  shield;
    short          sp;
    short          mp;
    unsigned char  readyTime;
    unsigned char  _pad39;
    short          resetTime;
    unsigned char  attackRange;
    unsigned char  stateType;
    unsigned char  attribType;
    unsigned char  _pad3F;
    short          disable;
    short          prevSkill;
    unsigned char  successType;
    unsigned char  successValue;
    unsigned char  targetType;
    unsigned char  applyRange;
    unsigned char  multiAttack;
    unsigned char  _pad49;
    unsigned short keepTime;
    unsigned char  weapon[2];
    unsigned char  weaponValue;
    unsigned char  bag;
    short          arrow;
    unsigned char  damageType;
    unsigned char  _pad53;
    short          damage[3];
    unsigned char  timeDamageType;
    unsigned char  _pad5B;
    short          timeDamage[3];
    short          addDamage[3];
    Ability        ability[10];
    unsigned short heal[3];
    unsigned short timeHeal[3];
    unsigned char  defenceType;
    unsigned char  defenceValue;
    unsigned char  limitHP;
    unsigned char  fixRange;
    short          changeType;
    short          changeLevel;

    SkillData();
    int  getValue(DWORD tag) const;
    bool setValue(DWORD tag, int value);
    bool loadFromRecord(RecordSet* rs);
    void read(iStream& s, DWORD tag);
    void write(oStream& s, DWORD tag) const;
    int  compare(const SkillData& other, DWORD tag) const;
    bool exportText(String& s, DWORD tag) const;
};
#pragma pack(pop)

static_assert(sizeof(SkillData) == 0xA4, "SkillData size");
