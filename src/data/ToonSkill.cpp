#include "FlntRows.h"
#include "ToonSkill.h"
#include "iStream.h"
#include "oStream.h"
#include "SkillText.h"
#include <typeinfo>

wchar_t g_toonSkillTypeText[256];

ToonSkill::ToonSkill(int skillId, int format)
    : Node()
    , m_name()
{
    int type = format & 0xFFF;
    m_format = type;
    m_skillId = skillId;
    m_levelCount = 9;
    if (type == 0x400)
        m_levelCount = 3;
    else if ((format & 0xFFF) >= 0x600)
        m_levelCount = 15;
    for (int i = 0; i < MaxLevels; ++i)
        FIELD(int, &m_levels[i], 0x0) = format;
}

ToonSkill::~ToonSkill()
{
    for (int i = 0; i < MaxLevels; ++i)
        FlntRows::forget(&m_levels[i]);
}

int ToonSkill::getId() const
{
    return FIELD(short, &m_levels[0], 0xA);
}

const wchar_t* ToonSkill::getString(DWORD tag) const
{
    if (tag == 'name')
        return m_name.m_buffer;
    if (tag == 'desc')
        return m_levelTexts[0].m_buffer;
    if (tag != 'tpdt')
        return 0;
    short typeDetail = FIELD(short, &m_levels[0], 0x22);
    memset(g_toonSkillTypeText, 0, sizeof(g_toonSkillTypeText));
    swprintf_s(g_toonSkillTypeText, L"%i - %ls", (int)typeDetail, getSkillTypeName(typeDetail));
    return g_toonSkillTypeText;
}

int ToonSkill::getInt(DWORD tag) const
{
    if (tag == 'skid')
        return m_skillId;
    return m_levels[0].SkillData::getValue(tag);
}

bool ToonSkill::setInt(DWORD tag, int value)
{
    if (tag != 'skid')
        return m_levels[0].SkillData::setValue(tag, value);
    m_skillId = value;
    return true;
}

bool ToonSkill::setString(DWORD tag, const wchar_t* value)
{
    if (tag != 'name')
        return false;
    m_name = value;
    return true;
}

void ToonSkill::reset()
{
    m_name.clear();
    for (int i = 0; i < MaxLevels; ++i)
    {
        int format = FIELD(int, &m_levels[i], 0x0);
        int id = FIELD(int, &m_levels[i], 0x4);
        memset(&m_levels[i], 0, 0xA4);
        FIELD(int, &m_levels[i], 0x0) = format;
        FIELD(int, &m_levels[i], 0x4) = id;
        m_levelNames[i].clear();
        m_levelTexts[i].clear();
    }
}

int ToonSkill::compare(const Node* other, DWORD tag) const
{
    const ToonSkill& o = dynamic_cast<const ToonSkill&>(*other);
    if (tag == 'name')
        return String::compareLogical(m_name, o.m_name);
    if (tag == 'desc')
        return String::compareLogical(m_levelTexts[0], o.m_levelTexts[0]);
    if (tag != 'skid')
        return m_levels[0].SkillData::compare(o.m_levels[0], tag);
    if (m_skillId != o.m_skillId)
        return 2 * (m_skillId >= o.m_skillId) - 1;
    return 0;
}

void ToonSkill::setFormat(int format)
{
    switch (format & 0xFFF)
    {
    case 0x400:
        m_levelCount = 3;
        break;
    case 0x500:
        m_levelCount = 9;
        break;
    case 0x600:
        m_levelCount = 15;
        break;
    default:
        return;
    }
    m_format = format;
    for (int i = 0; i < MaxLevels; ++i)
        FIELD(int, &m_levels[i], 0x0) = format;
}

bool ToonSkill::assign(const ToonSkill* other)
{
    m_levelCount = other->m_levelCount;
    m_name = other->m_name;
    for (int i = 0; i < MaxLevels; ++i)
    {
        m_levelNames[i] = other->m_levelNames[i];
        m_levelTexts[i] = other->m_levelTexts[i];
        m_levels[i] = other->m_levels[i];
    }
    return true;
}

bool ToonSkill::copyFrom(const Streamable* src)
{
    const ToonSkill* skill = dynamic_cast<const ToonSkill*>(src);
    if (skill)
        return assign(skill);
    return false;
}

void ToonSkill::read(iStream& in, DWORD fmt)
{
    if (fmt != 'file')
        throw (wchar_t*)L"Invalid call to ToonSkill::load";
    for (int i = 0; i < m_levelCount; ++i)
    {
        m_levelNames[i].read(in, 4, in.m_codePage);
        m_levelTexts[i].read(in, 4, in.m_codePage);
        m_levels[i].SkillData::read(in, 'file');
    }
    baseName(m_levelNames[0], m_name);
}

void ToonSkill::write(oStream& out, DWORD fmt) const
{
    switch (fmt)
    {
    case 'ltxt':
        for (int i = 0; i < m_levelCount; ++i)
        {
            m_levelNames[i].write(out, 4, out.m_codePage);
            m_levelTexts[i].write(out, 4, out.m_codePage);
        }
        break;
    case 'flnt':
        for (int i = 0; i < m_levelCount; ++i)
            m_levels[i].SkillData::write(out, 'flnt');
        break;
    case 'file':
        for (int i = 0; i < m_levelCount; ++i)
        {
            m_levelNames[i].write(out, 4, out.m_codePage);
            m_levelTexts[i].write(out, 4, out.m_codePage);
            m_levels[i].SkillData::write(out, 'file');
        }
        break;
    }
}

