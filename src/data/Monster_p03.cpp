#include "FlntRows.h"
#include "Monster.h"
#include "String.hpp"
#include "iStream.hpp"
#include "oStream.hpp"
#include "_imports_w0.h"
#include <shlwapi.h>

template <class T> static inline int cmpSigned(T a, T b)   { return a == b ? 0 : (a < b ? -1 : 1); }
template <class T> static inline int cmpUnsigned(T a, T b) { return a == b ? 0 : (a < b ? -1 : 1); }

int Monster::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 0x6D626964 : return (int)m_id;
    case 0x6D6F646C : return (int)(short)m_model;
    case 0x6C766C20 : return (int)(short)m_level;
    case 0x6578705F : return (int)(unsigned short)m_exp;
    case 0x61692020 : return (int)(unsigned char)m_ai;
    case 0x676C6431 : return (int)(unsigned short)m_gold1;
    case 0x676C6432 : return (int)(unsigned short)m_gold2;
    case 0x71757374 : return (int)(short)m_quest;
    case 0x68702020 : return (int)m_hp;
    case 0x73702020 : return (int)(unsigned short)m_sp;
    case 0x6D702020 : return (int)(unsigned short)m_mp;
    case 0x64657820 : return (int)(unsigned short)m_dex;
    case 0x77697320 : return (int)(unsigned short)m_wis;
    case 0x6C756320 : return (int)(unsigned short)m_luc;
    case 0x64617920 : return (int)(unsigned char)m_day;
    case 0x73697A65 : return (int)(unsigned char)m_size;
    case 0x61747472 : return (int)(unsigned char)m_attrib;
    case 0x64656673 : return (int)(unsigned short)m_defense;
    case 0x6D616763 : return (int)(unsigned short)m_magic;
    case 0x72733031 : return (int)(unsigned char)m_resist[0];
    case 0x72733032 : return (int)(unsigned char)m_resist[1];
    case 0x72733033 : return (int)(unsigned char)m_resist[2];
    case 0x72733034 : return (int)(unsigned char)m_resist[3];
    case 0x72733035 : return (int)(unsigned char)m_resist[4];
    case 0x72733036 : return (int)(unsigned char)m_resist[5];
    case 0x72733037 : return (int)(unsigned char)m_resist[6];
    case 0x72733038 : return (int)(unsigned char)m_resist[7];
    case 0x72733039 : return (int)(unsigned char)m_resist[8];
    case 0x72733130 : return (int)(unsigned char)m_resist[9];
    case 0x72733131 : return (int)(unsigned char)m_resist[10];
    case 0x72733132 : return (int)(unsigned char)m_resist[11];
    case 0x72733133 : return (int)(unsigned char)m_resist[12];
    case 0x72733134 : return (int)(unsigned char)m_resist[13];
    case 0x72733135 : return (int)(unsigned char)m_resist[14];
    case 0x726B3031 : return (int)(unsigned char)m_rk[0];
    case 0x726B3032 : return (int)(unsigned char)m_rk[1];
    case 0x726B3033 : return (int)(unsigned char)m_rk[2];
    case 0x726B3034 : return (int)(unsigned char)m_rk[3];
    case 0x726B3035 : return (int)(unsigned char)m_rk[4];
    case 0x726B3036 : return (int)(unsigned char)m_rk[5];
    case 0x6E746D65 : return (int)m_normalTime;
    case 0x6E737470 : return (int)(unsigned char)m_normalStep;
    case 0x63746D65 : return (int)m_chaseTime;
    case 0x63737470 : return (int)(unsigned char)m_chaseStep;
    case 0x63726E67 : return (int)(unsigned char)m_chaseRange;
    case 0x31747970 : return (int)(short)m_attack[0].type;
    case 0x31746D65 : return (int)m_attack[0].time;
    case 0x31726E67 : return (int)(unsigned char)m_attack[0].range;
    case 0x31646D67 : return (int)(unsigned short)m_attack[0].damage;
    case 0x31616464 : return (int)(unsigned short)m_attack[0].add;
    case 0x31617472 : return (int)(unsigned char)m_attack[0].attrib;
    case 0x31737063 : return (int)(unsigned char)m_attack[0].special;
    case 0x316D6F64 : return (int)(unsigned char)m_attack[0].mode;
    case 0x32747970 : return (int)(short)m_attack[1].type;
    case 0x32746D65 : return (int)m_attack[1].time;
    case 0x32726E67 : return (int)(unsigned char)m_attack[1].range;
    case 0x32646D67 : return (int)(unsigned short)m_attack[1].damage;
    case 0x32616464 : return (int)(unsigned short)m_attack[1].add;
    case 0x32617472 : return (int)(unsigned char)m_attack[1].attrib;
    case 0x32737063 : return (int)(unsigned char)m_attack[1].special;
    case 0x326D6F64 : return (int)(unsigned char)m_attack[1].mode;
    case 0x33747970 : return (int)(short)m_attack[2].type;
    case 0x33746D65 : return (int)m_attack[2].time;
    case 0x33726E67 : return (int)(unsigned char)m_attack[2].range;
    case 0x33646D67 : return (int)(unsigned short)m_attack[2].damage;
    case 0x33616464 : return (int)(unsigned short)m_attack[2].add;
    case 0x33617472 : return (int)(unsigned char)m_attack[2].attrib;
    case 0x33737063 : return (int)(unsigned char)m_attack[2].special;
    case 0x336D6F64 : return (int)(unsigned char)m_attack[2].mode;
    case 0x63747970 : return (int)(short)m_colorType;
    case 0x63687565 : return (int)(unsigned char)m_colorHue;
    case 0x63736174 : return (int)(unsigned char)m_colorSat;
    case 0x636C6774 : return (int)(unsigned char)m_colorLight;
    }
    if ((unsigned int)(tag - 0x64727031 ) <= 8)
        return (unsigned short)m_dropGroup[tag - 0x64727031];
    if ((unsigned int)(tag - 0x64727431 ) > 8)
        return 0;
    return m_dropRate[tag - 0x64727431];
}

