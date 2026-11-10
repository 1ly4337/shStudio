#include "MobsSkills.h"
#include "MobsSkill.h"
#include "String.hpp"
#include "String_p02.h"
#include "Serializable.h"
#include "iStream.h"
#include "oStream.h"
#include "_imports_w1.h"

void readSkillColumnHeaders(iStream& s)
{
    Serializable::checkSignature(s, L"id");
    Serializable::checkSignature(s, L"skilllevel");
    Serializable::checkSignature(s, L"image");
    Serializable::checkSignature(s, L"ani");
    Serializable::checkSignature(s, L"effect");
    Serializable::checkSignature(s, L"toggletype");
    Serializable::checkSignature(s, L"sound");
    Serializable::checkSignature(s, L"level");
    Serializable::checkSignature(s, L"country");
    Serializable::checkSignature(s, L"attackfighter");
    Serializable::checkSignature(s, L"defensefighter");
    Serializable::checkSignature(s, L"patrolrogue");
    Serializable::checkSignature(s, L"shootrogue");
    Serializable::checkSignature(s, L"attackmage");
    Serializable::checkSignature(s, L"defensemage");
    Serializable::checkSignature(s, L"grow");
    Serializable::checkSignature(s, L"point");
    Serializable::checkSignature(s, L"typeshow");
    Serializable::checkSignature(s, L"typeattack");
    Serializable::checkSignature(s, L"typeeffect");
    Serializable::checkSignature(s, L"type");
    String base;
    base.append(L"needweapon");
    for (int i = 1; i <= 15; ++i)
        Serializable::checkSignature(s, (base + i).m_buffer);
    Serializable::checkSignature(s, L"shield");
    Serializable::checkSignature(s, L"sp");
    Serializable::checkSignature(s, L"mp");
    Serializable::checkSignature(s, L"readytime");
    Serializable::checkSignature(s, L"resettime");
    Serializable::checkSignature(s, L"attackrange");
    Serializable::checkSignature(s, L"statetype");
    Serializable::checkSignature(s, L"attribtype");
    Serializable::checkSignature(s, L"disable");
    Serializable::checkSignature(s, L"prevskill");
    Serializable::checkSignature(s, L"successtype");
    Serializable::checkSignature(s, L"successvalue");
    Serializable::checkSignature(s, L"targettype");
    Serializable::checkSignature(s, L"applyrange");
    Serializable::checkSignature(s, L"multiattack");
    Serializable::checkSignature(s, L"keeptime");
    base = L"weapon";
    for (int j = 1; j <= 2; ++j)
        Serializable::checkSignature(s, (base + j).m_buffer);
    Serializable::checkSignature(s, L"weaponvalue");
    Serializable::checkSignature(s, L"bag");
    Serializable::checkSignature(s, L"arrow");
    Serializable::checkSignature(s, L"damagetype");
    base = L"damage";
    for (int k = 1; k <= 3; ++k)
        Serializable::checkSignature(s, (base + k).m_buffer);
    Serializable::checkSignature(s, L"timedamagetype");
    base = L"timedamage";
    for (int m = 1; m <= 3; ++m)
        Serializable::checkSignature(s, (base + m).m_buffer);
    base = L"adddamage";
    for (int n = 1; n <= 3; ++n)
        Serializable::checkSignature(s, (base + n).m_buffer);
    {
        String abilityType;
        abilityType.append(L"abilitytype", 11);
        String abilityValue;
        abilityValue.append(L"abilityvalue", 12);
        for (int ii = 1; ii <= 10; ++ii)
        {
            Serializable::checkSignature(s, (abilityType + ii).m_buffer);
            Serializable::checkSignature(s, (abilityValue + ii).m_buffer);
        }
        base = L"heal";
        for (int jj = 1; jj <= 3; ++jj)
            Serializable::checkSignature(s, (base + jj).m_buffer);
        base = L"timeheal";
        for (int kk = 1; kk <= 3; ++kk)
            Serializable::checkSignature(s, (base + kk).m_buffer);
        Serializable::checkSignature(s, L"defencetype");
        Serializable::checkSignature(s, L"defencevalue");
        Serializable::checkSignature(s, L"limithp");
        Serializable::checkSignature(s, L"fixrange");
        Serializable::checkSignature(s, L"changetype");
        Serializable::checkSignature(s, L"changelevel");
    }
}

