#include "Preferences.h"
#include "Merchant.h"
#include "_w2_inl.h"
#include <cwchar>

const wchar_t* g_merchantTypeNames[18] = {
    L"Generic Merchant",
    L"Weapon Merchant",
    L"Protection Merchant",
    L"Accessory Merchant",
    L"Liquid Medecine Merchant",
    L"Professional Trader",
    L"Riding Merchant",
    L"Mantle Merchant",
    L"Badge Merchant",
    L"Bank Teller",
    L"Gold Bar Merchant",
    L"Episo 6.x Merchant",
    L"Professional Blacksmith 1",
    L"Professional Blacksmith 2",
    L"Etin Manager",
    L"Etin Seller",
    L"Action Board",
    L"PvP Quests",
};

wchar_t g_merchantItemsBuf[0x200];

Merchant::Merchant()
    : StandardNPC(1)
{
    m_prof = 0;
    m_lastEmpty = 0;
}

Merchant::~Merchant()
{
}

void Merchant::reset()
{
    StandardNPC::reset();
    m_items.m_count = 0;
    m_lastEmpty = 0;
}

int Merchant::profIndex(unsigned char prof)
{
    if (prof == 0)   return 0;
    if (prof == 1)   return 1;
    if (prof == 2)   return 2;
    if (prof == 3)   return 3;
    if (prof == 4)   return 4;
    if (prof == 5)   return 5;
    if (prof == 6)   return 6;
    if (prof == 7)   return 7;
    if (prof == 8)   return 8;
    if (prof == 9)   return 9;
    if (prof == 10)  return 10;
    if (prof == 11)  return 11;
    if (prof == 15)  return 12;
    if (prof == 16)  return 13;
    if (prof == 100) return 14;
    if (prof == 101) return 15;
    int result = 16;
    if (prof != 117)
        return 17;
    return result;
}

const wchar_t* Merchant::getString(DWORD tag) const
{
    if (tag == 0x7369746D ) {
        String tmp;
        tmp << reinterpret_cast<const primitiveVector<long>&>(m_items);
        const wchar_t* r;
        if (tmp.m_buffer) {
            swprintf_s(g_merchantItemsBuf, 0x200u, L"%s", tmp.m_buffer);
            r = g_merchantItemsBuf;
        } else {
            r = L"";
        }
        return r;
    }
    if (tag == 0x70726F66 )
        return g_merchantTypeNames[profIndex(m_prof)];
    return StandardNPC::getString(tag);
}

int Merchant::getInt(DWORD tag) const
{
    if (tag == 0x70726F66 )
        return m_prof;
    if (tag == 0x6B69746D )
        return m_items.m_count;
    return StandardNPC::getInt(tag);
}

bool Merchant::setInt(DWORD tag, int value)
{
    if (tag != 0x70726F66 )
        return StandardNPC::setInt(tag, value);
    m_prof = (uint8_t)value;
    return true;
}

int Merchant::compare(const Node* otherNode, DWORD tag) const
{
    const Merchant* other = &dynamic_cast<const Merchant&>(*otherNode);
    if (tag == 0x70726F66 )
        return w2_cmpU(m_prof, other->m_prof);
    if (tag == 0x6B69746D )
        return w2_cmpS(m_items.m_count, other->m_items.m_count);
    return StandardNPC::compare(otherNode, tag);
}

void Merchant::assign(const StandardNPC* src)
{
    StandardNPC::assign(src);
    const Merchant* m = dynamic_cast<const Merchant*>(src);
    if (m) {
        m_prof = m->m_prof;
        m_items.m_count = 0;
        m_items.addAll(m->m_items.m_count, m->m_items.m_items);
    }
}

void Merchant::readMain(iStream& s, int tag)
{
    switch (tag) {
    case 0x66696C65 : {
        uint8_t type = s.readByte();
        m_id = s.readShort();
        m_prof = s.readByte();
        m_shape = s.readInt();
        m_unk1 = s.readInt();
        m_unk2 = s.readInt();
        m_country = s.readInt();
        m_type = type;
        m_name.read(s, 4, static_cast<aStream&>(s).m_codePage);
        m_welcome.read(s, 4, static_cast<aStream&>(s).m_codePage);
        m_welcome = m_welcome.unescape();
        return;
    }
    case 0x666C6E74 : {
        uint8_t type = s.readByte();
        m_id = s.readShort();
        m_prof = s.readByte();
        m_shape = s.readInt();
        m_unk1 = s.readInt();
        m_unk2 = s.readInt();
        m_country = s.readInt();
        m_type = type;
        return;
    }
    case 0x6C747874 : {
        String tmp;
        m_name.read(s, 4, 0xFFFFFFFF);
        tmp.read(s, 4, 0xFFFFFFFF);
        m_welcome = tmp.unescape();
        return;
    }
    }
}

void Merchant::writeMain(oStream& s, int tag) const
{
    switch (tag) {
    case 0x66696C65 : {
        s << (char)m_type << (unsigned short)m_id << (char)m_prof << (unsigned long)m_shape
          << (unsigned long)m_unk1 << (unsigned long)m_unk2 << (unsigned long)m_country;
        String welc = m_welcome.escape();
        m_name.write(s, 4, static_cast<aStream&>(s).m_codePage);
        welc.write(s, 4, static_cast<aStream&>(s).m_codePage);
        return;
    }
    case 0x666C6E74 :
        s << (char)m_type << (unsigned short)m_id << (char)m_prof << (unsigned long)m_shape
          << (unsigned long)m_unk1 << (unsigned long)m_unk2 << (unsigned long)m_country;
        return;
    case 0x6C747874 : {
        m_name.write(s, 4, 0xFFFFFFFF);
        String welc = m_welcome.escape();
        welc.write(s, 4, 0xFFFFFFFF);
        return;
    }
    }
}

void Merchant::read(iStream& s, DWORD tag)
{
    if (tag == 0x6C747874 ) {
        m_name.read(s, 4, static_cast<aStream&>(s).m_codePage);
        m_welcome.read(s, 4, static_cast<aStream&>(s).m_codePage);
        m_welcome = m_welcome.unescape();
        return;
    }
    bool cleanMerchant = g_preferences.getInt(L"NPCQuest", L"clean merchant", 1) == 1;
    this->readMain(s, tag);
    int n = s.readInt();
    for (int i = 0; i < n; ++i) {
        int hi = 1000 * (uint8_t)s.readByte();
        uint8_t lo = s.readByte();
        if (hi + lo) {
            m_items.add(hi + lo);
        } else if (cleanMerchant) {
            if (i == n - 1)
                m_lastEmpty = 1;
        } else {
            m_items.add(0);
        }
    }
    this->readLists(s, tag);
}

void Merchant::write(oStream& s, DWORD tag) const
{
    this->writeMain(s, tag);
    if (tag != 0x6C747874 ) {
        int count = m_items.m_count;
        s << (unsigned long)(m_lastEmpty ? count + 1 : count);
        for (int i = 0; i < count; ++i) {
            if (i < 0 || i >= m_items.m_count)
                w2_throwIndexOutOfBounds(i);
            int item = m_items.m_items[i];
            int hi = item / 1000;
            w2_putByte(s, (uint8_t)hi);
            w2_putByte(s, (uint8_t)(item + 24 * hi));
        }
        if (m_lastEmpty)
            s << (unsigned short)0;
    }
    this->writeLists(s, tag);
}