bool Monster::setString(DWORD tag, const wchar_t* text)
{
    if (tag != 0x6E616D65 )
        return false;
    m_name = text;
    return true;
}

bool Monster::setInt(DWORD tag, int value)
{
    switch (tag)
    {
    case 0x72776964 : return true;
    case 0x6D626964 : m_id = value; return true;
    case 0x6D6F646C : m_model = (short)value; return true;
    case 0x6C766C20 : m_level = (short)value; return true;
    case 0x6578705F : m_exp = (short)value; return true;
    case 0x61692020 : m_ai = (unsigned char)value; return true;
    case 0x676C6431 : m_gold1 = (short)value; return true;
    case 0x676C6432 : m_gold2 = (short)value; return true;
    case 0x71757374 : m_quest = (short)value; return true;
    case 0x68702020 : m_hp = value; return true;
    case 0x73702020 : m_sp = (short)value; return true;
    case 0x6D702020 : m_mp = (short)value; return true;
    case 0x64657820 : m_dex = (short)value; return true;
    case 0x77697320 : m_wis = (short)value; return true;
    case 0x6C756320 : m_luc = (short)value; return true;
    case 0x64617920 : m_day = (unsigned char)value; return true;
    case 0x73697A65 : m_size = (unsigned char)value; return true;
    case 0x61747472 : m_attrib = (unsigned char)value; return true;
    case 0x64656673 : m_defense = (short)value; return true;
    case 0x6D616763 : m_magic = (short)value; return true;
    case 0x72733031 : m_resist[0] = (unsigned char)value; return true;
    case 0x72733032 : m_resist[1] = (unsigned char)value; return true;
    case 0x72733033 : m_resist[2] = (unsigned char)value; return true;
    case 0x72733034 : m_resist[3] = (unsigned char)value; return true;
    case 0x72733035 : m_resist[4] = (unsigned char)value; return true;
    case 0x72733036 : m_resist[5] = (unsigned char)value; return true;
    case 0x72733037 : m_resist[6] = (unsigned char)value; return true;
    case 0x72733038 : m_resist[7] = (unsigned char)value; return true;
    case 0x72733039 : m_resist[8] = (unsigned char)value; return true;
    case 0x72733130 : m_resist[9] = (unsigned char)value; return true;
    case 0x72733131 : m_resist[10] = (unsigned char)value; return true;
    case 0x72733132 : m_resist[11] = (unsigned char)value; return true;
    case 0x72733133 : m_resist[12] = (unsigned char)value; return true;
    case 0x72733134 : m_resist[13] = (unsigned char)value; return true;
    case 0x72733135 : m_resist[14] = (unsigned char)value; return true;
    case 0x726B3031 : m_rk[0] = (unsigned char)value; return true;
    case 0x726B3032 : m_rk[1] = (unsigned char)value; return true;
    case 0x726B3033 : m_rk[2] = (unsigned char)value; return true;
    case 0x726B3034 : m_rk[3] = (unsigned char)value; return true;
    case 0x726B3035 : m_rk[4] = (unsigned char)value; return true;
    case 0x726B3036 : m_rk[5] = (unsigned char)value; return true;
    case 0x6E746D65 : m_normalTime = value; return true;
    case 0x6E737470 : m_normalStep = (unsigned char)value; return true;
    case 0x63746D65 : m_chaseTime = value; return true;
    case 0x63737470 : m_chaseStep = (unsigned char)value; return true;
    case 0x63726E67 : m_chaseRange = (unsigned char)value; return true;
    case 0x31616E69 : m_attack[0].ani = (unsigned char)value; return true;
    case 0x31747970 : m_attack[0].type = (short)value; return true;
    case 0x31746D65 : m_attack[0].time = value; return true;
    case 0x31726E67 : m_attack[0].range = (unsigned char)value; return true;
    case 0x31646D67 : m_attack[0].damage = (short)value; return true;
    case 0x31616464 : m_attack[0].add = (short)value; return true;
    case 0x31617472 : m_attack[0].attrib = (unsigned char)value; return true;
    case 0x31737063 : m_attack[0].special = (unsigned char)value; return true;
    case 0x316D6F64 : m_attack[0].mode = (unsigned char)value; return true;
    case 0x32616E69 : m_attack[1].ani = (unsigned char)value; return true;
    case 0x32747970 : m_attack[1].type = (short)value; return true;
    case 0x32746D65 : m_attack[1].time = value; return true;
    case 0x32726E67 : m_attack[1].range = (unsigned char)value; return true;
    case 0x32646D67 : m_attack[1].damage = (short)value; return true;
    case 0x32616464 : m_attack[1].add = (short)value; return true;
    case 0x32617472 : m_attack[1].attrib = (unsigned char)value; return true;
    case 0x32737063 : m_attack[1].special = (unsigned char)value; return true;
    case 0x326D6F64 : m_attack[1].mode = (unsigned char)value; return true;
    case 0x33616E69 : m_attack[2].ani = (unsigned char)value; return true;
    case 0x33747970 : m_attack[2].type = (short)value; return true;
    case 0x33746D65 : m_attack[2].time = value; return true;
    case 0x33726E67 : m_attack[2].range = (unsigned char)value; return true;
    case 0x33646D67 : m_attack[2].damage = (short)value; return true;
    case 0x33616464 : m_attack[2].add = (short)value; return true;
    case 0x33617472 : m_attack[2].attrib = (unsigned char)value; return true;
    case 0x33737063 : m_attack[2].special = (unsigned char)value; return true;
    case 0x336D6F64 : m_attack[2].mode = (unsigned char)value; return true;
    case 0x63747970 : m_colorType = (short)value; return true;
    case 0x63687565 : m_colorHue = (unsigned char)value; return true;
    case 0x63736174 : m_colorSat = (unsigned char)value; return true;
    case 0x636C6774 : m_colorLight = (unsigned char)value; return true;
    default:
        if ((unsigned int)(tag - 0x64727031 ) > 8)
        {
            if ((unsigned int)(tag - 0x64727431 ) > 8)
                return false;
            m_dropRate[tag - 0x64727431] = value;
            return true;
        }
        m_dropGroup[tag - 0x64727031] = (unsigned short)value;
        return true;
    }
}

