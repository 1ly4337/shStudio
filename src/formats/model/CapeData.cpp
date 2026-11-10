#include "CapeData.h"
#include <cstdarg>

wchar_t g_capeBlockName[16];
wchar_t g_capeDataName[16];

int sub_43CBF0(wchar_t* buffer, const wchar_t* format, ...)
{
    va_list args;
    va_start(args, format);
    int r = vswprintf_s(buffer, 16, format, args);
    va_end(args);
    return r;
}

static wchar_t p02_separator(DWORD fmt)
{
    if ((int)fmt <= 'dump')
    {
        if (fmt == 'dump' || fmt == '----' || fmt == 'csv ')
            return L';';
        return L',';
    }
    if (fmt == 'sql ' || fmt != 'tabs')
        return L',';
    return L'\t';
}

void CapeData::Block::reset()
{
    m_points[2] = 0;
    m_points[1] = 0;
    m_points[0] = 0;
    m_values[2] = 0.0f;
    m_values[1] = 0.0f;
    m_values[0] = 0.0f;
}

void CapeData::Block::read(iStream& in, DWORD )
{
    m_points[0] = in.readInt();
    m_points[1] = in.readInt();
    m_points[2] = in.readInt();
    m_values[0] = in.readFloat();
    m_values[1] = in.readFloat();
    m_values[2] = in.readFloat();
    if (fabs(m_values[0]) < 0.00001)
        m_values[0] = 0.0f;
    if (fabs(m_values[1]) < 0.00001)
        m_values[1] = 0.0f;
    if (fabs(m_values[2]) < 0.00001)
        m_values[2] = 0.0f;
}

void CapeData::Block::write(oStream& out, DWORD ) const
{
    out << (unsigned long)m_points[0] << (unsigned long)m_points[1] << (unsigned long)m_points[2];
    out << m_values[0] << m_values[1] << m_values[2];
}

bool CapeData::Block::toText(String& out, DWORD fmt) const
{
    wchar_t sep = p02_separator(fmt);
    if (fmt != 'dump')
        out << sep;
    out << (long)m_points[0] << sep << (long)m_points[1] << sep << (long)m_points[2]
        << sep << m_values[0] << sep << m_values[1] << sep << m_values[2];
    if (fmt == 'dump')
        out << L"\r\n";
    return true;
}

short CapeData::Block::getImage(int state, short def) const
{
    if ((unsigned)(state - 4) > 2)
        return def;
    return 130;
}

const wchar_t* CapeData::Block::getString(DWORD tag) const
{
    if (tag != 'name')
        return 0;
    memset(g_capeBlockName, 0, 16);
    sub_43CBF0(g_capeBlockName, L"#%d", m_index);
    return g_capeBlockName;
}

int CapeData::Block::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 0: return m_index;
    case 1: return m_points[0];
    case 2: return m_points[1];
    case 3: return m_points[2];
    case 4: return *(const int*)&m_values[0];
    case 5: return *(const int*)&m_values[1];
    case 6: return *(const int*)&m_values[2];
    }
    return 0;
}

int CapeData::Block::compare(const Node* , DWORD ) const
{
    return 0;
}

CapeData::CapeData(int index, int )
{
    CapeData::reset();
    m_index = index;
}

CapeData::~CapeData()
{
}

void CapeData::reset()
{
    m_unk3 = 0;
    m_unk2 = 0;
    m_unk1C = 0;
    m_unk1 = 0;
    m_3dc = 0;
    m_dds = 0;
    m_unk4[0] = 0;
    m_unk4[1] = 0;
    m_unk4[2] = 0;
    m_unk4[3] = 0;
    m_unk5 = 0;
    for (int i = 0; i < 20; ++i)
    {
        m_blocks[i].reset();
        m_blocks[i].m_index = i + 1;
    }
}

void CapeData::read(iStream& in, DWORD fmt)
{
    m_3dc = in.readInt();
    m_dds = in.readInt();
    m_unk1 = in.readInt();
    m_unk1C = in.readInt();
    m_unk2 = in.readInt();
    m_unk3 = in.readInt();
    for (int k = 0; k < 4; ++k)
    {
        int v = in.peekInt();
        in.skip(4);
        m_unk4[k] = v;
    }
    int v = in.peekInt();
    in.skip(4);
    m_unk5 = v;
    for (int i = 0; i < 20; ++i)
    {
        m_blocks[i].m_index = i + 1;
        m_blocks[i].read(in, fmt);
    }
}

void CapeData::write(oStream& out, DWORD fmt) const
{
    out << (unsigned long)m_dds << (unsigned long)m_3dc << (unsigned long)m_unk1 << (unsigned long)m_unk1C
        << (unsigned long)m_unk2 << (unsigned long)m_unk3;
    for (int k = 0; k < 4; ++k)
        out << (unsigned long)m_unk4[k];
    out << (unsigned long)m_unk5;
    for (int i = 0; i < 20; ++i)
        m_blocks[i].write(out, fmt);
}

bool CapeData::toText(String& out, DWORD fmt) const
{
    if ((fmt & 0x73716C00) == 0x73716C00)
        return false;
    wchar_t sep = p02_separator(fmt);
    out << (long)m_dds << sep << (long)m_3dc << sep << (long)m_unk1 << sep << (long)m_unk1C
        << sep << (long)m_unk2 << sep << (long)m_unk3;
    for (int k = 0; k < 4; ++k)
        out << sep << (long)m_unk4[k];
    out.m_radix = 16;
    out << sep << (long)m_unk5;
    out.m_radix = 10;
    if (fmt == 'dump')
        out << L"\r\n";
    for (int i = 0; i < 20; ++i)
        m_blocks[i].toText(out, fmt);
    out.newline();
    return true;
}

const wchar_t* CapeData::getString(DWORD tag) const
{
    if (tag != 'name')
        return 0;
    memset(g_capeDataName, 0, 16);
    sub_43CBF0(g_capeDataName, L"#%d", m_index);
    return g_capeDataName;
}

int CapeData::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 'indx': return m_index;
    case 'bDDS': return m_dds;
    case 'b3DC': return m_3dc;
    case 'unk1': return m_unk1;
    case 'unk2': return m_unk2;
    case 'unk3': return m_unk3;
    case 'uk41': return m_unk4[0];
    case 'uk42': return m_unk4[1];
    case 'uk43': return m_unk4[2];
    case 'uk44': return m_unk4[3];
    case 'unk5': return m_unk5;
    }
    return 0;
}

int CapeData::compare(const Node* , DWORD ) const
{
    return 0;
}
