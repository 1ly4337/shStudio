#include "StandardNPC.h"
#include "_w2_inl.h"
#include <cwchar>

StandardNPC::StandardNPC(int type)
{
    m_type = type;
    m_id = 0;
    StandardNPC::reset();
}

StandardNPC::~StandardNPC()
{
}

bool StandardNPC::copyFrom(const Streamable* src)
{
    const StandardNPC* npc = dynamic_cast<const StandardNPC*>(src);
    if (!npc)
        return false;
    this->assign(npc);
    return true;
}

const wchar_t* StandardNPC::typeName(int type)
{
    const wchar_t* result;
    switch ((unsigned int)type) {
    case 1u:   result = L"Merchant"; break;
    case 2u:   result = L"Gate-Keeper"; break;
    case 3u:   result = L"Blacksmith"; break;
    case 4u:   result = L"PvP-Manager"; break;
    case 5u:   result = L"Gambling House"; break;
    case 6u:   result = L"Warehouse"; break;
    case 7u:   result = L"Normal NPC"; break;
    case 8u:   result = L"Guard"; break;
    case 9u:   result = L"Animal"; break;
    case 0xAu: result = L"Apprentice"; break;
    case 0xBu: result = L"Guild-Master"; break;
    case 0xCu: result = L"Dead NPC"; break;
    case 0xDu: result = L"Combat Commander"; break;
    default:   result = L"Unknown"; break;
    }
    return result;
}

void StandardNPC::reset()
{
    m_shape = 0;
    m_unk2 = 0;
    m_unk1 = 0;
    m_country = 0;
    m_name.clear();
    m_welcome.clear();
    for (int i = 0; i < m_questsIn.m_capacity; ++i)
        m_questsIn.resetItem(&m_questsIn.m_items[i]);
    m_questsIn.m_count = 0;
    for (int i = 0; i < m_questsOut.m_capacity; ++i)
        m_questsOut.resetItem(&m_questsOut.m_items[i]);
    m_questsOut.m_count = 0;
}

const wchar_t* StandardNPC::getString(DWORD tag) const
{
    switch (tag) {
    case 0x6E616D65 : return m_name.m_buffer;
    case 0x77656C63 : return m_welcome.m_buffer;
    case 0x66616D69 : return typeName(m_type);
    case 0x71737469 : return m_questsInText.m_buffer;
    case 0x7173746F : return m_questsOutText.m_buffer;
    }
    return 0;
}

int StandardNPC::getInt(DWORD tag) const
{
    switch (tag) {
    case 0x73687065 : return m_shape;
    case 0x6B716F75 : return m_questsOut.m_count;
    case 0x6B71696E : return m_questsIn.m_count;
    case 0x636E7472 : return m_country;
    case 0x66616D69 : return m_type;
    case 0x756E6B31 : return m_unk1;
    case 0x74797065 : return m_type;
    case 0x75696420 : return m_id;
    case 0x756E6B32 : return m_unk2;
    }
    return 0;
}

bool StandardNPC::setString(DWORD tag, const wchar_t* text)
{
    if (tag == 0x6E616D65 ) {
        m_name = text;
        return true;
    }
    if (tag == 0x77656C63 ) {
        m_welcome = text;
        return true;
    }
    return false;
}

bool StandardNPC::setInt(DWORD tag, int value)
{
    switch (tag) {
    case 0x74797065 : m_type = value; return true;
    case 0x75696420 : m_id = (int16_t)value; return true;
    case 0x73687065 : m_shape = value; return true;
    case 0x636E7472 : m_country = value; return true;
    default: return false;
    }
}

int StandardNPC::compare(const Node* otherNode, DWORD tag) const
{
    const StandardNPC* other = &dynamic_cast<const StandardNPC&>(*otherNode);
    switch (tag) {
    case 0x74797065 :
    case 0x66616D69 :
        return w2_cmpS(m_type, other->m_type);
    case 0x75696420 :
        return w2_cmpS(m_id, other->m_id);
    case 0x6E616D65 :
        return String::compareLogical(m_name, other->m_name);
    case 0x73687065 :
        return w2_cmpS(m_shape, other->m_shape);
    case 0x636E7472 :
        return w2_cmpS(m_country, other->m_country);
    case 0x6B71696E :
        return w2_cmpS(m_questsIn.m_count, other->m_questsIn.m_count);
    case 0x6B716F75 :
        return w2_cmpS(m_questsOut.m_count, other->m_questsOut.m_count);
    default:
        return 0;
    }
}

void StandardNPC::read(iStream& s, DWORD tag)
{
    this->readMain(s, tag);
    this->readLists(s, tag);
}

void StandardNPC::write(oStream& s, DWORD tag) const
{
    this->writeMain(s, tag);
    this->writeLists(s, tag);
}

bool StandardNPC::toText(String& out, DWORD format) const
{
    char sepc;
    if (format <= 0x64756D70 ) {
        if (format == 0x64756D70  || format == 0x2D2D2D2D  || format == 0x63737620 )
            sepc = 59;
        else
            sepc = 44;
    } else if (format == 0x73716C20  || format != 0x74616273 ) {
        sepc = 44;
    } else {
        sepc = 9;
    }
    wchar_t sep = (wchar_t)(short)sepc;
    out.append(m_name.m_buffer, m_name.length());
    out << L"\r\n";
    out.append(m_welcome.m_buffer, m_welcome.length());
    out << L"\r\n";
    out << (unsigned long)m_type;
    out << (wchar_t)sep;
    out << (short)m_id;
    out << (wchar_t)sep;
    out << (long)m_shape;
    out << (wchar_t)sep;
    out << (long)m_unk1;
    out << (wchar_t)sep;
    out << (long)m_unk2;
    out << (wchar_t)sep;
    out << (long)m_country;
    return true;
}