int Monster::compare(const Node* node, DWORD tag) const
{
    const Monster* other = dynamic_cast<const Monster*>(node);
    if (tag == 0x72776964  || tag == 0x6D626964 )
        return cmpSigned(m_id, other->m_id);
    if (tag == 0x6E616D65 )
    {
        const wchar_t* a = m_name.m_buffer;
        const wchar_t* b = other->m_name.m_buffer;
        if (!a) return -(b != nullptr);
        if (b) return StrCmpLogicalW(a, b);
        return 1;
    }
    switch (tag)
    {
    case 0x6D6F646C : return cmpSigned((short)m_model, (short)other->m_model);
    case 0x6C766C20 : return cmpSigned((short)m_level, (short)other->m_level);
    case 0x6578705F : return cmpUnsigned(m_exp, other->m_exp);
    case 0x61692020 : return cmpUnsigned(m_ai, other->m_ai);
    case 0x676C6431 : return cmpUnsigned(m_gold1, other->m_gold1);
    case 0x676C6432 : return cmpUnsigned(m_gold2, other->m_gold2);
    case 0x71757374 : return cmpSigned((short)m_quest, (short)other->m_quest);
    case 0x68702020 : return cmpSigned(m_hp, other->m_hp);
    case 0x73702020 : return cmpUnsigned(m_sp, other->m_sp);
    case 0x6D702020 : return cmpUnsigned(m_mp, other->m_mp);
    case 0x64657820 : return cmpUnsigned(m_dex, other->m_dex);
    case 0x77697320 : return cmpUnsigned(m_wis, other->m_wis);
    case 0x6C756320 : return cmpUnsigned(m_luc, other->m_luc);
    case 0x64617920 : return cmpUnsigned(m_day, other->m_day);
    case 0x73697A65 : return cmpUnsigned(m_size, other->m_size);
    case 0x61747472 : return cmpUnsigned(m_attrib, other->m_attrib);
    case 0x64656673 : return cmpUnsigned(m_defense, other->m_defense);
    case 0x6D616763 : return cmpUnsigned(m_magic, other->m_magic);
    case 0x72733031 : return cmpUnsigned(m_resist[0], other->m_resist[0]);
    case 0x72733032 : return cmpUnsigned(m_resist[1], other->m_resist[1]);
    case 0x72733033 : return cmpUnsigned(m_resist[2], other->m_resist[2]);
    case 0x72733034 : return cmpUnsigned(m_resist[3], other->m_resist[3]);
    case 0x72733035 : return cmpUnsigned(m_resist[4], other->m_resist[4]);
    case 0x72733036 : return cmpUnsigned(m_resist[5], other->m_resist[5]);
    case 0x72733037 : return cmpUnsigned(m_resist[6], other->m_resist[6]);
    case 0x72733038 : return cmpUnsigned(m_resist[7], other->m_resist[7]);
    case 0x72733039 : return cmpUnsigned(m_resist[8], other->m_resist[8]);
    case 0x72733130 : return cmpUnsigned(m_resist[9], other->m_resist[9]);
    case 0x72733131 : return cmpUnsigned(m_resist[10], other->m_resist[10]);
    case 0x72733132 : return cmpUnsigned(m_resist[11], other->m_resist[11]);
    case 0x72733133 : return cmpUnsigned(m_resist[12], other->m_resist[12]);
    case 0x72733134 : return cmpUnsigned(m_resist[13], other->m_resist[13]);
    case 0x72733135 : return cmpUnsigned(m_resist[14], other->m_resist[14]);
    case 0x726B3031 : return cmpUnsigned(m_rk[0], other->m_rk[0]);
    case 0x726B3032 : return cmpUnsigned(m_rk[1], other->m_rk[1]);
    case 0x726B3033 : return cmpUnsigned(m_rk[2], other->m_rk[2]);
    case 0x726B3034 : return cmpUnsigned(m_rk[3], other->m_rk[3]);
    case 0x726B3035 : return cmpUnsigned(m_rk[4], other->m_rk[4]);
    case 0x726B3036 : return cmpUnsigned(m_rk[5], other->m_rk[5]);
    case 0x6E746D65 : return cmpSigned(m_normalTime, other->m_normalTime);
    case 0x6E737470 : return cmpUnsigned(m_normalStep, other->m_normalStep);
    case 0x63746D65 : return cmpSigned(m_chaseTime, other->m_chaseTime);
    case 0x63737470 : return cmpUnsigned(m_chaseStep, other->m_chaseStep);
    case 0x63726E67 : return cmpUnsigned(m_chaseRange, other->m_chaseRange);
    case 0x31616E69 : return cmpUnsigned(m_attack[0].ani, other->m_attack[0].ani);
    case 0x31747970 : return cmpSigned((short)m_attack[0].type, (short)other->m_attack[0].type);
    case 0x31746D65 : return cmpSigned(m_attack[0].time, other->m_attack[0].time);
    case 0x31726E67 : return cmpUnsigned(m_attack[0].range, other->m_attack[0].range);
    case 0x31646D67 : return cmpUnsigned(m_attack[0].damage, other->m_attack[0].damage);
    case 0x31616464 : return cmpUnsigned(m_attack[0].add, other->m_attack[0].add);
    case 0x31617472 : return cmpUnsigned(m_attack[0].attrib, other->m_attack[0].attrib);
    case 0x31737063 : return cmpUnsigned(m_attack[0].special, other->m_attack[0].special);
    case 0x316D6F64 : return cmpUnsigned(m_attack[0].mode, other->m_attack[0].mode);
    case 0x32616E69 : return cmpUnsigned(m_attack[1].ani, other->m_attack[1].ani);
    case 0x32747970 : return cmpSigned((short)m_attack[1].type, (short)other->m_attack[1].type);
    case 0x32746D65 : return cmpSigned(m_attack[1].time, other->m_attack[1].time);
    case 0x32726E67 : return cmpUnsigned(m_attack[1].range, other->m_attack[1].range);
    case 0x32646D67 : return cmpUnsigned(m_attack[1].damage, other->m_attack[1].damage);
    case 0x32616464 : return cmpUnsigned(m_attack[1].add, other->m_attack[1].add);
    case 0x32617472 : return cmpUnsigned(m_attack[1].attrib, other->m_attack[1].attrib);
    case 0x32737063 : return cmpUnsigned(m_attack[1].special, other->m_attack[1].special);
    case 0x326D6F64 : return cmpUnsigned(m_attack[1].mode, other->m_attack[1].mode);
    case 0x33616E69 : return cmpUnsigned(m_attack[2].ani, other->m_attack[2].ani);
    case 0x33747970 : return cmpSigned((short)m_attack[2].type, (short)other->m_attack[2].type);
    case 0x33746D65 : return cmpSigned(m_attack[2].time, other->m_attack[2].time);
    case 0x33726E67 : return cmpUnsigned(m_attack[2].range, other->m_attack[2].range);
    case 0x33646D67 : return cmpUnsigned(m_attack[2].damage, other->m_attack[2].damage);
    case 0x33616464 : return cmpUnsigned(m_attack[2].add, other->m_attack[2].add);
    case 0x33617472 : return cmpUnsigned(m_attack[2].attrib, other->m_attack[2].attrib);
    case 0x33737063 : return cmpUnsigned(m_attack[2].special, other->m_attack[2].special);
    case 0x336D6F64 : return cmpUnsigned(m_attack[2].mode, other->m_attack[2].mode);
    case 0x63747970 : return cmpSigned((short)m_colorType, (short)other->m_colorType);
    case 0x63687565 : return cmpUnsigned(m_colorHue, other->m_colorHue);
    case 0x63736174 : return cmpUnsigned(m_colorSat, other->m_colorSat);
    case 0x636C6774 : return cmpUnsigned(m_colorLight, other->m_colorLight);
    case 0x64727031 : return cmpUnsigned(m_dropGroup[0], other->m_dropGroup[0]);
    case 0x64727032 : return cmpUnsigned(m_dropGroup[1], other->m_dropGroup[1]);
    case 0x64727033 : return cmpUnsigned(m_dropGroup[2], other->m_dropGroup[2]);
    case 0x64727034 : return cmpUnsigned(m_dropGroup[3], other->m_dropGroup[3]);
    case 0x64727035 : return cmpUnsigned(m_dropGroup[4], other->m_dropGroup[4]);
    case 0x64727036 : return cmpUnsigned(m_dropGroup[5], other->m_dropGroup[5]);
    case 0x64727037 : return cmpUnsigned(m_dropGroup[6], other->m_dropGroup[6]);
    case 0x64727038 : return cmpUnsigned(m_dropGroup[7], other->m_dropGroup[7]);
    case 0x64727039 : return cmpUnsigned(m_dropGroup[8], other->m_dropGroup[8]);
    case 0x64727431 : return cmpSigned(m_dropRate[0], other->m_dropRate[0]);
    case 0x64727432 : return cmpSigned(m_dropRate[1], other->m_dropRate[1]);
    case 0x64727433 : return cmpSigned(m_dropRate[2], other->m_dropRate[2]);
    case 0x64727434 : return cmpSigned(m_dropRate[3], other->m_dropRate[3]);
    case 0x64727435 : return cmpSigned(m_dropRate[4], other->m_dropRate[4]);
    case 0x64727436 : return cmpSigned(m_dropRate[5], other->m_dropRate[5]);
    case 0x64727437 : return cmpSigned(m_dropRate[6], other->m_dropRate[6]);
    case 0x64727438 : return cmpSigned(m_dropRate[7], other->m_dropRate[7]);
    case 0x64727439 : return cmpSigned(m_dropRate[8], other->m_dropRate[8]);
    default: return 0;
    }
}
void Monster::read(iStream& s, DWORD tag)
{
    FlntRowRead<Monster> flntRow(this, s, tag, 8);
    switch (tag)
    {
    case 0x66696C65 :
        if (m_extended)
        {
            m_id = s.readInt();
            m_name.read(s, 4, (UINT)-1);
            m_model  = s.readShort();
            m_3A     = s.readShort();
            m_level  = s.readShort();
            m_exp    = s.readShort();
            m_ai     = s.readByte();
            m_gold1  = s.readShort();
            m_gold2  = s.readShort();
            m_quest  = s.readShort();
            m_hp     = s.readInt();
            m_sp     = s.readShort();
            m_mp     = s.readShort();
            m_dex    = s.readShort();
            m_wis    = s.readShort();
            m_luc    = s.readShort();
            m_day    = s.readByte();
            m_size   = s.readByte();
            m_attrib = s.readByte();
            m_defense = s.readShort();
            m_magic   = s.readShort();
            for (int i = 0; i < 15; ++i)
                m_resist[i] = s.readByte();
            for (int j = 0; j < 6; ++j)
                m_rk[j] = s.readByte();
            m_normalTime = s.readInt();
            m_normalStep = s.readByte();
            m_chaseTime  = s.readInt();
            m_chaseStep  = s.readByte();
            m_chaseRange = s.readByte();
            for (int k = 0; k < 3; ++k)
                m_attack[k].Monster::Attack::read(s, 0x66696C65 );
            int count = s.readInt();
            for (int i = 0; i < count; ++i)
            {
                m_dropGroup[i] = s.readShort();
                m_dropRate[i]  = s.readInt();
            }
        }
        else
        {
            m_name.read(s, 4, (UINT)-1);
            m_model  = s.readShort();
            m_level  = s.readShort();
            m_ai     = s.readByte();
            m_hp     = s.readInt();
            m_day    = s.readByte();
            m_size   = s.readByte();
            m_attrib = s.readByte();
            m_normalTime = s.readInt();
            m_normalStep = s.readByte();
            m_chaseTime  = s.readInt();
            m_chaseStep  = s.readByte();
            unsigned char b0 = s.readByte();
            unsigned char b1 = s.readByte();
            unsigned char b2 = s.readByte();
            unsigned char b3 = s.readByte();
            s.readByte();
            unsigned char b5 = s.readByte();
            unsigned char b6 = s.readByte();
            m_quest = s.readShort();
            m_attack[0].mode  = b1;
            m_attack[0].type  = b0;
            m_attack[1].range = b3;
            m_attack[1].type  = b2;
            m_attack[2].type  = b2;
            m_attack[2].mode  = b5;
            m_attack[2].add   = b6;
        }
        if ((m_version & 0xFFF) >= 0x620)
        {
            m_colorType  = s.readShort();
            m_colorHue   = s.readByte();
            m_colorSat   = s.readByte();
            m_colorLight = s.readByte();
        }
        break;

    case 0x666C6E74 :
        m_model  = (short)s.readInt64();
        m_level  = (short)s.readInt64();
        m_exp    = (unsigned short)s.readInt64();
        m_ai     = (unsigned char)s.readInt64();
        m_gold1  = (unsigned short)s.readInt64();
        m_gold2  = (unsigned short)s.readInt64();
        for (int i = 0; i < 9; ++i)
        {
            m_dropGroup[i] = (unsigned short)s.readInt64();
            m_dropRate[i]  = s.readInt64();
        }
        m_quest  = (short)s.readInt64();
        m_hp     = s.readInt64();
        m_sp     = (unsigned short)s.readInt64();
        m_mp     = (unsigned short)s.readInt64();
        m_dex    = (unsigned short)s.readInt64();
        m_wis    = (unsigned short)s.readInt64();
        m_luc    = (unsigned short)s.readInt64();
        m_day    = (unsigned char)s.readInt64();
        m_size   = (unsigned char)s.readInt64();
        m_attrib = (unsigned char)s.readInt64();
        m_defense = (unsigned short)s.readInt64();
        m_magic   = (unsigned short)s.readInt64();
        for (int k = 0; k < 15; ++k)
            m_resist[k] = (unsigned char)s.readInt64();
        for (int m = 0; m < 6; ++m)
            m_rk[m] = (unsigned char)s.readInt64();
        m_normalTime = s.readInt64();
        m_normalStep = (unsigned char)s.readInt64();
        m_chaseTime  = s.readInt64();
        m_chaseStep  = (unsigned char)s.readInt64();
        m_chaseRange = (unsigned char)s.readInt64();
        for (int k = 0; k < 3; ++k)
            m_attack[k].Monster::Attack::read(s, 0x666C6E74 );
        m_colorType  = (short)s.readInt64();
        m_colorHue   = (unsigned char)s.readInt64();
        m_colorSat   = (unsigned char)s.readInt64();
        m_colorLight = (unsigned char)s.readInt64();
        break;

    case 0x6C747874 :
        m_name.read(s, 4, (UINT)-1);
        break;
    }
}

