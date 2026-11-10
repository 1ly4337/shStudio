#include "Struct3D.h"
#include "String.hpp"
#include "iStream.hpp"
#include "oStream.hpp"
#include "Bytes.h"
#include "ListCtrl.h"
#include "ContentProvider.h"
#include "String_p02.h"
#include "_imports_w3.h"
#include <shlwapi.h>
#include <cfloat>

Struct3D::Struct3D()
    : Drawable(),
      Serializable(),
      m_vertices(),
      m_faces()
{
    m_type = 0;
    m_centered = false;
    m_bufC0 = 0;
    m_bufC4 = 0;
    m_unkBC = 0;
    m_unkB8 = 0;
}

Struct3D::~Struct3D()
{
    Struct3D::clear();
}

Vertex* Struct3D::getVertex(int index)
{
    if (index < 0 || index >= m_vertices.m_count)
        return 0;
    return m_vertices.m_items[index];
}

bool Struct3D::isEmpty() const
{
    return m_faces.size() == 0;
}

void Struct3D::clear()
{
    Drawable::clear();
    m_centered = false;
    m_vertices.deleteAll();
    m_faces.deleteAll();
    if (m_bufC0)
    {
        delete[] (char*)m_bufC0;
        m_bufC0 = 0;
    }
    if (m_bufC4)
    {
        delete[] (char*)m_bufC4;
        m_bufC4 = 0;
    }
}

void Struct3D::write(oStream& s, DWORD tag) const
{
    int count = m_vertices.size();
    s << (unsigned long)count;
    for (int i = 0; i < count; ++i)
    {
        Vertex** data = m_vertices.m_items;
        if (m_centered)
        {
            Vertex* v = data[i];
            v->m_pos.x = m_center.x + v->m_pos.x;
            v->m_pos.y = v->m_pos.y + m_center.y;
            v->m_pos.z = m_center.z + v->m_pos.z;
            v->write(s, tag);
            v->m_pos.x = v->m_pos.x - m_center.x;
            v->m_pos.y = v->m_pos.y - m_center.y;
            v->m_pos.z = v->m_pos.z - m_center.z;
        }
        else
        {
            data[i]->write(s, tag);
        }
    }

    int faceCount = m_faces.size();
    int written = 0;
    for (int k = 0; k < faceCount; ++k)
        if ((m_faces.m_items[k]->m_flags & 2) == 0)
            ++written;
    s << (unsigned long)written;
    for (int j = 0; j < faceCount; ++j)
    {
        Face* f = m_faces.m_items[j];
        if ((f->m_flags & 2) == 0)
            f->write(s, tag);
    }
}

bool Struct3D::toText(String& out, DWORD fmt) const
{
    if (fmt == 0x74797065 )
    {
        m_vertices.toText(out, 0x74797065 );
        m_vertices.toText(out, 0x636F6465 );
        m_faces.toText(out, 0x74797065 );
        m_faces.toText(out, 0x636F6465 );
    }
    else
    {
        m_vertices.toText(out, fmt);
        m_faces.toText(out, fmt);
    }
    return true;
}

void Struct3D::computeBounds()
{
    m_min.z = FLT_MAX;
    m_min.y = FLT_MAX;
    m_min.x = FLT_MAX;
    m_max.z = -FLT_MAX;
    m_max.y = -FLT_MAX;
    m_max.x = -FLT_MAX;
    int count = m_vertices.size();
    if (count > 0)
    {
        Vertex** it = m_vertices.m_items;
        do
        {
            Vertex* v = *it;
            if (m_min.x > v->m_pos.x) m_min.x = v->m_pos.x;
            if (m_min.y > v->m_pos.y) m_min.y = v->m_pos.y;
            if (m_min.z > v->m_pos.z) m_min.z = v->m_pos.z;
            if (v->m_pos.x > m_max.x) m_max.x = v->m_pos.x;
            if (v->m_pos.y > m_max.y) m_max.y = v->m_pos.y;
            if (v->m_pos.z > m_max.z) m_max.z = v->m_pos.z;
            ++it;
        } while (--count);
    }
    m_center.x = (m_max.x + m_min.x) * 0.5f;
    m_center.y = (m_max.y + m_min.y) * 0.5f;
    m_center.z = (m_max.z + m_min.z) * 0.5f;
}

void Struct3D::translate(const fPoint* origin)
{
    if (!m_centered)
    {
        int count = m_vertices.size();
        for (int i = 0; i < count; ++i)
        {
            Vertex* v = m_vertices.m_items[i];
            v->m_pos.x = v->m_pos.x - origin->x;
            v->m_pos.y = v->m_pos.y - origin->y;
            v->m_pos.z = v->m_pos.z - origin->z;
        }
        m_centered = true;
    }
}

void Struct3D::scale(const fPoint* scale)
{
    int count = m_vertices.size();
    if (count > 0)
    {
        Vertex** it = m_vertices.m_items;
        do
        {
            Vertex* v = *it++;
            v->m_pos.x = v->m_pos.x * scale->x;
            v->m_pos.y = scale->y * v->m_pos.y;
            v->m_pos.z = scale->z * v->m_pos.z;
        } while (--count);
    }
    computeBounds();
}

void Struct3D::drawWireframe(GLenum front, GLenum back)
{
    glPolygonMode(GL_FRONT, front);
    glPolygonMode(GL_BACK, back);
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    glLineWidth(1.0f);
    drawFaces();
}

