#include "Module.hpp"
#include "iStream.hpp"
#include "oStream.hpp"
#include "ContentProvider.h"
#include "String_p02.h"
#include "_imports_w3.h"

void Module::Vertex::read(iStream& s, DWORD )
{
    m_pos.read(s, 0x66696C65 );
    m_normal.read(s, 0x66696C65 );
    m_mask = s.readInt();
    m_u = s.readFloat();
    m_v = s.readFloat();
    m_pos.x = -m_pos.x;
}

void Module::Vertex::write(oStream& s, DWORD ) const
{
    oStream& o = s << (float)-m_pos.x << (float)m_pos.y << (float)m_pos.z;
    m_normal.write(o, 0x66696C65 );
    o << (unsigned long)m_mask << (float)m_u << (float)m_v;
}

bool Module::Vertex::toText(String& out, DWORD fmt) const
{
    if (fmt == 0x74616273 )
    {
        String& a = out << m_pos.x;
        a << L'\t';
        String& b = a << m_pos.y;
        b << L'\t';
        String& c = b << m_pos.z;
        c << L'\t';
        String& d = c << m_normal.x;
        d << L'\t';
        String& e = d << m_normal.y;
        e << L'\t';
        String& f = e << m_normal.z;
        f << L'\t';
        String& g = f << (int)m_mask;
        g << L'\t';
        String& h = g << m_u;
        h << L'\t';
        h << m_v << L"\r\n";
        return true;
    }
    if (fmt == 0x74797065 )
    {
        out << L"typedef struct _Vertex3DC {\r\n"
                       L"\tfloat\t\tx, y, z;\r\n"
                       L"\tfloat\t\tnx, ny, nz;\r\n"
                       L"\tCOLORREF\tmask;\r\n"
                       L"\tfloat\t\tu, v;\r\n"
                       L"} Vertices;\r\n"
                       L"\r\n";
        return true;
    }
    out << m_pos.x << ", "
        << m_pos.y << ", "
        << m_pos.z << ", "
        << m_normal.x << ", "
        << m_normal.y << ", "
        << m_normal.z << ", ";
    out.m_radix = 16;
    out << "0x" << (int)m_mask;
    out.m_radix = 16;
    out << ", " << m_u << ", " << m_v;
    return true;
}

Module::Module()
    : Struct3D(), m_textureName()
{
}

Module::~Module()
{
    m_textureName.clear();
    Struct3D::clear();
}

void Module::clear()
{
    m_textureName.clear();
    Struct3D::clear();
}

bool Module::loadTexture(const wchar_t* dir)
{
    if (!m_textureName.length())
        return false;
    String path;
    const wchar_t* prefix = L"Entity\\Texture\\";
    if (!g_contentProvider)
        prefix = dir;
    path << prefix;
    bool failed;
    {
        String full = path + (m_textureName);
        failed = !loadTextureBitmap(full, &m_bitmap);
    }
    bool result;
    if (failed)
        result = false;
    else
        result = setBitmap(m_bitmap);
    return result;
}

void Module::read(iStream& s, DWORD tag)
{
    try
    {
        m_textureName.read(s, 4, s.m_codePage);
        int vertexCount = s.readInt();
        for (int i = 0; i < vertexCount; ++i)
        {
            Module::Vertex* v = new Module::Vertex();
            v->read(s, tag);
            m_vertices.add(v);
        }

        int faceCount = s.readInt();
        for (int j = 0; j < faceCount; ++j)
        {
            Face* f = new Face((short)j);
            f->read(s, tag);
            m_faces.add(f);
        }

        for (int k = 0; k < faceCount; ++k)
        {
            Face* f = m_faces.m_items[k];
            Module::Vertex* a = f->m_v1 >= m_vertices.m_count ? 0 : dynamic_cast<Module::Vertex*>(m_vertices.m_items[f->m_v1]);
            Module::Vertex* b = f->m_v2 >= m_vertices.m_count ? 0 : dynamic_cast<Module::Vertex*>(m_vertices.m_items[f->m_v2]);
            Module::Vertex* c = f->m_v3 >= m_vertices.m_count ? 0 : dynamic_cast<Module::Vertex*>(m_vertices.m_items[f->m_v3]);
            if (a && b && c)
            {
                fPoint e1 = c->m_normal - a->m_normal;
                fPoint e2 = b->m_normal - a->m_normal;
                fPoint n = e2.unitCross(e1);
                f->m_normal.x = n.x;
                f->m_normal.y = n.y;
                f->m_normal.z = n.z;
            }
        }
        computeBounds();
    }
    catch (...)
    {
        clear();
        throw;
    }
}

void Module::write(oStream& s, DWORD tag) const
{
    m_textureName.write(s, 4, s.m_codePage);
    Struct3D::write(s, tag);
}

bool Module::toText(String& out, DWORD fmt) const
{
    bool result = false;
    if (fmt == 0x636F6465 )
    {
        out << L'"';
        String& s = out.append(m_textureName.m_buffer, m_textureName.length());
        s << L'"';
        s << L',';
        s << L"\r\n";
        return m_vertices.toText(out, 0x636F6465 ) && m_faces.toText(out, 0x636F6465 );
    }
    return result;
}
