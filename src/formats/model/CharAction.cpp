#include "CharAction.h"
#include "Spot.h"

CharAction::CharAction(int id, int )
{
    m_id = id;
    m_name.clear();
    m_mode = m_byte1 = m_byte2 = m_byte3 = 0;
    m_unk1 = 0;
    m_floats[3] = 0.0f;
    m_floats[2] = 0.0f;
    m_floats[1] = 0.0f;
    m_floats[0] = 0.0f;
}

CharAction::~CharAction()
{
}

bool CharAction::copyFrom(const Streamable* src)
{
    const CharAction* p = dynamic_cast<const CharAction*>(src);
    if (!p)
        return false;
    assign(*p);
    return true;
}

void CharAction::assign(const CharAction& src)
{
    m_name = src.m_name;
    m_mode = src.m_mode;
    m_byte1 = src.m_byte1;
    m_byte2 = src.m_byte2;
    m_byte3 = src.m_byte3;
    m_unk1 = src.m_unk1;
    for (int k = 0; k < 4; ++k)
        m_floats[k] = src.m_floats[k];
}

const wchar_t* CharAction::getString(DWORD tag) const
{
    if (tag == 'name')
        return m_name.m_buffer;
    return 0;
}

int CharAction::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 'indx': return m_id;
    case 'mode': return m_mode;
    case 'byt1': return m_byte1;
    case 'byt2': return m_byte2;
    case 'byt3': return m_byte3;
    case 'unk1': return m_unk1;
    case 'flt1': return *(const int*)&m_floats[0];
    case 'flt2': return *(const int*)&m_floats[1];
    case 'flt3': return *(const int*)&m_floats[2];
    case 'flt4': return *(const int*)&m_floats[3];
    }
    return 0;
}

bool CharAction::setInt(DWORD tag, int value)
{
    switch (tag)
    {
    case 'mode': m_mode = (BYTE)value; return true;
    case 'byt1': m_byte1 = (BYTE)value; return true;
    case 'byt2': m_byte2 = (BYTE)value; return true;
    case 'byt3': m_byte3 = (BYTE)value; return true;
    case 'unk1': m_unk1 = value; return true;
    case 'flt1': *(int*)&m_floats[0] = value; return true;
    case 'flt2': *(int*)&m_floats[1] = value; return true;
    case 'flt3': *(int*)&m_floats[2] = value; return true;
    case 'flt4': *(int*)&m_floats[3] = value; return true;
    default: return false;
    }
}

bool CharAction::setString(DWORD tag, const wchar_t* value)
{
    if (tag != 'name')
        return false;
    m_name = value;
    return true;
}

void CharAction::reset()
{
    m_name.clear();
    m_mode = m_byte1 = m_byte2 = m_byte3 = 0;
    m_unk1 = 0;
    m_floats[3] = 0.0f;
    m_floats[2] = 0.0f;
    m_floats[1] = 0.0f;
    m_floats[0] = 0.0f;
}

static int p02_cmpInt(int a, int b)
{
    if (a != b)
        return 2 * (a >= b) - 1;
    return 0;
}

static int p02_cmpByte(BYTE a, BYTE b)
{
    if (a != b)
        return a < b ? -1 : 1;
    return 0;
}

int CharAction::compare(const Node* other, DWORD tag) const
{
    const CharAction* p = dynamic_cast<const CharAction*>(other);
    switch (tag)
    {
    case 'indx': return p02_cmpInt(m_id, p->m_id);
    case 'name': return String::compareLogical(m_name, p->m_name);
    case 'mode': return p02_cmpByte(m_mode, p->m_mode);
    case 'byt1': return p02_cmpByte(m_byte1, p->m_byte1);
    case 'byt2': return p02_cmpByte(m_byte2, p->m_byte2);
    case 'byt3': return p02_cmpByte(m_byte3, p->m_byte3);
    case 'unk1': return p02_cmpInt(m_unk1, p->m_unk1);
    case 'flt1': return compareFloat(m_floats[0], p->m_floats[0]);
    case 'flt2': return compareFloat(m_floats[1], p->m_floats[1]);
    case 'flt3': return compareFloat(m_floats[2], p->m_floats[2]);
    case 'flt4': return compareFloat(m_floats[3], p->m_floats[3]);
    }
    return 0;
}

void CharAction::read(iStream& in, DWORD fmt)
{
    if (fmt != 'file')
        m_id = in.readInt();
    m_name.read(in, 4, (UINT)-1);
    if (m_name.m_length > 0)
    {
        m_mode = in.readByte();
        m_byte1 = in.readByte();
        m_byte2 = in.readByte();
        m_byte3 = in.readByte();
        m_unk1 = in.readInt();
        m_floats[0] = in.readFloat();
        m_floats[1] = in.readFloat();
        m_floats[2] = in.readFloat();
        m_floats[3] = in.readFloat();
    }
}

void CharAction::write(oStream& out, DWORD fmt) const
{
    if (fmt != 'file')
        out << (unsigned long)m_id;
    if (!m_name.m_length)
    {
        out << (unsigned long)0;
        return;
    }
    m_name.write(out, 4, (UINT)-1);
    out << (char)m_mode << (char)m_byte1 << (char)m_byte2 << (char)m_byte3;
    out << (unsigned long)m_unk1;
    out << m_floats[0] << m_floats[1] << m_floats[2] << m_floats[3];
}

bool CharAction::toText(String& out, DWORD fmt) const
{
    if (!m_name.m_length)
    {
        out << L"empty" << L"\r\n";
        return true;
    }
    wchar_t sep;
    if ((int)fmt <= 'dump')
    {
        if (fmt == 'dump' || fmt == '----' || fmt == 'csv ')
            sep = L';';
        else
            sep = L',';
    }
    else if (fmt == 'sql ' || fmt != 'tabs')
        sep = L',';
    else
        sep = L'\t';
    out << (long)m_id << sep << m_name << sep << (unsigned char)m_mode << sep << (unsigned char)m_byte1
        << sep << (unsigned char)m_byte2 << sep << (unsigned char)m_byte3 << sep << (long)m_unk1;
    for (int k = 0; k < 4; ++k)
        out << sep << m_floats[k];
    out << L"\r\n";
    return true;
}