void Monster::write(oStream& s, DWORD tag) const
{
    FlntRowWrite flntRow(this, s, tag);
    if (tag == 0x6C747874 )
    {
        s << ((__int64)m_id);
        m_name.write(s, 4, (UINT)-1);
    }
    else if (tag == 0x66696C65 )
    {
        if (m_extended)
        {
            (s << (unsigned long)(m_id));
            m_name.write(s, 4, (UINT)-1);
            (s << (unsigned short)(m_model));
            (s << (unsigned short)(m_3A));
            (s << (unsigned short)(m_level));
            (s << (short)(m_exp));
            (s << (char)(m_ai));
            (s << (short)(m_gold1));
            (s << (short)(m_gold2));
            (s << (unsigned short)(m_quest));
            (s << (unsigned long)(m_hp));
            (s << (short)(m_sp));
            (s << (short)(m_mp));
            (s << (short)(m_dex));
            (s << (short)(m_wis));
            (s << (short)(m_luc));
            (s << (char)(m_day));
            (s << (char)(m_size));
            (s << (char)(m_attrib));
            (s << (short)(m_defense));
            (s << (short)(m_magic));
            for (int i = 0; i < 15; ++i)
                (s << (char)(m_resist[i]));
            for (int i = 0; i < 6; ++i)
                (s << (char)(m_rk[i]));
            (s << (unsigned long)(m_normalTime));
            (s << (char)(m_normalStep));
            (s << (unsigned long)(m_chaseTime));
            (s << (char)(m_chaseStep));
            (s << (char)(m_chaseRange));
            for (int k = 0; k < 3; ++k)
                m_attack[k].Monster::Attack::write(s, 0x66696C65 );
            (s << (unsigned long)(9));
            for (int i = 0; i < 9; ++i)
            {
                (s << (short)(m_dropGroup[i]));
                (s << (unsigned long)(m_dropRate[i]));
            }
            if ((m_version & 0xFFF) >= 0x620)
            {
                (s << (unsigned short)(m_colorType));
                (s << (char)(m_colorHue));
                (s << (char)(m_colorSat));
                (s << (char)(m_colorLight));
            }
        }
        else
        {
            m_name.write(s, 4, (UINT)-1);
            (s << (unsigned short)(m_model));
            (s << (unsigned short)(m_level));
            (s << (char)(m_ai));
            (s << (unsigned long)(m_hp));
            (s << (char)(m_day));
            (s << (char)(m_size));
            (s << (char)(m_attrib));
            (s << (unsigned long)(m_normalTime));
            (s << (char)(m_normalStep));
            (s << (unsigned long)(m_chaseTime));
            (s << (char)(m_chaseStep));
            (s << (char)((unsigned char)m_attack[0].type));
            (s << (char)(m_attack[0].mode));
            (s << (char)((unsigned char)m_attack[1].type));
            (s << (char)(m_attack[1].range));
            (s << (char)((unsigned char)m_attack[2].type));
            (s << (char)(m_attack[2].mode));
            (s << (char)((unsigned char)m_attack[2].add));
            (s << (unsigned short)(m_quest));
            if ((m_version & 0xFFF) >= 0x620)
            {
                (s << (unsigned short)(m_colorType));
                (s << (char)(m_colorHue));
                (s << (char)(m_colorSat));
                (s << (char)(m_colorLight));
            }
        }
    }
    else if (tag == 0x666C6E74 )
    {
        s << ((__int64)m_id);
        s << ((__int64)(short)m_model);
        s << ((__int64)(short)m_level);
        s << ((__int64)(unsigned short)m_exp);
        s << ((__int64)(unsigned char)m_ai);
        s << ((__int64)(unsigned short)m_gold1);
        s << ((__int64)(unsigned short)m_gold2);
        for (int i = 0; i < 9; ++i)
        {
            s << ((__int64)(unsigned short)m_dropGroup[i]);
            s << ((__int64)m_dropRate[i]);
        }
        s << ((__int64)(short)m_quest);
        s << ((__int64)m_hp);
        s << ((__int64)(unsigned short)m_sp);
        s << ((__int64)(unsigned short)m_mp);
        s << ((__int64)(unsigned short)m_dex);
        s << ((__int64)(unsigned short)m_wis);
        s << ((__int64)(unsigned short)m_luc);
        s << ((__int64)(unsigned char)m_day);
        s << ((__int64)(unsigned char)m_size);
        s << ((__int64)(unsigned char)m_attrib);
        s << ((__int64)(unsigned short)m_defense);
        s << ((__int64)(unsigned short)m_magic);
        for (int j = 0; j < 15; ++j)
            s << ((__int64)(unsigned char)m_resist[j]);
        for (int k = 0; k < 6; ++k)
            s << ((__int64)(unsigned char)m_rk[k]);
        s << ((__int64)m_normalTime);
        s << ((__int64)(unsigned char)m_normalStep);
        s << ((__int64)m_chaseTime);
        s << ((__int64)(unsigned char)m_chaseStep);
        s << ((__int64)(unsigned char)m_chaseRange);
        for (int k = 0; k < 3; ++k)
            m_attack[k].Monster::Attack::write(s, 0x666C6E74 );
        s << ((__int64)(short)m_colorType);
        s << ((__int64)(unsigned char)m_colorHue);
        s << ((__int64)(unsigned char)m_colorSat);
        s << ((__int64)(unsigned char)m_colorLight);
    }
}