void Struct3D::drawFaces()
{
    glBegin(GL_TRIANGLES);
    int count = m_faces.size();
    for (int i = 0; i < count; ++i)
    {
        Face* f = m_faces.m_items[i];
        if ((f->m_flags & 2) != 0)
            continue;
        Vertex* a = getVertex(f->m_v1);
        Vertex* b = getVertex(f->m_v2);
        Vertex* c = getVertex(f->m_v3);
        if (!a || !b || !c)
            continue;
        if ((f->m_flags & 1) != 0)
        {
            glEnd();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glBegin(GL_TRIANGLES);
            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-a->m_pos.x, a->m_pos.y, a->m_pos.z);
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex3f(-b->m_pos.x, b->m_pos.y, b->m_pos.z);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex3f(-c->m_pos.x, c->m_pos.y, c->m_pos.z);
            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-a->m_pos.x, a->m_pos.y, a->m_pos.z);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex3f(-c->m_pos.x, c->m_pos.y, c->m_pos.z);
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex3f(-b->m_pos.x, b->m_pos.y, b->m_pos.z);
            glEnd();
            glPolygonMode(GL_FRONT, GL_LINE);
            glPolygonMode(GL_BACK, GL_POINT);
            glColor3f(0.0f, 0.0f, 0.0f);
            glBegin(GL_TRIANGLES);
        }
        else
        {
            glVertex3f(-a->m_pos.x, a->m_pos.y, a->m_pos.z);
            glVertex3f(-b->m_pos.x, b->m_pos.y, b->m_pos.z);
            glVertex3f(-c->m_pos.x, c->m_pos.y, c->m_pos.z);
        }
    }
    glEnd();
}

void Struct3D::drawImmediate()
{
    if ((m_flags & 1) != 0)
    {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.25f);
    }
    else
    {
        glDisable(GL_ALPHA_TEST);
    }
    drawTextured();
}

GLenum Struct3D::drawTextured()
{
    if (!m_texture)
    {
        drawFaces();
        return 0;
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glBegin(GL_TRIANGLES);
    int count = m_faces.size();
    for (int i = 0; i < count; ++i)
    {
        Face* f = m_faces.m_items[i];
        if ((f->m_flags & 2) != 0)
            continue;
        Vertex* a = getVertex(f->m_v1);
        Vertex* b = getVertex(f->m_v2);
        Vertex* c = getVertex(f->m_v3);
        if (!a || !b || !c)
            continue;
        if (byte_59FC6A && (a->m_pos.y < -1000.0f || a->m_pos.y > 1000.0f))
            continue;
        if ((f->m_flags & 1) != 0)
        {
            glEnd();
            glDisable(GL_TEXTURE_2D);
            glPolygonMode(GL_FRONT, GL_FILL);
            glPolygonMode(GL_BACK, GL_POINT);
            glBegin(GL_TRIANGLES);
            glNormal3f(f->m_normal.x, f->m_normal.y, f->m_normal.z);
            glColor3f(0.0f, 1.0f, 1.0f);
            glVertex3f(-a->m_pos.x, a->m_pos.y, a->m_pos.z);
            glColor3f(1.0f, 1.0f, 0.0f);
            glVertex3f(-b->m_pos.x, b->m_pos.y, b->m_pos.z);
            glColor3f(1.0f, 0.0f, 1.0f);
            glVertex3f(-c->m_pos.x, c->m_pos.y, c->m_pos.z);
            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-a->m_pos.x, a->m_pos.y, a->m_pos.z);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex3f(-c->m_pos.x, c->m_pos.y, c->m_pos.z);
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex3f(-b->m_pos.x, b->m_pos.y, b->m_pos.z);
            glEnd();
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, m_texture);
            glBegin(GL_TRIANGLES);
        }
        else
        {
            glNormal3f(f->m_normal.x, f->m_normal.y, f->m_normal.z);
            glTexCoord2f(a->m_u, -a->m_v);
            glVertex3f(-a->m_pos.x, a->m_pos.y, a->m_pos.z);
            glTexCoord2f(b->m_u, -b->m_v);
            glVertex3f(-b->m_pos.x, b->m_pos.y, b->m_pos.z);
            glTexCoord2f(c->m_u, -c->m_v);
            glVertex3f(-c->m_pos.x, c->m_pos.y, c->m_pos.z);
        }
    }
    glEnd();
    return glGetError();
}

void Struct3D::fillProperties(ListCtrl* list)
{
    list->insertNumberRow(L"Number of vertices", m_vertices.size(), 0);
    list->insertNumberRow(L"Number of faces", m_faces.size(), 0);
}

bool loadTextureBitmap(String& path, HBITMAP* outBitmap)
{
    *outBitmap = 0;
    if (!path.length())
        return false;
    const wchar_t* ext = PathFindExtensionW(path.m_buffer);
    if (!ext)
        return false;

    Bytes bytes;
    int err;
    if (g_contentProvider)
        err = g_contentProvider->readPath(path.m_buffer, &bytes);
    else
        err = bytes.load(path.m_buffer);

    if (err)
    {
        if (!_wcsicmp(ext, L".tga"))
        {
            ext = L".dds";
            path.removeExtension();
            path << L".dds";
            if (g_contentProvider)
                err = g_contentProvider->readPath(path.m_buffer, &bytes);
            else
                err = bytes.load(path.m_buffer);
        }
        if (err)
            return false;
    }

    bool ok;
    if (_wcsicmp(ext, L".tga"))
    {
        if (!_wcsicmp(ext, L".dds"))
            ok = loadDDS(bytes, (HGDIOBJ*)outBitmap) == 0;
        else
            ok = false;
    }
    else
    {
        ok = loadTGA(bytes, (HGDIOBJ*)outBitmap) == 0;
    }
    return ok;
}
