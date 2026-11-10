#include "VaniSubStruct.h"
#include "p04_common.h"
#include "iStream.h"
#include "oStream.h"
#include <cfloat>
#include "String_p02.h"

void VaniSubStruct::VertexVANI::read(iStream& in, DWORD fmt)
{
    m_pos.read(in, 'file');
    m_normal.read(in, 'file');
    m_bone = (long)in.readDword();
    m_u = in.readFloat();
    m_v = in.readFloat();
}

void VaniSubStruct::VertexVANI::write(oStream& out, DWORD fmt) const
{
    m_pos.write(out, 'file');
    m_normal.write(out, 'file');
    out << m_bone;
    out << m_u;
    out << m_v;
}

bool VaniSubStruct::VertexVANI::toText(String& out, DWORD fmt) const
{
    if (fmt == 'tabs')
    {
        appendFPoint(out, &m_pos);
        out << L'\t';
        appendFPoint(out, &m_normal);
        out << L'\t';
        out << (unsigned long)m_bone;
        out << L'\t';
        out << m_u;
        out << L'\t';
        out << m_v;
        out << L"\r\n";
        return true;
    }
    if (fmt == 'type')
    {
        out << L"typedef struct _VertexVANI {\r\n"
               L"\tfloat\t\tx, y, z;\r\n"
               L"\tfloat\t\tnx, ny, nz;\r\n"
               L"\tlong\t\tbone;\r\n"
               L" /* always -1 */\tfloat\t\tu, v;\r\n"
               L"} Vertices;\r\n"
               L"\r\n";
        return true;
    }
    out << m_pos.x << ", ";
    out << m_pos.y << ", ";
    out << m_pos.z << ", ";
    out << m_normal.x << ", ";
    out << m_normal.y << ", ";
    out << m_normal.z << ", ";
    out << (unsigned long)m_bone << ", ";
    out << m_u << ", ";
    out << m_v;
    return true;
}

void VaniSubStruct::VertexVANI::assign(const Vertex* src)
{
    m_pos.x = src->m_pos.x;
    m_pos.y = src->m_pos.y;
    m_pos.z = src->m_pos.z;
    m_u = src->m_u;
    m_v = src->m_v;
    const VertexVANI* other = dynamic_cast<const VertexVANI*>(src);
    if (other)
    {
        m_normal.x = other->m_normal.x;
        m_normal.y = other->m_normal.y;
        m_normal.z = other->m_normal.z;
    }
}

VaniSubStruct::VaniSubStruct(int nSteps)
    : Struct3D()
{
    m_nSteps = nSteps;
    m_curStep = 0;
}

VaniSubStruct::~VaniSubStruct()
{
}

Vertex* VaniSubStruct::getVertex(int index)
{
    int i = index + m_nVertsPerStep * m_curStep;
    if (i < 0 || i >= m_vertices.m_count)
        return 0;
    return m_vertices.m_items[i];
}

void VaniSubStruct::computeBounds()
{
    m_min.z = FLT_MAX;
    m_min.y = FLT_MAX;
    m_min.x = FLT_MAX;
    m_max.z = -FLT_MAX;
    m_max.y = -FLT_MAX;
    m_max.x = -FLT_MAX;
    int count = m_vertices.size();
    for (int i = 0; i < count; ++i)
    {
        VertexVANI* v;
        if (i < 0 || i >= m_vertices.m_count)
            v = 0;
        else
            v = dynamic_cast<VertexVANI*>(m_vertices.m_items[i]);
        if (m_min.x > v->m_pos.x)
            m_min.x = v->m_pos.x;
        if (m_min.y > v->m_pos.y)
            m_min.y = v->m_pos.y;
        if (m_min.z > v->m_pos.z)
            m_min.z = v->m_pos.z;
        if (v->m_pos.x > m_max.x)
            m_max.x = v->m_pos.x;
        if (v->m_pos.y > m_max.y)
            m_max.y = v->m_pos.y;
        if (v->m_pos.z > m_max.z)
            m_max.z = v->m_pos.z;
    }
    m_center.x = (m_max.x + m_min.x) * 0.5f;
    m_center.y = (m_max.y + m_min.y) * 0.5f;
    m_center.z = (m_max.z + m_min.z) * 0.5f;
}

void VaniSubStruct::read(iStream& in, DWORD fmt)
{
    m_textureName.read(in, 4, in.m_codePage);
    m_faces.read(in, fmt);
    int n = (int)in.readDword();
    int total = n * m_nSteps;
    m_nVertsPerStep = n;
    if (total > 0)
    {
        do
        {
            VertexVANI* v = new VertexVANI;
            v->read(in, fmt);
            m_vertices.add(v);
            --total;
        } while (total);
    }

    int nFaces = m_faces.size();
    for (int i = 0; i < nFaces; ++i)
    {
        Face* face = m_faces.m_items[i];
        int i1 = face->m_v1;
        VertexVANI* a = (i1 < 0 || i1 >= m_vertices.m_count) ? 0 : dynamic_cast<VertexVANI*>(m_vertices.m_items[i1]);
        int i2 = face->m_v2;
        VertexVANI* b = (i2 < 0 || i2 >= m_vertices.m_count) ? 0 : dynamic_cast<VertexVANI*>(m_vertices.m_items[i2]);
        int i3 = face->m_v3;
        VertexVANI* c = (i3 < 0 || i3 >= m_vertices.m_count) ? 0 : dynamic_cast<VertexVANI*>(m_vertices.m_items[i3]);
        if (a && b && c)
        {
            fPoint ca = c->m_normal - a->m_normal;
            fPoint ba = b->m_normal - a->m_normal;
            fPoint n = ba.unitCross(ca);
            face->m_normal.x = n.x;
            face->m_normal.y = n.y;
            face->m_normal.z = n.z;
        }
    }
}

void VaniSubStruct::write(oStream& out, DWORD fmt) const
{
    m_textureName.write(out, 4, out.m_codePage);
    m_faces.write(out, fmt);
    out << m_nVertsPerStep;
    int total = m_nVertsPerStep * m_nSteps;
    for (int i = 0; i < total; ++i)
    {
        Vertex* v;
        if (i < 0 || i >= m_vertices.m_count)
            v = 0;
        else
            v = m_vertices.m_items[i];
        v->write(out, fmt);
    }
}

bool VaniSubStruct::loadTexture(const String& dir)
{
    bool ok;
    String path(dir);
    path.append(m_textureName.m_buffer, m_textureName.m_length);
    path.removeExtension();
    path << L".dds";
    HGDIOBJ bitmap = 0;
    if (loadTextureBitmap(path, (HBITMAP*)&bitmap))
        ok = setBitmap((HBITMAP)bitmap);
    else
        ok = false;
    return ok;
}
