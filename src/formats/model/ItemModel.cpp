#include "ItemModel.h"
#include "Spot.h"

ItemModel::ItemModel(int id, DWORD version, int )
{
    m_id = id;
    m_version = version;
    ItemModel::reset();
}

ItemModel::~ItemModel()
{
}

bool ItemModel::copyFrom(const Streamable* src)
{
    const ItemModel* p = dynamic_cast<const ItemModel*>(src);
    if (!p)
        return false;
    assign(p);
    return true;
}

void ItemModel::reset()
{
    m_vertexIndex = 0;
    m_textureIndex = 0;
    m_glow = -1;
    m_format = 0;
    m_unk2 = 0;
    m_unk3 = 0;
    m_rgba = 0xFFFFFFFF;
    m_rotY = 0.0f;
    m_scale = 1.0f;
    m_unk5 = 0;
    m_texture.clear();
    m_vertex.clear();
}

void ItemModel::assign(const ItemModel* src)
{
    m_id = src->m_id;
    m_version = src->m_version;
    m_vertexIndex = src->m_vertexIndex;
    m_textureIndex = src->m_textureIndex;
    m_glow = src->m_glow;
    m_unk2 = src->m_unk2;
    m_format = src->m_format;
    m_unk3 = src->m_unk3;
    m_rgba = src->m_rgba;
    m_rotY = src->m_rotY;
    m_scale = src->m_scale;
    m_unk5 = src->m_unk5;
    m_texture = src->m_texture;
    m_vertex = src->m_vertex;
}

const wchar_t* ItemModel::getString(DWORD tag) const
{
    if (tag == 'text')
        return m_texture.m_buffer;
    if (tag == 'vert')
        return m_vertex.m_buffer;
    return 0;
}

int ItemModel::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 'indx': return m_id;
    case 'glow': return m_glow;
    case 'unk2': return m_unk2;
    case 'frmt': return m_format;
    case 'unk3': return m_unk3;
    case 'rgba': return (int)m_rgba;
    case 'RotY': return *(const int*)&m_rotY;
    case 'scal': return *(const int*)&m_scale;
    case 'unk5': return m_unk5;
    }
    return 0;
}

bool ItemModel::setString(DWORD tag, const wchar_t* value)
{
    if (tag == 'text')
        m_texture = value;
    else if (tag == 'vert')
        m_vertex = value;
    return false;
}

bool ItemModel::setInt(DWORD tag, int value)
{
    switch (tag)
    {
    case 'indx': m_id = value; return true;
    case 'glow': m_glow = (value == 255) ? -1 : value; return true;
    case 'unk2': m_unk2 = value; return true;
    case 'frmt': m_format = value; return true;
    case 'unk3': m_unk3 = value; return true;
    case 'rgba': m_rgba = (DWORD)value; return true;
    case 'RotY': *(int*)&m_rotY = value; return true;
    case 'scal': *(int*)&m_scale = value; return true;
    case 'unk5': m_unk5 = value; return true;
    default: return false;
    }
}

void ItemModel::read(iStream& in, DWORD fmt)
{
    int first = in.readInt();
    if (fmt == 'clip')
    {
        m_id = first;
        m_texture.read(in, 4, in.m_codePage);
        m_vertex.read(in, 4, in.m_codePage);
        m_glow = in.readInt();
        m_format = in.readInt();
        m_rgba = (DWORD)in.readInt();
        m_unk2 = in.readInt();
        m_unk3 = in.readInt();
        m_rotY = in.readFloat();
        m_scale = in.readFloat();
        m_unk5 = in.readInt();
        m_textureIndex = -1;
        m_vertexIndex = -1;
        return;
    }
    m_vertexIndex = first;
    m_textureIndex = in.readInt();
    m_glow = in.readInt();
    m_unk2 = in.readInt();
    m_format = in.readInt();
    if (m_format >= 0)
        m_unk3 = in.readInt();
    if (m_format >= 1)
    {
        m_rgba = (DWORD)in.readInt();
        m_rotY = in.readFloat();
        m_scale = in.readFloat();
        m_unk5 = in.readInt();
    }
    if (m_version == 'IT2 ')
        in.skip(1024);
}

void ItemModel::write(oStream& out, DWORD fmt) const
{
    if (fmt == 'clip')
    {
        out << (unsigned long)m_id;
        m_texture.write(out, 4, out.m_codePage);
        m_vertex.write(out, 4, out.m_codePage);
        out << (unsigned long)m_glow << (unsigned long)m_format << (unsigned long)m_rgba
            << (unsigned long)m_unk2 << (unsigned long)m_unk3;
        out << m_rotY << m_scale << (unsigned long)m_unk5;
        return;
    }
    out << (unsigned long)m_vertexIndex << (unsigned long)m_textureIndex << (unsigned long)m_glow
        << (unsigned long)m_unk2 << (unsigned long)m_format;
    if (m_format >= 0)
        out << (unsigned long)m_unk3;
    if (m_format >= 1)
    {
        out << (unsigned long)m_rgba;
        out << m_rotY << m_scale << (unsigned long)m_unk5;
    }
}

static int p02_cmpInt(int a, int b)
{
    if (a != b)
        return 2 * (a >= b) - 1;
    return 0;
}

int ItemModel::compare(const Node* other, DWORD tag) const
{
    const ItemModel* p = dynamic_cast<const ItemModel*>(other);
    switch (tag)
    {
    case 'indx': return p02_cmpInt(m_id, p->m_id);
    case 'text': return String::compareLogical(m_texture, p->m_texture);
    case 'vert': return String::compareLogical(m_vertex, p->m_vertex);
    case 'glow': return p02_cmpInt(m_glow, p->m_glow);
    case 'unk2': return p02_cmpInt(m_unk2, p->m_unk2);
    case 'unk3': return p02_cmpInt(m_unk3, p->m_unk3);
    case 'rgba':
        if (m_rgba != p->m_rgba)
            return m_rgba < p->m_rgba ? -1 : 1;
        return 0;
    case 'RotY': return compareFloat(m_rotY, p->m_rotY);
    case 'scal': return compareFloat(m_scale, p->m_scale);
    case 'unk5': return p02_cmpInt(m_unk5, p->m_unk5);
    }
    return 0;
}

bool ItemModel::toText(String& out, DWORD fmt) const
{
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
    if (fmt != '----')
        return false;
    out << (long)m_id << sep << (long)m_glow << sep << L'"' << m_texture << L'"'
        << sep << L'"' << m_vertex << L'"';
    return true;
}
