#include "Name.h"
#include "iStream.hpp"
#include "oStream.hpp"
#include "_imports_w0.h"
#include <shlwapi.h>
#include <string.h>

Name::Name(int id, int owner)
{
    m_id = id;
    m_owner = owner;
}

Name::~Name()
{
}

int Name::getId() const { return 0; }
int Name::displayV2() const { return 0; }

void Name::clearAll()
{
    reset();
}

bool Name::isEmpty()
{
    return m_file.m_length == 0;
}

int Name::compareById(Node* other)
{
    return compare(other, 0x69645F5F );
}

bool readFixedString256(iStream& s, String& dst, UINT codePage)
{
    dst.clear();
    if (s.m_size - s.m_pos < 256)
        return false;
    char* mb = (char*)s.readAlloc(256);
    int len = (int)strlen(mb);
    dst.clear();
    dst.append(mb, len, codePage);
    return true;
}

void writeFixedString256(oStream& s, const String& src, UINT codePage)
{
    struct
    {
        unsigned int len;
        CHAR text[256];
    } buf;
    memset(buf.text, 0, sizeof(buf.text));
    BOOL usedDefaultChar = FALSE;
    WideCharToMultiByte(codePage, 0, src.m_buffer, -1, buf.text, 256, "?", &usedDefaultChar);
    buf.len = (unsigned int)strlen(buf.text);
    s.put(&buf.len, 0x100);
    s.put(buf.text, buf.len);
}

void Name::read(iStream& s, DWORD tag)
{
    if (!readFixedString256(s, m_file, s.m_codePage))
        throw 0;
    if (&m_file != &m_name)
        m_name.append(m_file.m_buffer, m_file.m_length);
}

void Name::write(oStream& s, DWORD tag) const
{
    writeFixedString256(s, m_file, s.m_codePage);
}

bool Name::toText(String& out, DWORD tag) const
{
    (out << (long)(m_id));
    if (tag == 0x64756D70 )
    {
        (out << (char)(':'));
        (out << (char)('\t'));
    }
    else
    {
        (out << (char)(sub_41A900(tag)));
    }
    out.append(m_file.m_buffer, m_file.m_length);
    return true;
}

void Name::reset()
{
    m_file.clear();
    m_name.clear();
}

bool Name::setString(DWORD tag, const wchar_t* text)
{
    if (tag != 0x6E616D65 )
        return false;
    m_file = text;
    m_name = text;
    return true;
}

bool Name::setInt(DWORD tag, int value)
{
    if (tag != 0x66696C65 )
        return false;
    if (&m_file != &m_name)
    {
        const wchar_t* buf = m_file.m_buffer;
        int len = m_file.m_length;
        m_name.clear();
        m_name.append(buf, len);
    }
    if (!value)
        (m_name << (const wchar_t*)(L" (missing file)"));
    return true;
}

const wchar_t* Name::getString(DWORD tag) const
{
    if (tag == 0x6E616D65 )
        return m_name.m_buffer;
    return 0;
}

int Name::getInt(DWORD tag) const
{
    if (tag == 0x31636F6C )
        return 1;
    if (tag == 0x69645F5F )
        return m_id;
    return 0;
}

int Name::compare(const Node* node, DWORD tag) const
{
    const Name* other = dynamic_cast<const Name*>(node);
    if (tag == 0x69645F5F )
    {
        int b = other->m_id;
        int a = m_id;
        if (a != b)
            return a >= b ? 1 : -1;
        return 0;
    }
    if (tag == 0x6E616D65 )
    {
        const wchar_t* a = m_file.m_buffer;
        const wchar_t* b = other->m_file.m_buffer;
        if (a)
        {
            if (b)
                return StrCmpLogicalW(a, b);
            return 1;
        }
        return -(b != 0);
    }
    return 0;
}