bool Monster::toText(String& out, DWORD tag) const
{
    switch (tag)
    {
    case 0x64726F70 :
        for (int i = 0; i < 9; )
        {
            String& a = (out << "INSERT INTO [MobItems] values (");
            String& b = (a << (long)(m_id));
            String& c = (b << ", ");
            ++i;
            String& d = (c << (long)(i));
            String& e = (d << ", ");
            String& f = (e << (unsigned short)(m_dropGroup[i - 1]));
            String& g = (f << ", ");
            String& h = (g << (long)(m_dropRate[i - 1]));
            (h << ')');
            h.newline();
        }
        return true;

    case 0x6D6F6273 :
    {
        {
            String quoted = String::sqlQuote(m_name.m_buffer);
            String& a = (out << "INSERT INTO [Mobs] values (");
            String* p = &(a << (long)(m_id));
            (*p << ',');
            p = &p->append(quoted.m_buffer, quoted.m_length);
            (*p << ',');
            p = &(*p << (short)(m_level));
            (*p << ',');
            p = &(*p << (short)((short)m_exp));
            (*p << ',');
            p = &(*p << (unsigned char)(m_ai));
            (*p << ',');
            p = &(*p << (short)((short)m_gold1));
            (*p << ',');
            p = &(*p << (short)((short)m_gold2));
            (*p << ',');
            p = &(*p << (short)(m_quest));
            (*p << ',');
            p = &(*p << (long)(m_hp));
            (*p << ',');
            p = &(*p << (short)((short)m_sp));
            (*p << ',');
            p = &(*p << (short)((short)m_mp));
            (*p << ',');
            p = &(*p << (short)((short)m_dex));
            (*p << ',');
            p = &(*p << (short)((short)m_wis));
            (*p << ',');
            p = &(*p << (short)((short)m_luc));
            (*p << ',');
            p = &(*p << (unsigned char)(m_day));
            (*p << ',');
            p = &(*p << (unsigned char)(m_size));
            (*p << ',');
            p = &(*p << (unsigned char)(m_attrib));
            (*p << ',');
            p = &(*p << (short)((short)m_defense));
            (*p << ',');
            p = &(*p << (short)((short)m_magic));
            (*p << ',');
        }
        for (int i = 0; i < 15; ++i)
            ((out << (unsigned char)(m_resist[i])) << ',');
        for (int i = 0; i < 6; ++i)
            ((out << (unsigned char)(m_rk[i])) << ',');
        String* p = &(out << (long)(m_normalTime));
        (*p << ',');
        p = &(*p << (unsigned char)(m_normalStep));
        (*p << ',');
        p = &(*p << (long)(m_chaseTime));
        (*p << ',');
        p = &(*p << (unsigned char)(m_chaseStep));
        (*p << ',');
        p = &(*p << (unsigned char)(m_chaseRange));
        (*p << ',');
        p = &sub_43D120(out, &m_attack[0]);
        (*p << ',');
        p = &sub_43D120(*p, &m_attack[1]);
        (*p << ',');
        p = &sub_43D120(*p, &m_attack[2]);
        (*p << ')');
        p->newline();
        return true;
    }

    case 0x63737620 :
    {
        char sep = sub_41A900(0x63737620 );
        {
            String quoted = String::sqlQuote(m_name.m_buffer);
            String* p = &(out << (long)(m_id));
            (*p << (char)(sep));
            p = &p->append(quoted.m_buffer, quoted.m_length);
            p = &(*p << (char)(sep));
            p = &(*p << (short)(m_model));
            p = &(*p << (char)(sep));
            p = &(*p << (short)(m_3A));
            p = &(*p << (char)(sep));
            p = &(*p << (short)(m_level));
            p = &(*p << (char)(sep));
            p = &(*p << (unsigned short)(m_exp));
            p = &(*p << (char)(sep));
            p = &(*p << (unsigned char)(m_ai));
            p = &(*p << (char)(sep));
            p = &(*p << (unsigned short)(m_gold1));
            p = &(*p << (char)(sep));
            p = &(*p << (unsigned short)(m_gold2));
            p = &(*p << (char)(sep));
            p = &(*p << (short)(m_quest));
            p = &(*p << (char)(sep));
            p = &(*p << (long)(m_hp));
            p = &(*p << (char)(sep));
            p = &(*p << (unsigned short)(m_sp));
            p = &(*p << (char)(sep));
            p = &(*p << (unsigned short)(m_mp));
            p = &(*p << (char)(sep));
            p = &(*p << (unsigned short)(m_dex));
            p = &(*p << (char)(sep));
            p = &(*p << (unsigned short)(m_wis));
            p = &(*p << (char)(sep));
            p = &(*p << (unsigned short)(m_luc));
            p = &(*p << (char)(sep));
            p = &(*p << (unsigned char)(m_day));
            p = &(*p << (char)(sep));
            p = &(*p << (unsigned char)(m_size));
            p = &(*p << (char)(sep));
            p = &(*p << (unsigned char)(m_attrib));
            p = &(*p << (char)(sep));
            p = &(*p << (unsigned short)(m_defense));
            p = &(*p << (char)(sep));
            (*p << (unsigned short)(m_magic));
        }
        for (int j = 0; j < 15; ++j)
        {
            (out << (char)(sep));
            (out << (unsigned char)(m_resist[j]));
        }
        for (int k = 0; k < 6; ++k)
        {
            (out << (char)(sep));
            (out << (unsigned char)(m_rk[k]));
        }
        (out << (char)(sep));
        String* p = &(out << (long)(m_normalTime));
        (*p << (char)(sep));
        p = &(*p << (unsigned char)(m_normalStep));
        (*p << (char)(sep));
        p = &(*p << (long)(m_chaseTime));
        (*p << (char)(sep));
        p = &(*p << (unsigned char)(m_chaseStep));
        (*p << (char)(sep));
        p = &(*p << (unsigned char)(m_chaseRange));
        (*p << (char)(sep));
        p = &sub_43D120(*p, &m_attack[0]);
        (*p << (char)(sep));
        p = &sub_43D120(*p, &m_attack[1]);
        (*p << (char)(sep));
        sub_43D120(*p, &m_attack[2]);
        for (int i = 0; i < 9; ++i)
        {
            (out << (char)(sep));
            String& q = (out << (unsigned short)(m_dropGroup[i]));
            (q << (char)(sep));
            (q << (long)(m_dropRate[i]));
        }
        if ((m_version & 0xFFFu) >= 0x620)
        {
            (out << (char)(sep));
            p = &(out << (short)(m_colorType));
            (*p << (char)(sep));
            p = &(*p << (unsigned char)(m_colorHue));
            (*p << (char)(sep));
            p = &(*p << (unsigned char)(m_colorSat));
            (*p << (char)(sep));
            (*p << (unsigned char)(m_colorLight));
        }
        return true;
    }

    default:
        return false;
    }
}

