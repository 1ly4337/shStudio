#include "SuperModule.h"
#include "Module.hpp"
#include "Face.h"
#include "String.hpp"
#include "iStream.hpp"
#include "oStream.hpp"
#include "ContentProvider.h"
#include "String_p02.h"
#include "_imports_w3.h"
#include <GL/gl.h>

static inline float& radiusOf(SuperModule* m) { return m->m_A8; }
static inline float radiusOf(const SuperModule* m) { return m->m_A8; }

static inline void w3_indent(String& s)
{
    ++s.m_indent;
    s << L'\t';
}

static inline void w3_unindent(String& s)
{
    --s.m_indent;
    wchar_t* b = s.m_buffer;
    if (b)
    {
        int len = s.length();
        if (b[len - 1] == L'\t')
            b[len] = 0;
        b = s.m_buffer;
        if (b)
        {
            len = s.length();
            if (b[len - 1] == L'\t')
            {
                s.m_length = len - 1;
                b[len - 1] = 0;
            }
        }
    }
}

void SuperModule::clear()
{
    m_parts.deleteAll();
    m_center.z = 0;
    m_center.y = 0;
    m_center.x = 0;
    m_min.z = 0;
    m_min.y = 0;
    m_min.x = 0;
    m_max.z = 0;
    m_max.y = 0;
    m_max.x = 0;
    m_texture = 0;
    m_98.z = 0;
    m_98.y = 0;
    m_98.x = 0;
    radiusOf(this) = 0;
    m_BC.z = 0;
    m_BC.y = 0;
    m_BC.x = 0;
    m_AC.x = 0;
    m_AC.y = m_BC.y;
    m_AC.z = m_BC.z;
    m_CC[1].z = 0;
    m_CC[1].y = 0;
    m_CC[1].x = 0;
    m_CC[0].x = 0;
    m_CC[0].y = m_CC[1].y;
    m_CC[0].z = m_CC[1].z;
    m_points.deleteAll();
    m_faces.deleteAll();
}

void SuperModule::read(iStream& s, DWORD tag)
{
    try
    {
        m_98.read(s, 0x66696C65 );
        radiusOf(this) = s.readFloat();
        m_AC.read(s, 0x66696C65 );
        m_BC.read(s, 0x66696C65 );
        int moduleCount = s.readInt();
        for (int i = 0; i < moduleCount; ++i)
        {
            Module* m = new Module();
            m->read(s, tag);
            m_parts.add(m);
        }
        computeBounds();
        m_CC[0].read(s, 0x66696C65 );
        m_CC[1].read(s, 0x66696C65 );

        int groups = s.readInt();
        if (groups > 1)
            groups = 1;
        for (int g = 0; g < groups; ++g)
        {
            int pointCount = s.readInt();
            for (int j = 0; j < pointCount; ++j)
            {
                fPoint* p = new fPoint();
                p->read(s, tag);
                m_points.add(p);
            }
            int faceCount = s.readInt();
            for (int k = 0; k < faceCount; ++k)
            {
                Face* f = new Face((short)k);
                f->read(s, tag);
                m_faces.add(f);
            }
        }
    }
    catch (...)
    {
        clear();
        throw;
    }
}

void SuperModule::write(oStream& s, DWORD tag) const
{
    m_98.write(s, 0x66696C65 );
    oStream& o = s << radiusOf(this);
    m_AC.write(o, 0x66696C65 );
    m_BC.write(o, 0x66696C65 );
    int moduleCount = m_parts.size();
    s << (unsigned long)moduleCount;
    for (int i = 0; i < moduleCount; ++i)
        ((Module*)m_parts.m_items[i])->write(s, tag);
    m_CC[0].write(s, 0x66696C65 );
    m_CC[1].write(s, 0x66696C65 );
    s << (unsigned long)1;
    int pointCount = m_points.size();
    s << (unsigned long)pointCount;
    for (int j = 0; j < pointCount; ++j)
        m_points.m_items[j]->write(s, tag);
    int faceCount = m_faces.size();
    s << (unsigned long)faceCount;
    for (int k = 0; k < faceCount; ++k)
        m_faces.m_items[k]->write(s, tag);
}

bool SuperModule::toText(String& out, DWORD fmt) const
{
    if (fmt != 0x636F6465 )
        return false;
    out << L"SuperModule module = {";
    w3_indent(out);
    out << L"\r\n";
    String& a = appendFPoint(out, &m_98);
    a << L',';
    String& b = a << L"\r\n" << radiusOf(this);
    b << L',';
    String& c = appendFPoint(b << L"\r\n", &m_AC);
    c << L',';
    String& d = appendFPoint(c << L"\r\n", &m_BC);
    d << L',';
    d << L"\r\n";

    int moduleCount = m_parts.size();
    if (moduleCount > 0)
    {
        out << L'{';
        w3_indent(out);
        out << L"\r\n";
        for (int i = 0; i < moduleCount; ++i)
        {
            out << L'{';
            w3_indent(out);
            out << L"\r\n";
            ((Module*)m_parts.m_items[i])->toText(out, 0x636F6465 );
            w3_unindent(out);
            out << "}," << L"\r\n";
        }
        w3_unindent(out);
        out << L'}';
        out << L"\r\n";
    }
    appendFPoint(out, &m_CC[0]) << L"\r\n";
    appendFPoint(out, &m_CC[1]) << L"\r\n";
    w3_unindent(out);
    out << L'}';
    out << L"\r\n";
    return true;
}

void SuperModule::vslot24(const fPoint* factor)
{
    int moduleCount = m_parts.size();
    for (int i = 0; i < moduleCount; ++i)
        m_parts.m_items[i]->scale(factor);
    int pointCount = m_points.size();
    if (pointCount > 0)
    {
        fPoint** it = m_points.m_items;
        do
        {
            fPoint* p = *it++;
            p->x = p->x * factor->x;
            p->y = factor->y * p->y;
            p->z = factor->z * p->z;
        } while (--pointCount);
    }
    computeBounds();
    m_98.x = m_center.x;
    m_98.y = m_center.y;
    m_98.z = m_center.z;
    m_AC.x = m_min.x;
    m_AC.y = m_min.y;
    m_AC.z = m_min.z;
    m_BC.x = m_max.x;
    m_BC.y = m_max.y;
    m_BC.z = m_max.z;
}

void SuperModule::vslot23(int mode)
{
    render(mode);
    if ((m_flags & 2) != 0)
    {
        glPolygonMode(GL_FRONT, GL_LINE);
        glPolygonMode(GL_BACK, GL_POINT);
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(1.0f);
        glBegin(GL_TRIANGLES);
        int faceCount = m_faces.size();
        for (int i = 0; i < faceCount; ++i)
        {
            fPoint** pts = m_points.m_items;
            Face* f = m_faces.m_items[i];
            fPoint* a = pts[f->m_v1];
            fPoint* b = pts[f->m_v2];
            fPoint* c = pts[f->m_v3];
            glVertex3f(-a->x, a->y, a->z);
            glVertex3f(-b->x, b->y, b->z);
            glVertex3f(-c->x, c->y, c->z);
        }
        glEnd();
    }
}