void writeSkillColumnHeaders(oStream& s)
{
    s << L"id";
    s << L"skilllevel";
    s << L"image";
    s << L"ani";
    s << L"effect";
    s << L"toggletype";
    s << L"sound";
    s << L"level";
    s << L"country";
    s << L"attackfighter";
    s << L"defensefighter";
    s << L"patrolrogue";
    s << L"shootrogue";
    s << L"attackmage";
    s << L"defensemage";
    s << L"grow";
    s << L"point";
    s << L"typeshow";
    s << L"typeattack";
    s << L"typeeffect";
    s << L"type";
    String base;
    base.append(L"needweapon");
    for (int i = 1; i <= 15; ++i)
        s << (const wchar_t*)(base + i).m_buffer;
    s << L"shield";
    s << L"sp";
    s << L"mp";
    s << L"readytime";
    s << L"resettime";
    s << L"attackrange";
    s << L"statetype";
    s << L"attribtype";
    s << L"disable";
    s << L"prevskill";
    s << L"successtype";
    s << L"successvalue";
    s << L"targettype";
    s << L"applyrange";
    s << L"multiattack";
    s << L"keeptime";
    base = L"weapon";
    for (int j = 1; j <= 2; ++j)
        s << (const wchar_t*)(base + j).m_buffer;
    s << L"weaponvalue";
    s << L"bag";
    s << L"arrow";
    s << L"damagetype";
    base = L"damage";
    for (int k = 1; k <= 3; ++k)
        s << (const wchar_t*)(base + k).m_buffer;
    s << L"timedamagetype";
    base = L"timedamage";
    for (int m = 1; m <= 3; ++m)
        s << (const wchar_t*)(base + m).m_buffer;
    base = L"adddamage";
    for (int n = 1; n <= 3; ++n)
        s << (const wchar_t*)(base + n).m_buffer;
    {
        String abilityType;
        abilityType.append(L"abilitytype", 11);
        String abilityValue;
        abilityValue.append(L"abilityvalue", 12);
        for (int ii = 1; ii <= 10; ++ii)
        {
            s << (const wchar_t*)(abilityType + ii).m_buffer;
            s << (const wchar_t*)(abilityValue + ii).m_buffer;
        }
        base = L"heal";
        for (int jj = 1; jj <= 3; ++jj)
            s << (const wchar_t*)(base + jj).m_buffer;
        base = L"timeheal";
        for (int kk = 1; kk <= 3; ++kk)
            s << (const wchar_t*)(base + kk).m_buffer;
        s << L"defencetype";
        s << L"defencevalue";
        s << L"limithp";
        s << L"fixrange";
        s << L"changetype";
        s << L"changelevel";
    }
}

HRESULT MobsSkills::setCodePage(int fmt)
{
    m_format = fmt;
    for (int i = 0; i < m_count; ++i)
        m_items[i]->m_skill.version = m_format;
    return 0;
}

void MobsSkills::read(iStream& in, DWORD fmt)
{
    if (fmt == 0x6C747874 )
    {
        if (m_buffer2)
        {
            delete[] m_buffer2;
            m_buffer2 = 0;
        }
        m_buffer2 = (wchar_t*)in.readAlloc(0x80);
        if (in.readInt() != 4)
            throw L"Invalid or not supported DB format";
        Serializable::checkSignature(in, L"id");
        Serializable::checkSignature(in, L"skilllevel");
        Serializable::checkSignature(in, L"name");
        Serializable::checkSignature(in, L"text");
        int count = (int)in.readInt();
        String discard;
        if (count > 0)
        {
            do
            {
                int id = (int)in.readInt64();
                in.readInt64();
                int i = 0;
                for (; i < m_count; ++i)
                {
                    if (m_items[i]->m_skill.skillId == id)
                        break;
                }
                MobsSkill* skill = (i < m_count) ? m_items[i] : 0;
                if (skill)
                {
                    skill->m_name.read(in, 4, (UINT)-1);
                    skill->m_desc.read(in, 4, (UINT)-1);
                }
                else
                {
                    discard.read(in, 4, (UINT)-1);
                    discard.read(in, 4, (UINT)-1);
                }
            } while (--count);
        }
        return;
    }

    if (m_buffer1)
    {
        delete[] m_buffer1;
        m_buffer1 = 0;
    }
    deleteAll();

    if (fmt == 0x666C6E74 )
    {
        m_buffer1 = (wchar_t*)in.readAlloc(0x80);
        if (in.readDword() != 100)
            throw L"Invalid or not supported DB format";
        m_format = 0x1600;
        readSkillColumnHeaders(in);
        int count = (int)in.readDword();
        if (count >= 1)
        {
            do
            {
                int id = (int)in.readInt64();
                in.readInt64();
                if (in.m_pos >= 16)
                    in.m_pos -= 16;
                MobsSkill* skill = new MobsSkill(id, 0x1620, 0);
                add(skill);
                skill->read(in, 0x666C6E74 );
            } while (--count);
        }
    }
    else if (fmt == 0x66696C65 )
    {
        int formats[4] = { 0x600, 0x500, 0x400, 0 };
        int fi = 0;
        for (;;)
        {
            deleteAll();
            in.m_pos = 0;
            m_format = formats[fi];
            if (!m_format)
                throw 1;
            int levels = 9;
            if (m_format == 0x400)
                levels = 3;
            else if (m_format >= 0x600)
                levels = 15;
            MobsSkill* current = 0;
            try
            {
                MobsSkill extraLevel(m_format, 0, 0);
                int count = (int)in.readInt();
                for (int i = 0; i < count; ++i)
                {
                    current = new MobsSkill(m_format, (short)(i + 1), 0);
                    current->read(in, fmt);
                    add(current);
                    current = 0;
                    for (int j = 1; j < levels; ++j)
                        extraLevel.MobsSkill::read(in, fmt);
                }
                if (in.m_size - in.m_pos >= 32)
                    throw 2;
                return;
            }
            catch (...)
            {
                if (current)
                    delete current;
            }
            ++fi;
        }
    }
}

