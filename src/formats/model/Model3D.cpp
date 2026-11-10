#include "Model3D.h"
#include "iStream.hpp"
#include "oStream.hpp"
#include "Bytes.h"
#include "ListCtrl.h"
#include "ContentProvider.h"
#include "String_p02.h"
#include "_imports_w3.h"

void Model3D::Vertex3DE::read(iStream& s, DWORD )
{
    m_pos.read(s, 0x66696C65 );
    m_bone = s.readInt();
    m_u = s.readFloat();
    m_v = s.readFloat();
}

void Model3D::Vertex3DE::write(oStream& s, DWORD ) const
{
    m_pos.write(s, 0x66696C65 );
    s << (unsigned long)m_bone << (float)m_u << (float)m_v;
}

void Model3D::Vertex3DE::assign(const Vertex* src)
{
    m_pos.x = src->m_pos.x;
    m_pos.y = src->m_pos.y;
    m_pos.z = src->m_pos.z;
    m_u = src->m_u;
    m_v = src->m_v;
    const Vertex3DE* e = dynamic_cast<const Vertex3DE*>(src);
    if (e)
        m_bone = e->m_bone;
}

bool Model3D::Vertex3DE::toText(String& out, DWORD fmt) const
{
    if (fmt == 0x74616273 )
    {
        String& a = appendFPoint(out, &m_pos);
        a << L'\t';
        String& b = a << (long)m_bone;
        b << L'\t';
        String& c = b << m_u;
        c << L'\t';
        c << m_v << L"\r\n";
        return true;
    }
    if (fmt == 0x74797065 )
    {
        out << L"typedef struct _Vertex3DE {\r\n"
                       L"\tfloat\t\tx, y, z;\r\n"
                       L"\tlong\t\tbone;\r\n"
                       L"\tfloat\t\tu, v;\r\n"
                       L"} Vertices;\r\n"
                       L"\r\n";
        return true;
    }
    out << m_pos.x << ", "
        << m_pos.y << ", "
        << m_pos.z << ", "
        << (long)m_bone << ", "
        << m_u << ", "
        << m_v;
    return true;
}

void Model3D::Vertex3DO::read(iStream& s, DWORD )
{
    m_pos.read(s, 0x66696C65 );
    m_normal.read(s, 0x66696C65 );
    m_u = s.readFloat();
    m_v = s.readFloat();
}

void Model3D::Vertex3DO::write(oStream& s, DWORD ) const
{
    m_pos.write(s, 0x66696C65 );
    m_normal.write(s, 0x66696C65 );
    s << (float)m_u << (float)m_v;
}

void Model3D::Vertex3DO::assign(const Vertex* src)
{
    m_pos.x = src->m_pos.x;
    m_pos.y = src->m_pos.y;
    m_pos.z = src->m_pos.z;
    m_u = src->m_u;
    m_v = src->m_v;
    const Vertex3DO* o = dynamic_cast<const Vertex3DO*>(src);
    if (o)
    {
        m_normal.x = o->m_normal.x;
        m_normal.y = o->m_normal.y;
        m_normal.z = o->m_normal.z;
    }
}

