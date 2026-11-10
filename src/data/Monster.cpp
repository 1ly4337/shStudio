#include "FlntRows.h"
#include "Monster.h"

String sub_43CCF0(const wchar_t* prefix, int n)
{
    String s;
    s.append(prefix);
    s << (long)n;
    return s;
}

String& sub_43D120(String& s, const void* attack)
{
    const Monster::Attack* a = (const Monster::Attack*)attack;
    return s << a->type << L',' << (long)a->time << L',' << (unsigned char)a->range << L','
             << (short)a->damage << L',' << (short)a->add << L',' << (unsigned char)a->attrib << L','
             << (unsigned char)a->special << L',' << (unsigned char)a->mode;
}

void Monster::Attack::read(iStream& in, DWORD tag)
{
    if (tag == 'file')
    {
        type = in.readShort();
        time = in.readInt();
        range = in.readByte();
        damage = (WORD)in.readShort();
        add = (WORD)in.readShort();
        attrib = in.readByte();
        special = in.readByte();
        mode = in.readByte();
    }
    else if (tag == 'flnt')
    {
        ani = (BYTE)in.readInt64();
        type = (short)in.readInt64();
        time = (int)in.readInt64();
        range = (BYTE)in.readInt64();
        damage = (WORD)in.readInt64();
        add = (WORD)in.readInt64();
        attrib = (BYTE)in.readInt64();
        special = (BYTE)in.readInt64();
        mode = (BYTE)in.readInt64();
    }
}

void Monster::Attack::write(oStream& out, DWORD tag) const
{
    if (tag == 'file')
    {
        out << (unsigned short)type << (unsigned long)time << (char)range << (short)damage << (short)add
            << (char)attrib << (char)special << (char)mode;
    }
    else if (tag == 'flnt')
    {
        out << (__int64)ani << (__int64)type << (__int64)time << (__int64)range << (__int64)damage
            << (__int64)add << (__int64)attrib << (__int64)special << (__int64)mode;
    }
}

Monster::Monster(int id, int version, int )
{
    m_version = version;
    m_id = id;
    m_extended = false;
    Monster::reset();
}

Monster::~Monster()
{
    FlntRows::forget(this);
}

int Monster::nodeType() const
{
    return 'mstr';
}

int Monster::getVersion() const
{
    return 522;
}

bool Monster::copyFrom(const Streamable* src)
{
    Monster* p = dynamic_cast<Monster*>(const_cast<Streamable*>(src));
    if (!p)
        return false;
    assignFrom(p);
    return true;
}

void Monster::reset()
{
    m_name.clear();
    m_name << "Error Monster" << (long)m_id;
    m_level = 0;
    m_exp = 0;
    m_model = 0;
    m_quest = 0;
    m_gold1 = 0;
    m_gold2 = 0;
    for (int i = 0; i < 9; ++i)
        m_dropGroup[i] = 0;
    for (int i = 0; i < 9; ++i)
        m_dropRate[i] = 0;
    m_size = 0;
    m_attrib = 0;
    m_day = 0;
    m_ai = 0;
    m_hp = 0;
    m_defense = 0;
    m_magic = 0;
    m_wis = 0;
    m_luc = 0;
    m_mp = 0;
    m_dex = 0;
    m_sp = 0;
    for (int i = 0; i < 15; ++i)
        m_resist[i] = 0;
    for (int i = 0; i < 6; ++i)
        m_rk[i] = 0;
    m_chaseTime = 0;
    m_normalTime = 0;
    m_chaseStep = 0;
    m_chaseRange = 0;
    m_normalStep = 0;
    memset(m_attack, 0, sizeof(m_attack));
    m_colorType = 0;
    m_colorSat = 0x32;
    m_colorLight = 0;
    m_colorHue = 0;
}

const wchar_t* Monster::getString(DWORD tag) const
{
    if (tag == 'name')
        return m_name.m_buffer;
    if (tag != 'attr')
        return 0;
    switch (m_attrib)
    {
    case 1: return L"Fire";
    case 2: return L"Water";
    case 3: return L"Earth";
    case 4: return L"Wind";
    default: return L"None";
    }
}