void Monster::assignFrom(Monster* src)
{
    if (&src->m_name != &m_name)
        m_name.append(src->m_name.m_buffer, src->m_name.m_length);
    m_model   = src->m_model;
    m_level   = src->m_level;
    m_exp     = src->m_exp;
    m_ai      = src->m_ai;
    m_gold1   = src->m_gold1;
    m_gold2   = src->m_gold2;
    m_quest   = src->m_quest;
    m_hp      = src->m_hp;
    m_sp      = src->m_sp;
    m_mp      = src->m_mp;
    m_dex     = src->m_dex;
    m_wis     = src->m_wis;
    m_luc     = src->m_luc;
    m_day     = src->m_day;
    m_size    = src->m_size;
    m_attrib  = src->m_attrib;
    m_defense = src->m_defense;
    m_magic   = src->m_magic;
    memcpy(m_resist, src->m_resist, 15);
    memcpy(m_rk, src->m_rk, 6);
    m_normalTime = src->m_normalTime;
    m_normalStep = src->m_normalStep;
    m_chaseTime  = src->m_chaseTime;
    m_chaseStep  = src->m_chaseStep;
    m_chaseRange = src->m_chaseRange;
    for (int k = 0; k < 3; ++k)
    {
        m_attack[k].type    = src->m_attack[k].type;
        m_attack[k].time    = src->m_attack[k].time;
        m_attack[k].range   = src->m_attack[k].range;
        m_attack[k].damage  = src->m_attack[k].damage;
        m_attack[k].add     = src->m_attack[k].add;
        m_attack[k].attrib  = src->m_attack[k].attrib;
        m_attack[k].special = src->m_attack[k].special;
        m_attack[k].mode    = src->m_attack[k].mode;
    }
    m_colorType  = src->m_colorType;
    m_colorHue   = src->m_colorHue;
    m_colorSat   = src->m_colorSat;
    m_colorLight = src->m_colorLight;
}