bool Model3D::Vertex3DO::toText(String& out, DWORD fmt) const
{
    if (fmt == 0x74616273 )
    {
        String& a = appendFPoint(out, &m_pos);
        a << L'\t';
        String& b = appendFPoint(a, &m_normal);
        b << L'\t';
        String& c = b << m_u;
        c << L'\t';
        c << m_v << L"\r\n";
        return true;
    }
    if (fmt == 0x74797065 )
    {
        out << L"typedef struct _Vertex3DO {\r\n"
                       L"\tfloat\t\tx, y, z;\r\n"
                       L"\tfloat\t\tnx, ny, nz;\r\n"
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
        << m_normal.z << ", "
        << m_u << ", "
        << m_v;
    return true;
}

Model3D::Vertex3DC::Vertex3DC(int version)
{
    m_version = version;
}

void Model3D::Vertex3DC::read(iStream& s, DWORD )
{
    m_pos.read(s, 0x66696C65 );
    m_w = s.readFloat();
    if (m_version >= 1536)
    {
        m_u1 = s.readFloat();
        m_u2 = s.readFloat();
    }
    m_bones = s.readInt();
    m_normal.read(s, 0x66696C65 );
    m_u = s.readFloat();
    m_v = s.readFloat();
}

void Model3D::Vertex3DC::write(oStream& s, DWORD ) const
{
    m_pos.write(s, 0x66696C65 );
    s << (float)m_w;
    if (m_version >= 1536)
        s << (float)m_u1 << (float)m_u2;
    oStream& o = s << (unsigned long)m_bones;
    m_normal.write(o, 0x66696C65 );
    o << (float)m_u << (float)m_v;
}

void Model3D::Vertex3DC::assign(const Vertex* src)
{
    m_pos.x = src->m_pos.x;
    m_pos.y = src->m_pos.y;
    m_pos.z = src->m_pos.z;
    m_u = src->m_u;
    m_v = src->m_v;
    const Vertex3DO* o = dynamic_cast<const Vertex3DO*>(src);
    if (o)
    {
        m_normal.x = o->m_normal.x;
        m_normal.y = o->m_normal.y;
        m_normal.z = o->m_normal.z;
    }
    const Vertex3DC* c = dynamic_cast<const Vertex3DC*>(src);
    if (c)
    {
        m_version = c->m_version;
        m_w = c->m_w;
        m_bones = c->m_bones;
        m_u1 = c->m_u1;
        m_u2 = c->m_u2;
    }
}

bool Model3D::Vertex3DC::toText(String& out, DWORD fmt) const
{
    if (fmt == 0x74616273 )
    {
        String& a = appendFPoint(out, &m_pos);
        a << L'\t';
        String& b = a << m_w;
        b << L'\t';
        if (m_version >= 1536)
        {
            String& c = out << m_u1;
            c << L'\t';
            String& d = c << m_u2;
            d << L'\t';
        }
        String& e = out << (unsigned char)m_bones;
        e << L'\t';
        String& f = e << (unsigned char)(m_bones >> 8);
        f << L'\t';
        String& g = appendFPoint(f, &m_normal);
        g << L'\t';
        String& h = g << m_u;
        h << L'\t';
        h << m_v << L"\r\n";
        return true;
    }
    if (fmt == 0x74797065 )
    {
        if (m_version < 1536)
            out << L"typedef struct _Vertex3DC {\r\n"
                           L"\tfloat\t\tx, y, z;\r\n"
                           L"\tfloat\t\tw;\r\n"
                           L"\tbyte\t\tbone1, bone2;\r\n"
                           L"\tshort\t\talignment;\r\n"
                           L"\tfloat\t\tnx, ny, nz;\r\n"
                           L"\tfloat\t\tu, v;\r\n"
                           L"} Vertices;\r\n"
                           L"\r\n";
        else
            out << L"typedef struct _Vertex3DC {\r\n"
                           L"\tfloat\t\tx, y, z;\r\n"
                           L"\tfloat\t\tw;\r\n"
                           L"\tfloat\t\tu1, u2;\r\n"
                           L"\tbyte\t\tbone1, bone2;\r\n"
                           L"\tshort\t\talignment;\r\n"
                           L"\tfloat\t\tnx, ny, nz;\r\n"
                           L"\tfloat\t\tu, v;\r\n"
                           L"} Vertices;\r\n"
                           L"\r\n";
        return true;
    }
    out << m_pos.x << ", "
        << m_pos.y << ", "
        << m_pos.z << ", "
        << m_w;
    if (m_version >= 1536)
    {
        out << ", " << m_u1 << ", " << m_u2;
    }
    out << ", "
        << (unsigned char)m_bones << ", "
        << (unsigned char)(m_bones >> 8) << ", "
        << m_normal.x << ", "
        << m_normal.y << ", "
        << m_normal.z << ", "
        << m_u << ", "
        << m_v;
    return true;
}

Model3D::Model3D(int type)
    : Struct3D(), m_textureName()
{
    m_type = type;
    m_version = 1280;
    m_matrixCount = 0;
    m_matrices = 0;
    m_hBitmap = 0;
}

Model3D::~Model3D()
{
    Model3D::clear();
}

int Model3D::serialV5() const
{
    return m_version;
}

HRESULT Model3D::setCodePage(int version)
{
    m_version = version;
    if (m_type != 0x6B334443 )
        return 0;
    int count = m_vertices.size();
    for (int i = 0; i < count; ++i)
    {
        Vertex* v = (i < 0 || i >= m_vertices.m_count) ? 0 : m_vertices.m_items[i];
        Vertex3DC* c = dynamic_cast<Vertex3DC*>(v);
        if (c)
            c->m_version = version;
    }
    return 0;
}

void Model3D::clear()
{
    Struct3D::clear();
    if (m_matrices)
    {
        delete[] m_matrices;
        m_matrices = 0;
    }
    m_matrixCount = 0;
    if (m_hBitmap)
    {
        DeleteObject(m_hBitmap);
        m_hBitmap = 0;
    }
}

void Model3D::fillProperties(ListCtrl* list)
{
    list->insertNumberRow(L"Format", (m_version >> 8) & 0xF, 0);
    list->insertNumberRow(L"Number of vertices", m_vertices.size(), 0);
    list->insertNumberRow(L"Number of faces", m_faces.size(), 0);
}

void Model3D::readModel(iStream& s, int version, bool hasMatrices)
{
    clear();
    if (hasMatrices)
    {
        int n = s.readInt();
        m_matrixCount = n;
        if (n <= 0 || 68 * n >= s.m_size - s.m_pos)
        {
            m_matrixCount = 0;
            if (s.m_pos >= 4)
                s.m_pos = s.m_pos - 4;
        }
        else
        {
            m_matrices = new Matrix4[n];
            for (int i = 0; i < m_matrixCount; ++i)
                m_matrices[i].read(s, 0x66696C65 );
        }
    }

    int vertexCount = s.readInt();
    if (vertexCount < 0)
        throw 0;
    for (int left = vertexCount; left > 0; --left)
    {
        Vertex* v;
        int type = m_type;
        if (type == 0x6B334445 )
            v = new Vertex3DE();
        else if (type == 0x6B33444F )
            v = new Vertex3DO();
        else if (type == 0x6B334443 )
            v = new Vertex3DC(version);
        else
            continue;
        if (v)
        {
            v->read(s, 0x66696C65 );
            m_vertices.add(v);
        }
    }

    int faceCount = s.readInt();
    if (faceCount < 0)
        throw 0;
    for (int j = 0; j < faceCount; ++j)
    {
        Face* f = new Face((short)j);
        f->read(s, 0x66696C65 );
        m_faces.add(f);
    }

    if (m_type == 0x6B33444F  || m_type == 0x6B334443 )
    {
        for (int i = 0; i < faceCount; ++i)
        {
            Face* f = m_faces.m_items[i];
            Vertex3DO* a = f->m_v1 >= m_vertices.m_count ? 0 : dynamic_cast<Vertex3DO*>(m_vertices.m_items[f->m_v1]);
            Vertex3DO* b = f->m_v2 >= m_vertices.m_count ? 0 : dynamic_cast<Vertex3DO*>(m_vertices.m_items[f->m_v2]);
            Vertex3DO* c = f->m_v3 >= m_vertices.m_count ? 0 : dynamic_cast<Vertex3DO*>(m_vertices.m_items[f->m_v3]);
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
    }
    computeBounds();
}

static bool Model3D_detectFormat(iStream& s, String& textureName, int* version, bool isK3DC)
{
    int n = s.readInt();
    if ((unsigned int)(n - 1) <= 0xFE)
    {
        if (s.m_pos >= 4)
            s.m_pos = s.m_pos - 4;
        textureName.read(s, 4, s.m_codePage);
        return isK3DC;
    }
    if (n != 444)
        return isK3DC;
    *version = 1536;
    return true;
}

HRESULT Model3D::loadFiles(const wchar_t* modelFile, const wchar_t* textureFile)
{
    if (!modelFile)
        return E_POINTER;
    if (!textureFile)
        return E_POINTER;
    ContentProvider* fs = g_contentProvider;
    if (!fs && *modelFile == L'\\')
        return E_POINTER;

    HGDIOBJ bitmap = 0;
    Bytes bytes;
    int err;
    if (fs)
        err = fs->readPath(textureFile, &bytes);
    else
        err = bytes.load(textureFile);
    if (!err)
    {
        err = loadDDS(bytes, &bitmap);
        if (!err)
        {
            if (g_contentProvider)
                err = g_contentProvider->readPath(modelFile, &bytes);
            else
                err = bytes.load(modelFile);
            if (!err)
            {
                try
                {
                    read(bytes, 0x66696C65 );
                    setBitmap((HBITMAP)bitmap);
                    return err;
                }
                catch (...)
                {
                    err = 13;
                }
            }
        }
    }
    if (bitmap)
        DeleteObject(bitmap);
    return err;
}

void Model3D::read(iStream& s, DWORD )
{
    if (s.m_size - s.m_pos < 4)
        throw 0;
    try
    {
        m_version = 0x500;
        bool hasMatrices = Model3D_detectFormat(s, m_textureName, &m_version, m_type == 0x6B334443 );
        readModel(s, m_version, hasMatrices);
    }
    catch (...)
    {
        if (m_type != 0x6B334443 )
        {
            clear();
            throw 1;
        }
        s.m_pos = 0;
        Model3D_detectFormat(s, m_textureName, &m_version, false);
        try
        {
            readModel(s, m_version, false);
        }
        catch (...)
        {
            clear();
            throw;
        }
    }
}

void Model3D::write(oStream& s, DWORD tag) const
{
    s << (unsigned long)(m_version < 1536 ? 0 : 444);
    if (m_matrixCount > 0)
    {
        s << (unsigned long)m_matrixCount;
        for (int i = 0; i < m_matrixCount; ++i)
            m_matrices[i].write(s, tag);
    }
    Struct3D::write(s, tag);
}

bool Model3D::toText(String& out, DWORD fmt) const
{
    if (m_matrices && m_matrixCount > 0)
    {
        if (fmt == 0x74797065 )
        {
            out << L"typedef struct _Matrix4x4 {\r\n\tfloat\t\tm[4][4];\r\n} Matrix4x4;\r\n\r\n";
            out.m_flags &= ~1u;
            out << "Matrix4x4 matrixes[/* " << (long)m_matrixCount << " */] = {" << L"\r\n";
            for (int i = 0; i < m_matrixCount; ++i)
            {
                out.m_flags &= ~1u;
                out << "/*" << (long)i << "*/\t{ ";
                out.m_flags |= 1u;
                m_matrices[i].toText(out, 0x636F6465 );
                out << " }," << L"\r\n";
            }
            for (int k = 0; k < 3; ++k)
            {
                int len = out.length();
                if (!len)
                    break;
                out.m_length = len - 1;
                out.m_buffer[len - 1] = 0;
            }
            out << L"\r\n";
            out << "};" << L"\r\n";
        }
        else
        {
            int i = 0;
            do
            {
                m_matrices[i].toText(out, 0x64756D70 );
                out << L"\r\n";
                ++i;
            } while (i < m_matrixCount);
        }
        out << L"\r\n";
    }
    return Struct3D::toText(out, fmt);
}

void* Model3D::getPositions(int* outSize)
{
    int count = m_vertices.size();
    if (count <= 0)
        return 0;
    Float3* r = new Float3[count];
    if (r)
    {
        for (int i = 0; i < count; ++i)
        {
            Vertex* v = (i < 0 || i >= m_vertices.m_count) ? 0 : m_vertices.m_items[i];
            r[i].x = v->m_pos.x;
            r[i].y = v->m_pos.y;
            r[i].z = v->m_pos.z;
        }
        *outSize = 12 * count;
    }
    return r;
}

void* Model3D::getNormals(int* outSize)
{
    if (m_type != 0x6B33444F )
        return 0;
    int count = m_vertices.size();
    if (count <= 0)
        return 0;
    Float3* r = new Float3[count];
    if (!r)
        return 0;
    for (int i = 0; i < count; ++i)
    {
        Vertex* v = (i < 0 || i >= m_vertices.m_count) ? 0 : m_vertices.m_items[i];
        Vertex3DO* o = dynamic_cast<Vertex3DO*>(v);
        if (o)
        {
            r[i].x = o->m_normal.x;
            r[i].y = o->m_normal.y;
            r[i].z = o->m_normal.z;
        }
    }
    *outSize = 12 * count;
    return r;
}

void* Model3D::getIndices(int* outSize)
{
    int count = m_faces.size();
    if (count <= 0)
        return 0;
    Short3* r = new Short3[count];
    if (r)
    {
        for (int i = 0; i < count; ++i)
        {
            Face* f = (Face*)m_faces.get(i);
            r[i].a = f->m_v1;
            r[i].b = f->m_v2;
            r[i].c = f->m_v3;
        }
        *outSize = 6 * count;
    }
    return r;
}

void* Model3D::getTexCoords(int* outSize)
{
    int count = m_vertices.size();
    if (count <= 0)
        return 0;
    Float2* r = new Float2[count];
    if (r)
    {
        for (int i = 0; i < count; ++i)
        {
            Vertex* v = (i < 0 || i >= m_vertices.m_count) ? 0 : m_vertices.m_items[i];
            r[i].u = v->m_u;
            r[i].v = -v->m_v;
        }
        *outSize = 8 * count;
    }
    return r;
}

bool Model3D::loadTexture(const wchar_t* )
{
    if (m_texture)
        return true;
    if (m_bitmap || (m_textureName.length() && loadTextureBitmap(m_textureName, &m_bitmap)))
        return setBitmap(m_bitmap);
    return false;
}