void StandardNPC::assign(const StandardNPC* src)
{
    m_shape = src->m_shape;
    m_unk1 = src->m_unk1;
    m_unk2 = src->m_unk2;
    m_country = src->m_country;
    if (&src->m_name != &m_name)
        m_name = src->m_name;
    if (&src->m_welcome != &m_welcome)
        m_welcome = src->m_welcome;
    primitiveVectorAssign(m_questsIn, src->m_questsIn);
    primitiveVectorAssign(m_questsOut, src->m_questsOut);
    if (&src->m_questsInText != &m_questsInText)
        m_questsInText = src->m_questsInText;
    if (&src->m_questsOutText != &m_questsOutText)
        m_questsOutText = src->m_questsOutText;
}

void StandardNPC::readMain(iStream& s, int tag)
{
    switch (tag) {
    case 0x66696C65 : {
        uint8_t type = s.readByte();
        m_id = s.readShort();
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

void StandardNPC::writeMain(oStream& s, int tag) const
{
    switch (tag) {
    case 0x66696C65 : {
        String welc = m_welcome.escape();
        String name = m_name.escape();
        s << (unsigned long)m_type << (unsigned short)m_id << (unsigned long)m_shape
          << (unsigned long)m_unk1 << (unsigned long)m_unk2 << (unsigned long)m_country;
        name.write(s, 4, static_cast<aStream&>(s).m_codePage);
        welc.write(s, 4, static_cast<aStream&>(s).m_codePage);
        return;
    }
    case 0x666C6E74 :
        s << (unsigned long)m_type << (unsigned short)m_id << (unsigned long)m_shape
          << (unsigned long)m_unk1 << (unsigned long)m_unk2 << (unsigned long)m_country;
        return;
    case 0x6C747874 : {
        String welc = m_welcome.escape();
        String name = m_name.escape();
        name.write(s, 4, static_cast<aStream&>(s).m_codePage);
        welc.write(s, 4, static_cast<aStream&>(s).m_codePage);
        return;
    }
    }
}

void StandardNPC::readLists(iStream& s, int tag)
{
    if (tag == 0x6C747874 )
        return;
    int n = s.readInt();
    if ((unsigned int)n > 0x3E7)
        throw -1;
    for (int k = n; k > 0; --k)
        m_questsIn.add((short)(unsigned short)s.readShort());
    int m = s.readInt();
    if (m < 0)
        throw -1;
    for (int k = m; k > 0; --k)
        m_questsOut.add((short)(unsigned short)s.readShort());
    this->buildListStrings();
}

void StandardNPC::writeLists(oStream& s, int tag) const
{
    if (tag == 0x6C747874 )
        return;
    int n = m_questsIn.m_count;
    s << (unsigned long)n;
    for (int i = 0; i < n; ++i) {
        if (i < 0 || i >= m_questsIn.m_capacity)
            w2_throwIndexOutOfBounds(i);
        s << (unsigned short)m_questsIn.m_items[i];
    }
    int m = m_questsOut.m_count;
    s << (unsigned long)m;
    for (int i = 0; i < m; ++i) {
        if (i < 0 || i >= m_questsOut.m_capacity)
            w2_throwIndexOutOfBounds(i);
        s << (unsigned short)m_questsOut.m_items[i];
    }
}

void StandardNPC::buildListStrings()
{
    m_questsInText.clear();
    m_questsOutText.clear();

    int last = m_questsIn.m_count - 1;
    for (int i = 0; i < last; ++i) {
        unsigned short v = (i < 0 || i >= m_questsIn.m_capacity) ? 0 : (unsigned short)m_questsIn.m_items[i];
        m_questsInText << (short)v;
        m_questsInText << (wchar_t)L',';
    }
    {
        unsigned short v = (last < 0 || last >= m_questsIn.m_capacity) ? 0 : (unsigned short)m_questsIn.m_items[last];
        m_questsInText << (short)v;
    }

    int last2 = m_questsOut.m_count - 1;
    for (int i = 0; i < last2; ++i) {
        short v = (i < 0 || i >= m_questsOut.m_capacity) ? 0 : m_questsOut.m_items[i];
        m_questsOutText << v;
        m_questsOutText << (wchar_t)L',';
    }
    {
        short v = (last2 < 0 || last2 >= m_questsOut.m_capacity) ? 0 : m_questsOut.m_items[last2];
        m_questsOutText << v;
    }
}

void StandardNPC::clearLists()
{
    for (int i = 0; i < m_questsIn.m_capacity; ++i)
        m_questsIn.resetItem(&m_questsIn.m_items[i]);
    m_questsIn.m_count = 0;
    for (int i = 0; i < m_questsOut.m_capacity; ++i)
        m_questsOut.resetItem(&m_questsOut.m_items[i]);
    m_questsOut.m_count = 0;
    m_questsInText.clear();
    m_questsOutText.clear();
}

void StandardNPC::trimStrings()
{
    m_name.stripCrLf();
    m_name.stripRight(L' ');
    m_name.stripCrLf();
    m_welcome.stripCrLf();
    m_welcome.stripRight(L' ');
    m_welcome.stripCrLf();
}