bool ToonSkill::toText(String& out, DWORD fmt) const
{
    wchar_t sep = p04_separator(fmt);

    if (fmt == 'csv ')
    {
        for (int i = 0; i < m_levelCount; ++i)
        {
            const String& name = m_levelNames[i].m_length ? m_levelNames[i] : m_name;
            String escaped = m_levelTexts[i].escape();
            for (unsigned int k = 0; k < escaped.m_length; ++k)
                if (escaped.m_buffer[k] == L';')
                    escaped.m_buffer[k] = L',';
            out << (long)m_skillId << sep << (unsigned char)FIELD(BYTE, &m_levels[i], 0x8) << sep;
            out.append(name.m_buffer, name.m_length) << sep;
            out.append(escaped.m_buffer, escaped.m_length) << sep;
            m_levels[i].SkillData::exportText(out, 'csv ');
            out.newline();
        }
        return true;
    }

    if (fmt == 'sql ')
    {
        out << L"INSERT INTO [PS_GameDefs].[dbo].[Skills] ("
            << L"SkillID,SkillLevel,SkillName,Country,Attackfighter,Defensefighter,Patrolrogue,Shootrogue,Attackmage,Defensemage,PrevSkillID,ReqLevel,Grow,SkillPoint,TypeShow,TypeAttack,TypeEffect,TypeDetail,NeedWeapon1,NeedWeapon2,NeedWeapon3,NeedWeapon4,NeedWeapon5,NeedWeapon6,NeedWeapon7,NeedWeapon8,NeedWeapon9,NeedWeapon10,NeedWeapon11,NeedWeapon12,NeedWeapon13,NeedWeapon14,NeedWeapon15,Shield,SP,MP,ReadyTime,ResetTime,AttackRange,StateType,AttrType,[Disable],SuccessType,SuccessValue,TargetType,ApplyRange,MultiAttack,KeepTime,Weapon1,Weapon2,Weaponvalue,Bag,Arrow,DamageType,DamageHP,DamageSP,DamageMP,TimeDamageType,TimeDamageHP,TimeDamageSP,TimeDamageMP,AddDamageHP,AddDamageSP,AddDamageMP,AbilityType1,AbilityValue1,AbilityType2,AbilityValue2,AbilityType3,AbilityValue3,";
        if (m_format >= 0x600)
            out << L"AbilityType4,AbilityValue4,AbilityType5,AbilityValue5,AbilityType6,AbilityValue6,AbilityType7,AbilityValue7,AbilityType8,AbilityValue8,AbilityType9,AbilityValue9,AbilityType10,AbilityValue10,";
        out << L"HealHP,HealSP,HealMP,TimeHealHP,TimeHealSP,TimeHealMP,DefenceType,DefenceValue,LimitHP,FixRange,ChangeType,ChangeLevel,UpdateDate"
            << L") VALUES "
            << L"\r\n";
        for (int i = 0; i < m_levelCount; ++i)
        {
            const String& name = m_levelNames[i].m_length ? m_levelNames[i] : m_name;
            String quoted = String::sqlQuote(name.m_buffer);
            out << L'(';
            out << (long)m_skillId << sep << (unsigned char)FIELD(BYTE, &m_levels[i], 0x8) << sep;
            out.append(quoted.m_buffer, quoted.m_length) << sep;
            m_levels[i].SkillData::exportText(out, 'sql ');
            out << L')';
            out << (char)(i == m_levelCount - 1 ? ';' : ',');
            out << L"\r\n";
        }
        return true;
    }

    for (int i = 0; i < m_levelCount; ++i)
        m_levels[i].SkillData::exportText(out, fmt);
    return true;
}

static inline void dropLast(String& s, int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (!s.m_length)
            break;
        s.m_buffer[--s.m_length] = 0;
    }
}

void ToonSkill::baseName(const String& src, String& dst)
{
    if (&src != &dst)
        dst = src;

    if (dst.m_length >= 3 && !_wcsnicmp(dst.m_buffer + dst.m_length - 3, L"...", 3))
        dropLast(dst, 3);

    int length = (int)dst.m_length;
    if (length <= 0)
        return;

    unsigned last = length - 1;
    if (dst.m_buffer && last < (unsigned)length && (unsigned short)(dst.m_buffer[last] - L'1') <= 8)
    {
        dropLast(dst, 1);
        unsigned prev = length - 2;
        if (dst.m_buffer && prev < dst.m_length && dst.m_buffer[prev] == L'1')
            dropLast(dst, 1);
    }
    if (dst.m_length && dst.m_buffer[dst.m_length - 1] == L' ')
        dropLast(dst, 1);

    if (dst.m_length < 3 || _wcsnicmp(dst.m_buffer + dst.m_length - 3, L"lvl", 3))
    {
        if (dst.endsWithNoCase(L"lv"))
            dropLast(dst, 2);
    }
    else
    {
        dropLast(dst, 3);
    }

    if (dst.m_length && dst.m_buffer[dst.m_length - 1] == L' ')
        dropLast(dst, 1);
}
