#include "Face.h"
#include "String.hpp"
#include "iStream.hpp"
#include "oStream.hpp"
#include "_imports_w3.h"
#include <cstdio>

wchar_t g_faceText[64];

Face::Face(short index)
{
    m_index = (unsigned short)index;
    m_flags = 0;
}

int Face::nodeTag()
{
    return 0x66616365;
}

short Face::getImage(int tag, short def) const
{
    if (tag == 0x6E616D65  || tag == 0x696E6478 )
        return 0;
    if (tag == 1 || tag == 0x696E6431  || tag == 2 || tag == 0x696E6432  ||
        tag == 3 || tag == 0x696E6433 )
        return 16;
    return def;
}

const wchar_t* Face::getString(DWORD tag) const
{
    if (tag != 0x6E616D65  && tag != 0x696E6478 )
        return 0;
    swprintf_s(g_faceText, 64, L"# %d", m_index);
    return g_faceText;
}

int Face::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 1:
    case 0x696E6431:
        return m_v1;
    case 2:
    case 0x696E6432:
        return m_v2;
    case 3:
    case 0x696E6433:
        return m_v3;
    }
    return 0;
}

bool Face::setInt(DWORD tag, int value)
{
    switch (tag)
    {
    case 1:
    case 0x696E6431:
        m_v1 = (unsigned short)value;
        return true;
    case 2:
    case 0x696E6432:
        m_v2 = (unsigned short)value;
        return true;
    case 3:
    case 0x696E6433:
        m_v3 = (unsigned short)value;
        return true;
    default:
        return false;
    }
}

void Face::reset()
{
    m_v2 = 0xFFFF;
    m_v3 = 0xFFFF;
    m_v1 = 0xFFFF;
    m_flags = 0;
}

int Face::compare(const Node* other, DWORD tag) const
{
    const Face* f = dynamic_cast<const Face*>(other);
    if (f)
        return compare(f, tag);
    return 1;
}

int Face::compare(const Face* other, DWORD tag) const
{
    unsigned short a, b;
    switch (tag)
    {
    case 1:
    case 0x696E6431:
        a = m_v1; b = other->m_v1;
        break;
    case 2:
    case 0x696E6432:
        a = m_v2; b = other->m_v2;
        break;
    case 3:
    case 0x696E6433:
        a = m_v3; b = other->m_v3;
        break;
    default:
        a = m_index; b = other->m_index;
        break;
    }
    if (a == b)
        return 0;
    return a < b ? -1 : 1;
}

void Face::read(iStream& s, DWORD )
{
    m_v1 = s.readShort();
    m_v2 = s.readShort();
    m_v3 = s.readShort();
}

void Face::write(oStream& s, DWORD ) const
{
    s << (short)m_v1 << (short)m_v2 << (short)m_v3;
}

bool Face::copyFrom(const Streamable* src)
{
    const Face* f = dynamic_cast<const Face*>(src);
    if (!f)
        return false;
    m_index = f->m_index;
    m_v1 = f->m_v1;
    m_v2 = f->m_v2;
    m_v3 = f->m_v3;
    m_flags = f->m_flags;
    return true;
}

bool Face::toText(String& out, DWORD fmt) const
{
    if (fmt == 0x74616273 )
    {
        out << L'\t';
        String& a = out << (unsigned short)m_v1;
        a << L'\t';
        String& b = a << (unsigned short)m_v2;
        b << L'\t';
        b << (unsigned short)m_v3 << L"\r\n";
        return true;
    }
    if (fmt == 0x636F6465 )
    {
        out << L"{ " << (unsigned short)m_v1 << L", " << (unsigned short)m_v2
            << L", " << (unsigned short)m_v3 << L" }";
    }
    else
    {
        out << (unsigned short)m_v1 << L", " << (unsigned short)m_v2 << L", " << (unsigned short)m_v3;
    }
    return true;
}
