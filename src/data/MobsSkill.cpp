#include "FlntRows.h"
#include "MobsSkill.h"
#include "SkillText.h"
#include "Streamable.h"
#include "iStream.h"
#include "oStream.h"
#include "_imports_w1.h"
#include <shlwapi.h>
#include <cstring>

MobsSkill::MobsSkill(int version, short skillId, int )
{
    memset(&m_skill, 0, 0xA4);
    m_skill.version = version;
    m_skill.skillId = skillId;
    m_skill.level = 1;
    MobsSkill::reset();
}

MobsSkill::~MobsSkill()
{
    FlntRows::forget(&m_skill);
}

int MobsSkill::getId() const
{
    return m_skill.icon;
}

bool MobsSkill::copyFrom(const Streamable* src)
{
    const MobsSkill* other = dynamic_cast<const MobsSkill*>(src);
    if (!other)
        return false;
    *this = *other;
    return true;
}

MobsSkill& MobsSkill::operator=(const MobsSkill& other)
{
    m_skill = other.m_skill;
    m_name = other.m_name;
    m_desc = other.m_desc;
    return *this;
}

const wchar_t* MobsSkill::getString(DWORD tag) const
{
    if (tag == 0x6E616D65 )
        return m_name.m_buffer;
    if (tag == 0x64657363 )
        return m_desc.m_buffer;
    if (tag != 0x74706474 )
        return 0;
    memset(g_skillTypeTextBuf, 0, 0x100);
    const wchar_t* typeName = getSkillTypeName(m_skill.typeDetail);
    swprintf128(g_skillTypeTextBuf, L"%i - %ls", (int)m_skill.typeDetail, typeName);
    return g_skillTypeTextBuf;
}

bool MobsSkill::setString(DWORD tag, const wchar_t* value)
{
    if (tag == 0x6E616D65 )
    {
        m_name = value;
        return true;
    }
    if (tag == 0x64657363 )
    {
        m_desc = value;
        return true;
    }
    return false;
}

int MobsSkill::getInt(DWORD tag) const
{
    return m_skill.getValue(tag);
}

bool MobsSkill::setInt(DWORD tag, int value)
{
    return m_skill.setValue(tag, value);
}

void MobsSkill::reset()
{
    int keepId = m_skill.skillId;
    int keepVersion = m_skill.version;
    memset(&m_skill, 0, 0xA4);
    m_skill.skillId = keepId;
    m_skill.version = keepVersion;
    m_name.clear();
    m_desc.clear();
}

void MobsSkill::read(iStream& in, DWORD fmt)
{
    if (fmt == 0x6C747874 )
        return;
    if (fmt == 0x66696C65 )
    {
        m_name.read(in, 4, in.m_codePage);
        m_desc.read(in, 4, in.m_codePage);
    }
    m_skill.read(in, fmt);
}

void MobsSkill::write(oStream& out, DWORD fmt) const
{
    if (fmt == 0x6C747874 )
        return;
    if (fmt == 0x66696C65 )
    {
        m_name.write(out, 4, out.m_codePage);
        m_desc.write(out, 4, out.m_codePage);
    }
    m_skill.write(out, fmt);
}

int MobsSkill::compare(const Node* other, DWORD tag) const
{
    const MobsSkill& o = dynamic_cast<const MobsSkill&>(*other);
    if (tag == 0x6E616D65 )
    {
        const wchar_t* a = m_name.m_buffer;
        const wchar_t* b = o.m_name.m_buffer;
        if (!a)
            return -(b != 0);
        if (!b)
            return 1;
        return StrCmpLogicalW(a, b);
    }
    if (tag == 0x64657363 )
    {
        const wchar_t* a = m_desc.m_buffer;
        const wchar_t* b = o.m_desc.m_buffer;
        if (!a)
            return -(b != 0);
        if (!b)
            return 1;
        return StrCmpLogicalW(a, b);
    }
    return m_skill.compare(o.m_skill, tag);
}

bool MobsSkill::toText(String& s, DWORD fmt) const
{
    wchar_t sep = skillExportSeparator(fmt);
    if (fmt == 0x63737620 )
    {
        String escaped = m_desc.escape();
        for (unsigned int i = 0; i < escaped.m_length; ++i)
        {
            if (escaped.m_buffer[i] == L';')
                escaped.m_buffer[i] = L',';
        }
        s << (long)m_skill.skillId << sep
          << (long)100 << sep;
        s.append(m_name.m_buffer, m_name.m_length) << sep;
        s.append(escaped.m_buffer, escaped.m_length) << sep;
        m_skill.exportText(s, 0x63737620 );
        s << L"\r\n";
        return true;
    }
    if (fmt == 0x73716C20 )
    {
        s << L"INSERT INTO [PS_GameDefs].[dbo].[Skills] ("
          << L"SkillID,SkillLevel,SkillName,Country,Attackfighter,Defensefighter,Patrolrogue,Shootrogue,Attackmage,Defensemage,PrevSkillID,ReqLevel,Grow,SkillPoint,TypeShow,TypeAttack,TypeEffect,TypeDetail,NeedWeapon1,NeedWeapon2,NeedWeapon3,NeedWeapon4,NeedWeapon5,NeedWeapon6,NeedWeapon7,NeedWeapon8,NeedWeapon9,NeedWeapon10,NeedWeapon11,NeedWeapon12,NeedWeapon13,NeedWeapon14,NeedWeapon15,Shield,SP,MP,ReadyTime,ResetTime,AttackRange,StateType,AttrType,[Disable],SuccessType,SuccessValue,TargetType,ApplyRange,MultiAttack,KeepTime,Weapon1,Weapon2,Weaponvalue,Bag,Arrow,DamageType,DamageHP,DamageSP,DamageMP,TimeDamageType,TimeDamageHP,TimeDamageSP,TimeDamageMP,AddDamageHP,AddDamageSP,AddDamageMP,AbilityType1,AbilityValue1,AbilityType2,AbilityValue2,AbilityType3,AbilityValue3,";
        if (m_skill.version >= 0x600)
            s << L"AbilityType4,AbilityValue4,AbilityType5,AbilityValue5,AbilityType6,AbilityValue6,AbilityType7,AbilityValue7,AbilityType8,AbilityValue8,AbilityType9,AbilityValue9,AbilityType10,AbilityValue10,";
        s << L"HealHP,HealSP,HealMP,TimeHealHP,TimeHealSP,TimeHealMP,DefenceType,DefenceValue,LimitHP,FixRange,ChangeType,ChangeLevel,UpdateDate"
          << L") VALUES "
          << L"\r\n";
        {
            String quoted = String::sqlQuote(m_name.m_buffer);
            s << L'(';
            s << (long)m_skill.skillId << sep
              << (long)100 << sep;
            s.append(quoted.m_buffer, quoted.m_length) << sep;
        }
        m_skill.exportText(s, 0x73716C20 );
        s << L')';
        s << L';';
        s << L"\r\n";
        return true;
    }
    s << L'"';
    s.append(m_name.m_buffer, m_name.m_length) << L'"' << sep << L'"';
    s.append(m_desc.m_buffer, m_desc.m_length) << L'"' << sep;
    m_skill.exportText(s, fmt);
    s << L',';
    s << L"\r\n";
    return true;
}
