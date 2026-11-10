#include "ModelMLX.h"

ModelMLX::ModelMLX(int )
{
    m_id = 0;
    for (int i = 0; i < 4; ++i)
        m_alpha[i] = 1;
}

ModelMLX::~ModelMLX()
{
}

bool ModelMLX::copyFrom(const Streamable* src)
{
    const ModelMLX* p = dynamic_cast<const ModelMLX*>(src);
    if (!p)
        return false;
    assign(p);
    return true;
}

void ModelMLX::assign(const ModelMLX* src)
{
    m_id = src->m_id;
    m_name = src->m_name;
    for (int i = 0; i < 4; ++i)
    {
        m_textures[i] = src->m_textures[i];
        m_vertices[i] = src->m_vertices[i];
        m_alpha[i] = src->m_alpha[i];
    }
}

const wchar_t* ModelMLX::getString(DWORD tag) const
{
    switch (tag)
    {
    case 'name': return m_name.m_buffer;
    case 'txUp': return m_textures[0].m_buffer;
    case 'txLo': return m_textures[1].m_buffer;
    case 'txFt': return m_textures[2].m_buffer;
    case 'txHa': return m_textures[3].m_buffer;
    case 'vrUp': return m_vertices[0].m_buffer;
    case 'vrLo': return m_vertices[1].m_buffer;
    case 'vrFt': return m_vertices[2].m_buffer;
    case 'vrHa': return m_vertices[3].m_buffer;
    }
    return 0;
}

int ModelMLX::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 'indx': return m_id;
    case 'alUp': return m_alpha[0];
    case 'alLo': return m_alpha[1];
    case 'alFt': return m_alpha[2];
    case 'alHa': return m_alpha[3];
    }
    return 0;
}

bool ModelMLX::setString(DWORD tag, const wchar_t* value)
{
    switch (tag)
    {
    case 'name': m_name = value; return true;
    case 'txUp': m_textures[0] = value; return true;
    case 'txLo': m_textures[1] = value; return true;
    case 'txFt': m_textures[2] = value; return true;
    case 'txHa': m_textures[3] = value; return true;
    case 'vrUp': m_vertices[0] = value; return true;
    case 'vrLo': m_vertices[1] = value; return true;
    case 'vrFt': m_vertices[2] = value; return true;
    case 'vrHa': m_vertices[3] = value; return true;
    default: return false;
    }
}

bool ModelMLX::setInt(DWORD tag, int value)
{
    switch (tag)
    {
    case 'indx': m_id = value; return true;
    case 'alUp': m_alpha[0] = value; return true;
    case 'alLo': m_alpha[1] = value; return true;
    case 'alFt': m_alpha[2] = value; return true;
    case 'alHa': m_alpha[3] = value; return true;
    default: return false;
    }
}

void ModelMLX::read(iStream& in, DWORD fmt)
{
    m_id = in.readInt();
    m_name.read(in, 4, in.m_codePage);
    m_textures[0].read(in, 4, in.m_codePage);
    m_vertices[0].read(in, 4, in.m_codePage);
    if (fmt == '2XLM')
    {
        m_alpha[0] = in.readInt();
        for (int i = 1; i < 4; ++i)
        {
            m_textures[i].read(in, 4, in.m_codePage);
            m_vertices[i].read(in, 4, in.m_codePage);
            m_alpha[i] = in.readInt();
        }
    }
    else
    {
        for (int i = 1; i < 4; ++i)
        {
            m_textures[i].read(in, 4, in.m_codePage);
            m_vertices[i].read(in, 4, in.m_codePage);
        }
    }
}

void ModelMLX::write(oStream& out, DWORD fmt) const
{
    out << (unsigned long)m_id;
    m_name.write(out, 4, out.m_codePage);
    m_textures[0].write(out, 4, out.m_codePage);
    m_vertices[0].write(out, 4, out.m_codePage);
    if (fmt == '2XLM')
    {
        out << (unsigned long)m_alpha[0];
        for (int i = 1; i < 4; ++i)
        {
            m_textures[i].write(out, 4, out.m_codePage);
            m_vertices[i].write(out, 4, out.m_codePage);
            out << (unsigned long)m_alpha[i];
        }
    }
    else
    {
        for (int i = 1; i < 4; ++i)
        {
            m_textures[i].write(out, 4, out.m_codePage);
            m_vertices[i].write(out, 4, out.m_codePage);
        }
    }
}

void ModelMLX::reset()
{
    m_name.clear();
    for (int i = 0; i < 4; ++i)
    {
        m_textures[i].clear();
        m_vertices[i].clear();
        m_alpha[i] = 1;
    }
}

static int p02_cmpInt(int a, int b)
{
    if (a != b)
        return 2 * (a >= b) - 1;
    return 0;
}

int ModelMLX::compare(const Node* other, DWORD tag) const
{
    const ModelMLX* p = dynamic_cast<const ModelMLX*>(other);
    switch (tag)
    {
    case 'indx': return p02_cmpInt(m_id, p->m_id);
    case 'name': return String::compareLogical(m_name, p->m_name);
    case 'txUp': return String::compareLogical(m_textures[0], p->m_textures[0]);
    case 'txLo': return String::compareLogical(m_textures[1], p->m_textures[1]);
    case 'txFt': return String::compareLogical(m_textures[2], p->m_textures[2]);
    case 'txHa': return String::compareLogical(m_textures[3], p->m_textures[3]);
    case 'vrUp': return String::compareLogical(m_vertices[0], p->m_vertices[0]);
    case 'vrLo': return String::compareLogical(m_vertices[1], p->m_vertices[1]);
    case 'vrFt': return String::compareLogical(m_vertices[2], p->m_vertices[2]);
    case 'vrHa': return String::compareLogical(m_vertices[3], p->m_vertices[3]);
    case 'alUp': return p02_cmpInt(m_alpha[0], p->m_alpha[0]);
    case 'alLo': return p02_cmpInt(m_alpha[1], p->m_alpha[1]);
    case 'alFt': return p02_cmpInt(m_alpha[2], p->m_alpha[2]);
    case 'alHa': return p02_cmpInt(m_alpha[3], p->m_alpha[3]);
    default: return 0;
    }
}

bool ModelMLX::toText(String& out, DWORD fmt) const
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

    if (fmt == '----' || fmt == 'tabs')
    {
        out << (long)m_id << sep << m_name;
        if (fmt == '----')
        {
            for (int i = 0; i < 4; ++i)
                out << sep << m_textures[i] << sep << m_vertices[i] << sep << (long)m_alpha[i];
        }
        out.newline();
        return true;
    }
    if (fmt != 'csv ')
        return false;
    {
        String name = String::sqlQuote(m_name.m_buffer);
        out << (long)m_id << sep << name;
    }
    for (int i = 0; i < 4; ++i)
    {
        String vertex = String::sqlQuote(m_vertices[i].m_buffer);
        String texture = String::sqlQuote(m_textures[i].m_buffer);
        out << sep << texture << sep << vertex << sep << (long)m_alpha[i];
    }
    out << L"\r\n";
    return true;
}
