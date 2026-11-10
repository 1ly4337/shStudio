#include "ModelMLT.h"

ModelMLT::ModelMLT(int id, const wchar_t* texture, const wchar_t* vertex, int alpha, int )
{
    m_id = id;
    m_alpha = alpha;
    m_vrs3 = 0;
    m_texture = texture;
    m_vertex = vertex;
}

ModelMLT::ModelMLT(const ModelMLT& src, int )
{
    m_id = src.m_id;
    m_alpha = src.m_alpha;
    m_vrs3 = src.m_vrs3;
    if (&src.m_texture != &m_texture)
    {
        m_texture.clear();
        m_texture.append(src.m_texture.m_buffer, src.m_texture.m_length);
    }
    if (&src.m_vertex != &m_vertex)
    {
        m_vertex.clear();
        m_vertex.append(src.m_vertex.m_buffer, src.m_vertex.m_length);
    }
}

ModelMLT::~ModelMLT()
{
}

bool ModelMLT::copyFrom(const Streamable* src)
{
    const ModelMLT* p = dynamic_cast<const ModelMLT*>(src);
    if (!p)
        return false;
    assignModel(p);
    return true;
}

void ModelMLT::reset()
{
    m_id = 0;
    m_alpha = 1;
    m_texture.clear();
    m_vertex.clear();
}

void ModelMLT::assignModel(const ModelMLT* src)
{
    m_alpha = src->m_alpha;
    if (&src->m_texture != &m_texture)
    {
        m_texture.clear();
        m_texture.append(src->m_texture.m_buffer, src->m_texture.m_length);
    }
    if (&src->m_vertex != &m_vertex)
    {
        m_vertex.clear();
        m_vertex.append(src->m_vertex.m_buffer, src->m_vertex.m_length);
    }
}

const wchar_t* ModelMLT::getString(DWORD tag) const
{
    if (tag == 'text')
        return m_texture.m_buffer;
    if (tag == 'vert')
        return m_vertex.m_buffer;
    return 0;
}

int ModelMLT::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 'indx': return m_id;
    case 'alph': return m_alpha;
    case 'vrs3': return m_vrs3;
    }
    return 0;
}

bool ModelMLT::setString(DWORD tag, const wchar_t* value)
{
    if (tag == 'text')
    {
        m_texture = value;
        return true;
    }
    if (tag == 'vert')
    {
        m_vertex = value;
        return true;
    }
    return false;
}

bool ModelMLT::setInt(DWORD tag, int value)
{
    switch (tag)
    {
    case 'indx': m_id = value; return true;
    case 'alph': m_alpha = value; return true;
    case 'vrs3': m_vrs3 = value; return true;
    default: return false;
    }
}

int ModelMLT::compare(const Node* other, DWORD tag) const
{
    const ModelMLT* p = dynamic_cast<const ModelMLT*>(other);
    switch (tag)
    {
    case 'indx':
        if (m_id != p->m_id) return 2 * (m_id >= p->m_id) - 1;
        return 0;
    case 'alph':
        if (m_alpha != p->m_alpha) return 2 * (m_alpha >= p->m_alpha) - 1;
        return 0;
    case 'vrs3':
        if (m_vrs3 != p->m_vrs3) return 2 * (m_vrs3 >= p->m_vrs3) - 1;
        return 0;
    case 'text':
        return String::compareLogical(m_texture, p->m_texture);
    case 'vert':
        return String::compareLogical(m_vertex, p->m_vertex);
    default:
        return 0;
    }
}

bool ModelMLT::toText(String& out, DWORD fmt) const
{
    char sep;
    if ((int)fmt <= 'dump')
    {
        if (fmt == 'dump' || fmt == '----' || fmt == 'csv ')
            sep = ';';
        else
            sep = ',';
    }
    else if (fmt == 'sql ')
        sep = ',';
    else if (fmt == 'tabs')
        sep = '\t';
    else
        sep = ',';
    String& s = out << (long)m_id << sep << (long)m_alpha << sep << L'"';
    String& t = s.append(m_texture.m_buffer, m_texture.m_length) << L'"' << sep << L'"';
    t.append(m_vertex.m_buffer, m_vertex.m_length) << L'"' << L"\r\n";
    return true;
}
