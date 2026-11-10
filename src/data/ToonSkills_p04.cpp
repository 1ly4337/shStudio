#include "p04_common.h"
#include "ToonSkills.h"
#include "ToonSkill.h"
#include "iStream.h"
#include "oStream.h"
#include "String.hpp"
#include "_imports_p04.h"

HRESULT ToonSkills::setCodePage(int format)
{
    FIELD(int, this, 0x2C) = format;
    for (int i = 0; i < m_count; ++i)
        m_items[i]->setFormat(format);
    return 0;
}

void ToonSkills::read(iStream& in, DWORD fmt)
{
    if (fmt == 'ltxt')
    {
        if (m_buffer2)
        {
            delete[] m_buffer2;
            m_buffer2 = 0;
        }
        m_buffer2 = (wchar_t*)in.readAlloc(128);
        if (in.readDword() != 4)
            throw (const wchar_t*)L"Invalid or not supported DB format";
        Serializable::checkSignature(in, L"id");
        Serializable::checkSignature(in, L"skilllevel");
        Serializable::checkSignature(in, L"name");
        Serializable::checkSignature(in, L"text");
        String skipped;
        int rows = (int)in.readDword();
        if (rows > 0)
        {
            do
            {
                int id = (int)in.readInt64();
                int level = (int)in.readInt64();
                ToonSkill* skill = findById(id);
                if (skill && (unsigned)(level - 1) <= 14)
                {
                    skill->m_levelNames[level - 1].read(in, 4, in.m_codePage);
                    skill->m_levelTexts[level - 1].read(in, 4, in.m_codePage);
                    if (level == 1)
                        ToonSkill::baseName(skill->m_levelNames[0], skill->m_name);
                }
                else
                {
                    skipped.read(in, 4, (UINT)-1);
                    skipped.read(in, 4, (UINT)-1);
                }
                --rows;
            } while (rows);
        }
        return;
    }

    if (m_buffer1)
    {
        delete[] m_buffer1;
        m_buffer1 = 0;
    }
    deleteAll();

    if (fmt == 'flnt')
    {
        m_buffer1 = (wchar_t*)in.readAlloc(128);
        if (in.readDword() != 100)
            throw (const wchar_t*)L"Invalid or not supported DB format";
        FIELD(int, this, 0x2C) = 0x1600;
        readSkillColumnHeaders(in);
        int rows = (int)in.readDword();
        for (int i = 0; i < rows; ++i)
        {
            int id = (int)in.readInt64();
            int level = (int)in.readInt64();
            if ((unsigned)(level - 1) > 14)
                throw (wchar_t*)L"Invalid skill level value (out of range).";
            if (in.m_pos >= 16)
                in.m_pos -= 16;
            ToonSkill* skill = findById(id);
            if (!skill)
            {
                skill = new ToonSkill(id, FIELD(int, this, 0x2C));
                add(skill);
            }
            skill->m_levels[level - 1].SkillData::read(in, 'flnt');
        }
    }
    else if (fmt == 'file')
    {
        int formats[4] = { 0x600, 0x500, 0x400, 0 };
        for (int f = 0;; ++f)
        {
            FIELD(int, this, 0x2C) = formats[f];
            if (!formats[f])
                throw (wchar_t*)L"Unsupported skills format";
            int count = (int)in.readDword();
            try
            {
                for (int id = 1; id <= count; ++id)
                {
                    ToonSkill* skill = new ToonSkill(id, FIELD(int, this, 0x2C));
                    add(skill);
                    skill->read(in, fmt);
                }
                return;
            }
            catch (...)
            {
                deleteAll();
                in.m_pos = 0;
            }
        }
    }
}

void ToonSkills::write(oStream& out, DWORD fmt) const
{
    if (m_count < 1)
        return;
    switch (fmt)
    {
    case 'ltxt':
    {
        if (!m_buffer2)
            throw (char*)"error";
        out.put(m_buffer2, 128);
        out << (unsigned long)4;
        out << L"id";
        out << L"skilllevel";
        out << L"name";
        out << L"text";
        out << (unsigned long)(15 * m_count);
        for (int i = 0; i < m_count; ++i)
        {
            ToonSkill* skill = m_items[i];
            int level = 0;
            do
            {
                out << (__int64)skill->m_skillId;
                out << (__int64)++level;
                skill->m_levelNames[level - 1].write(out, 4, (UINT)-1);
                skill->m_levelTexts[level - 1].write(out, 4, (UINT)-1);
            } while (level < 15);
        }
        break;
    }
    case 'flnt':
        if (!m_buffer1)
            throw (char*)"error";
        out.put(m_buffer1, 128);
        out << (unsigned long)100;
        writeSkillColumnHeaders(out);
        out << (unsigned long)(15 * m_count);
        for (int i = 0; i < m_count; ++i)
            m_items[i]->write(out, 'flnt');
        break;
    case 'file':
        out << (unsigned long)m_count;
        for (int i = 0; i < m_count; ++i)
            m_items[i]->write(out, 'file');
        break;
    }
}

bool ToonSkills::toText(String& out, DWORD fmt) const
{
    out.clear();
    if (fmt == '----' || fmt == 'csv ')
    {
        out << L"SkillID;SkillLevel;SkillName;Description;Icon;Ani;Country;Fighter;Defenser;Ranger;Archer;Mage;Healer;PrevSkill;ReqLevel;Grow;Point;TypeShow;TypeAttack;TypeEffect;TypeDetail;NeedWeapon1;NeedWeapon2;NeedWeapon3;NeedWeapon4;NeedWeapon5;NeedWeapon6;NeedWeapon7;NeedWeapon8;NeedWeapon9;NeedWeapon10;NeedWeapon11;NeedWeapon12;NeedWeapon13;NeedWeapon14;NeedWeapon15;Shield;SP;MP;ReadyTime;ResetTime;AttackRange;StateType;AttribType;Disable;SuccessType;SuccessValue;TargetType;ApplyRange;MultiAttack;KeepTime;Weapon1;Weapon2;WeaponValue;Bag;Arrow;DamageType;DamageHP;DamageSP;DamageMP;TimeDamageType;TimeDamageHP;TimeDamageSP;TimeDamageMP;AddDamageHP;AddDamageSP;AddDamageMP;AbilityType1;AbilityValue1;AbilityType2;AbilityValue2;AbilityType3;AbilityValue3;";
        if (FIELD(int, this, 0x2C) >= 0x600)
            out << L"AbilityType4;AbilityValue4;AbilityType5;AbilityValue5;AbilityType6;AbilityValue6;AbilityType7;AbilityValue7;AbilityType8;AbilityValue8;AbilityType9;AbilityValue9;AbilityType10;AbilityValue10;";
        out << L"HealHP;HealSP;HealMP;TimeHealHP;TimeHealSP;TimeHealMP;DefenceType;DefenceValue;LimitHP;FixRange;ChangeType;ChangeLevel"
            << L"\r\n";
        for (int i = 0; i < m_count; ++i)
            m_items[i]->toText(out, fmt);
        return true;
    }
    if (fmt != 'sql ')
        return true;
    out << "DELETE FROM [PS_GameDefs].[dbo].[Skills] WHERE SkillLevel<100;" << L"\r\n";
    out << "GO" << L"\r\n";
    for (int i = 0; i < m_count; ++i)
        m_items[i]->toText(out, 'sql ');
    return true;
}