void MobsSkills::write(oStream& out, DWORD fmt) const
{
    if (m_count < 1)
        return;
    switch (fmt)
    {
    case 0x6C747874:
        if (!m_buffer2)
            throw "error";
        out.put(m_buffer2, 0x80);
        out << (unsigned long)4;
        out << L"id";
        out << L"skilllevel";
        out << L"name";
        out << L"text";
        out << (unsigned long)m_count;
        for (int i = 0; i < m_count; ++i)
        {
            MobsSkill* skill = m_items[i];
            out << (__int64)skill->m_skill.skillId;
            out << (__int64)1;
            skill->m_name.write(out, 4, (UINT)-1);
            skill->m_desc.write(out, 4, (UINT)-1);
        }
        break;

    case 0x666C6E74:
        if (!m_buffer1)
            throw "error";
        out.put(m_buffer1, 0x80);
        out << (unsigned long)100;
        writeSkillColumnHeaders(out);
        out << (unsigned long)m_count;
        for (int j = 0; j < m_count; ++j)
            m_items[j]->m_skill.write(out, 0x666C6E74 );
        break;

    case 0x66696C65:
    {
        int levels = 9;
        if (m_format == 0x400)
            levels = 3;
        else if (m_format >= 0x600)
            levels = 15;
        MobsSkill extraLevel(m_format, 0, 0);
        out << (unsigned long)m_count;
        for (int i = 0; i < m_count; ++i)
        {
            m_items[i]->write(out, 0x66696C65 );
            for (int k = levels - 1; k > 0; --k)
                extraLevel.MobsSkill::write(out, 0x66696C65 );
        }
        break;
    }
    }
}

bool MobsSkills::toText(String& out, DWORD fmt) const
{
    out.clear();
    if (fmt == 0x2D2D2D2D  || fmt == 0x63737620 )
    {
        out << L"SkillID;SkillLevel;SkillName;Description;Icon;Ani;Country;Fighter;Defenser;Ranger;Archer;Mage;Healer;PrevSkill;ReqLevel;Grow;Point;TypeShow;TypeAttack;TypeEffect;TypeDetail;NeedWeapon1;NeedWeapon2;NeedWeapon3;NeedWeapon4;NeedWeapon5;NeedWeapon6;NeedWeapon7;NeedWeapon8;NeedWeapon9;NeedWeapon10;NeedWeapon11;NeedWeapon12;NeedWeapon13;NeedWeapon14;NeedWeapon15;Shield;SP;MP;ReadyTime;ResetTime;AttackRange;StateType;AttribType;Disable;SuccessType;SuccessValue;TargetType;ApplyRange;MultiAttack;KeepTime;Weapon1;Weapon2;WeaponValue;Bag;Arrow;DamageType;DamageHP;DamageSP;DamageMP;TimeDamageType;TimeDamageHP;TimeDamageSP;TimeDamageMP;AddDamageHP;AddDamageSP;AddDamageMP;AbilityType1;AbilityValue1;AbilityType2;AbilityValue2;AbilityType3;AbilityValue3;";
        if (m_format >= 0x600)
            out << L"AbilityType4;AbilityValue4;AbilityType5;AbilityValue5;AbilityType6;AbilityValue6;AbilityType7;AbilityValue7;AbilityType8;AbilityValue8;AbilityType9;AbilityValue9;AbilityType10;AbilityValue10;";
        out << L"HealHP;HealSP;HealMP;TimeHealHP;TimeHealSP;TimeHealMP;DefenceType;DefenceValue;LimitHP;FixRange;ChangeType;ChangeLevel"
            << L"\r\n";
        for (int i = 0; i < m_count; ++i)
            m_items[i]->toText(out, fmt);
        return true;
    }
    if (fmt == 0x73716C20 )
    {
        out << L"DELETE FROM [PS_GameDefs].[dbo].[Skills] WHERE SkillLevel=100;" << L"\r\n";
        out << L"GO" << L"\r\n";
        for (int i = 0; i < m_count; ++i)
            m_items[i]->toText(out, 0x73716C20 );
        return true;
    }
    for (int i = 0; i < m_count; ++i)
        m_items[i]->toText(out, fmt);
    return true;
}
