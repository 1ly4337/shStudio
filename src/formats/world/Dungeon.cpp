#include "Dungeon.h"
#include "Name.h"
#include "ListCtrl.h"
#include "StudioFrame.h"
#include "Bytes.h"
#include "imageio.h"
#include "DIBPixels.h"
#include <GL/glu.h>
#include <cfloat>
#include <cmath>

wchar_t g_dungeonItemName[32];

static inline bool lexLessEqual(const fPoint& a, const fPoint& b)
{
    if (a.x == b.x)
    {
        if (a.y == b.y)
            return a.z <= b.z;
        return a.y <= b.y;
    }
    return a.x <= b.x;
}

String& appendFPoint(String& out, const fPoint* p)
{
    out << L'(';
    if (fabsf(p->x) >= 0.00001f)
        out << p->x << L',';
    else
        out << 0.0f << L',';
    if (fabsf(p->y) >= 0.00001f)
        out << p->y << L',';
    else
        out << 0.0f << L',';
    out << (fabsf(p->z) >= 0.00001f ? p->z : 0.0f);
    out << L')';
    return out;
}

bool Dungeon::Segment::Matrice::toText(String& out, DWORD ) const
{
    for (int i = 0; i < 5; ++i)
    {
        out << L'(';
        appendFPoint(out, &p[i]) << L')';
        out.newline();
    }
    out.newline();
    return true;
}

Dungeon::Segment::Module::pvBlock::pvBlock(int , int )
{
}

void Dungeon::Segment::Module::pvBlock::read(iStream& in, DWORD )
{
    m_id = in.readInt();
    m_pos.read(in, 'file');
    m_value = in.readFloat();
    m_type = in.readInt();
    m_dir.read(in, 'file');
    m_params[0] = in.readInt();
    m_params[1] = in.readInt();
    m_params[2] = in.readInt();
    m_params[3] = in.readInt();
    m_points[0].read(in, 'file');
    m_points[1].read(in, 'file');
    m_points[2].read(in, 'file');
}

void Dungeon::Segment::Module::pvBlock::write(oStream& out, DWORD ) const
{
    m_pos.write(out << (unsigned long)m_id, 'file');
    oStream& o = out << m_value << (unsigned long)m_type;
    m_dir.write(o, 'file');
    oStream& o2 = o << (unsigned long)m_params[0] << (unsigned long)m_params[1] << (unsigned long)m_params[2]
                    << (unsigned long)m_params[3];
    m_points[0].write(o2, 'file');
    m_points[1].write(o2, 'file');
    m_points[2].write(o2, 'file');
}

bool Dungeon::Segment::Module::pvBlock::toText(String& out, DWORD fmt) const
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
    String& a = out << (long)m_id << sep;
    String& b = appendFPoint(a, &m_pos) << sep;
    String& c = b << m_value << sep;
    String& d = c << (long)m_type << sep;
    appendFPoint(d, &m_dir);
    for (int i = 0; i < 4; ++i)
        out << sep << (long)m_params[i];
    for (int i = 0; i < 3; ++i)
    {
        out << sep;
        appendFPoint(out, &m_points[i]);
    }
    return true;
}

Dungeon::Segment::Module::Block::Vertex::Vertex(int , int )
    : m_color(0), m_u2(0), m_v2(0)
{
}

void Dungeon::Segment::Module::Block::Vertex::read(iStream& in, DWORD )
{
    m_pos.read(in, 'file');
    m_normal.read(in, 'file');
    m_color = in.readInt();
    m_u = in.readFloat();
    m_v = in.readFloat();
    m_u2 = in.readFloat();
    m_v2 = in.readFloat();
    m_pos.x = -m_pos.x;
}

void Dungeon::Segment::Module::Block::Vertex::write(oStream& out, DWORD ) const
{
    oStream& o = out << -m_pos.x << m_pos.y << m_pos.z;
    m_normal.write(o, 'file');
    o << (unsigned long)m_color << m_u << m_v << m_u2 << m_v2;
}

bool Dungeon::Segment::Module::Block::Vertex::toText(String& out, DWORD fmt) const
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
    String& a = appendFPoint(out, &m_pos) << sep;
    String& b = appendFPoint(a, &m_normal) << sep;
    b << m_color << sep << m_u << sep << m_v << sep << m_u2 << sep << m_v2;
    return true;
}

Dungeon::Segment::Module::Block::Block(int , int )
    : m_id(0)
{
}

Dungeon::Segment::Module::Block::~Block()
{
}

void Dungeon::Segment::Module::Block::clear()
{
    m_vertices.deleteAll();
    m_faces.deleteAll();
}

void Dungeon::Segment::Module::Block::translate(const fPoint* )
{
}

void Dungeon::Segment::Module::Block::fillProperties(ListCtrl* )
{
}

bool Dungeon::Segment::Module::Block::isEmpty() const
{
    return m_faces.isEmpty();
}

short Dungeon::Segment::Module::Block::getImage(int state, short ) const
{
    if (state == 'name')
        return 0;
    return 1024;
}

const wchar_t* Dungeon::Segment::Module::Block::getString(DWORD tag) const
{
    if (tag != 'name')
        return 0;
    memset(g_dungeonItemName, 0, sizeof(g_dungeonItemName));
    swprintf_s(g_dungeonItemName, L"%s # %d", L"Block", m_id);
    return g_dungeonItemName;
}

int Dungeon::Segment::Module::Block::getInt(DWORD tag) const
{
    return tag == '1col';
}

void Dungeon::Segment::Module::Block::scale(const fPoint* factor)
{
    int n = m_vertices.size();
    for (int i = 0; i < n; ++i)
    {
        Vertex* v = m_vertices.get(i);
        v->m_pos.x = factor->x * v->m_pos.x;
        v->m_pos.y = v->m_pos.y * factor->x;
        v->m_pos.z = v->m_pos.z * factor->z;
    }
}

void Dungeon::Segment::Module::Block::computeBounds()
{
    int n = m_vertices.size();
    if (n)
    {
        m_min.z = FLT_MAX;
        m_min.y = FLT_MAX;
        m_min.x = FLT_MAX;
        m_max.z = -FLT_MAX;
        m_max.y = -FLT_MAX;
        m_max.x = -FLT_MAX;
        for (int i = 0; i < n; ++i)
        {
            const fPoint& p = m_vertices.m_items[i]->m_pos;
            const fPoint& lo = lexLessEqual(p, m_min) ? p : m_min;
            m_min.x = lo.x;
            m_min.y = lo.y;
            m_min.z = lo.z;
            const fPoint& hi = lexLessEqual(m_max, p) ? p : m_max;
            m_max.x = hi.x;
            m_max.y = hi.y;
            m_max.z = hi.z;
        }
    }
    else
    {
        m_min.z = 0.0f;
        m_min.y = 0.0f;
        m_min.x = 0.0f;
        m_max.z = 0.0f;
        m_max.y = 0.0f;
        m_max.x = 0.0f;
    }
}

void Dungeon::Segment::Module::Block::drawWireframe(GLenum , GLenum )
{
    glBegin(GL_TRIANGLES);
    int n = m_faces.size();
    for (int i = 0; i < n; ++i)
    {
        Face* f = m_faces.m_items[i];
        Vertex* a = m_vertices.get(f->m_v1);
        Vertex* b = m_vertices.get(f->m_v2);
        Vertex* c = m_vertices.get(f->m_v3);
        glVertex3f(-a->m_pos.x, a->m_pos.y, a->m_pos.z);
        glVertex3f(-b->m_pos.x, b->m_pos.y, b->m_pos.z);
        glVertex3f(-c->m_pos.x, c->m_pos.y, c->m_pos.z);
    }
    glEnd();
}

void Dungeon::Segment::Module::Block::drawImmediate()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glBegin(GL_TRIANGLES);
    int n = m_faces.size();
    for (int i = 0; i < n; ++i)
    {
        Face* f = m_faces.m_items[i];
        Vertex* a = m_vertices.get(f->m_v1);
        Vertex* b = m_vertices.get(f->m_v2);
        Vertex* c = m_vertices.get(f->m_v3);
        if (!a || !b || !c)
            continue;
        if ((f->m_flags & 1) == 0)
        {
            glTexCoord2f(a->m_u, -a->m_v);
            glVertex3f(-a->m_pos.x, a->m_pos.y, a->m_pos.z);
            glTexCoord2f(b->m_u, -b->m_v);
            glVertex3f(-b->m_pos.x, b->m_pos.y, b->m_pos.z);
            glTexCoord2f(c->m_u, -c->m_v);
            glVertex3f(-c->m_pos.x, c->m_pos.y, c->m_pos.z);
        }
        else
        {
            glEnd();
            glDisable(GL_TEXTURE_2D);
            glPolygonMode(GL_FRONT, GL_FILL);
            glPolygonMode(GL_BACK, GL_LINE);
            glBegin(GL_TRIANGLES);
            glColor3f(0.0f, 1.0f, 1.0f);
            glVertex3f(-a->m_pos.x, a->m_pos.y, a->m_pos.z);
            glColor3f(1.0f, 0.0f, 1.0f);
            glVertex3f(-b->m_pos.x, b->m_pos.y, b->m_pos.z);
            glColor3f(1.0f, 1.0f, 0.0f);
            glVertex3f(-c->m_pos.x, c->m_pos.y, c->m_pos.z);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex3f(-a->m_pos.x, a->m_pos.y, a->m_pos.z);
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex3f(-c->m_pos.x, c->m_pos.y, c->m_pos.z);
            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-b->m_pos.x, b->m_pos.y, b->m_pos.z);
            glEnd();
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, m_texture);
            glBegin(GL_TRIANGLES);
        }
    }
    glEnd();
}

void Dungeon::Segment::Module::Block::read(iStream& in, DWORD fmt)
{
    m_id = in.readInt();
    m_vertices.read(in, fmt);
    m_faces.read(in, fmt);
    if (fmt == 'dgpv')
        m_pvBlocks.read(in, 'dgpv');
}

void Dungeon::Segment::Module::Block::write(oStream& out, DWORD fmt) const
{
    out << (unsigned long)m_id;
    m_vertices.write(out, fmt);
    m_faces.write(out, fmt);
    if (fmt == 'dgpv')
        m_pvBlocks.write(out, 'dgpv');
}

bool Dungeon::Segment::Module::Block::toText(String& out, DWORD ) const
{
    out << (long)m_id << L'\t' << (long)m_vertices.size() << L'\t' << (long)m_faces.size();
    return true;
}

Dungeon::Segment::Module::Module(int index, int )
    : m_index(index), m_unk7C(0)
{
}

Dungeon::Segment::Module::~Module()
{
}

void Dungeon::Segment::Module::clear()
{
    m_blocks.deleteAll();
}

void Dungeon::Segment::Module::translate(const fPoint* )
{
}

void Dungeon::Segment::Module::fillProperties(ListCtrl* )
{
}

bool Dungeon::Segment::Module::isEmpty()
{
    return m_blocks.isEmpty();
}

short Dungeon::Segment::Module::getImage(int state, short ) const
{
    if (state == 'name')
        return 0;
    return 1024;
}

int Dungeon::Segment::Module::getId() const
{
    return 3;
}

const wchar_t* Dungeon::Segment::Module::getString(DWORD tag) const
{
    if (tag != 'name')
        return 0;
    memset(g_dungeonItemName, 0, sizeof(g_dungeonItemName));
    swprintf_s(g_dungeonItemName, L"%s # %d", L"Module", m_index);
    return g_dungeonItemName;
}

int Dungeon::Segment::Module::getInt(DWORD tag) const
{
    return tag == '1col';
}

void Dungeon::Segment::Module::reset()
{
    clear();
}

int Dungeon::Segment::Module::compare(const Node* , DWORD ) const
{
    return 0;
}

void Dungeon::Segment::Module::computeBounds()
{
    int n = m_blocks.size();
    if (n)
    {
        m_min.z = FLT_MAX;
        m_min.y = FLT_MAX;
        m_min.x = FLT_MAX;
        m_max.z = -FLT_MAX;
        m_max.y = -FLT_MAX;
        m_max.x = -FLT_MAX;
        for (int i = 0; i < n; ++i)
        {
            Block* c = m_blocks.m_items[i];
            c->computeBounds();
            const fPoint& lo = lexLessEqual(c->m_min, m_min) ? c->m_min : m_min;
            m_min.x = lo.x;
            m_min.y = lo.y;
            m_min.z = lo.z;
            const fPoint& hi = lexLessEqual(m_max, c->m_max) ? c->m_max : m_max;
            m_max.x = hi.x;
            m_max.y = hi.y;
            m_max.z = hi.z;
        }
    }
    else
    {
        m_min.z = 0.0f;
        m_min.y = 0.0f;
        m_min.x = 0.0f;
        m_max.z = 0.0f;
        m_max.y = 0.0f;
        m_max.x = 0.0f;
    }
}

void Dungeon::Segment::Module::drawWireframe(GLenum front, GLenum back)
{
    int n = m_blocks.size();
    for (int i = 0; i < n; ++i)
        m_blocks.m_items[i]->drawWireframe(front, back);
}

void Dungeon::Segment::Module::drawImmediate()
{
    if (m_texture)
    {
        if ((signed char)m_flags >= 0)
        {
            int n = m_blocks.size();
            for (int i = 0; i < n; ++i)
            {
                m_blocks.m_items[i]->m_texture = m_texture;
                m_blocks.m_items[i]->drawImmediate();
            }
        }
    }
}

void Dungeon::Segment::Module::scale(const fPoint* factor)
{
    int n = m_blocks.size();
    for (int i = 0; i < n; ++i)
        m_blocks.get(i)->scale(factor);
}

void Dungeon::Segment::Module::read(iStream& in, DWORD fmt)
{
    m_textureIndex = in.readInt();
    m_blocks.read(in, fmt);
}

void Dungeon::Segment::Module::write(oStream& out, DWORD fmt) const
{
    out << (unsigned long)m_textureIndex;
    m_blocks.write(out, fmt);
}

bool Dungeon::Segment::Module::toText(String& out, DWORD fmt) const
{
    int n = m_blocks.size();
    out << (long)m_textureIndex << L'\t' << (long)n << " blocks";
    ++out.m_indent;
    out << L'\t';
    out << L"\r\n";
    for (int i = 0; i < n; ++i)
        m_blocks.m_items[i]->toText(out, fmt);
    p02_unindent(out);
    out.newline();
    return true;
}

Dungeon::Segment::Segment(int index, int )
    : m_index(index)
{
}

Dungeon::Segment::~Segment()
{
}

void Dungeon::Segment::clear()
{
    m_modules.deleteAll();
    m_bounds.deleteAll();
}

void Dungeon::Segment::translate(const fPoint* )
{
}

void Dungeon::Segment::fillProperties(ListCtrl* )
{
}

short Dungeon::Segment::getImage(int state, short ) const
{
    if (state == 'name')
        return 0;
    return 1024;
}

int Dungeon::Segment::getId() const
{
    return 2;
}

const wchar_t* Dungeon::Segment::getString(DWORD tag) const
{
    if (tag != 'name')
        return 0;
    memset(g_dungeonItemName, 0, sizeof(g_dungeonItemName));
    swprintf_s(g_dungeonItemName, L"%s # %d", L"Segment", m_index);
    return g_dungeonItemName;
}

int Dungeon::Segment::getInt(DWORD tag) const
{
    return tag == '1col';
}

void Dungeon::Segment::reset()
{
    clear();
}

int Dungeon::Segment::compare(const Node* , DWORD ) const
{
    return 0;
}

void Dungeon::Segment::computeBounds()
{
    int n = m_modules.size();
    if (n)
    {
        m_min.z = FLT_MAX;
        m_min.y = FLT_MAX;
        m_min.x = FLT_MAX;
        m_max.z = -FLT_MAX;
        m_max.y = -FLT_MAX;
        m_max.x = -FLT_MAX;
        for (int i = 0; i < n; ++i)
        {
            Module* c = m_modules.m_items[i];
            c->computeBounds();
            const fPoint& lo = lexLessEqual(c->m_min, m_min) ? c->m_min : m_min;
            m_min.x = lo.x;
            m_min.y = lo.y;
            m_min.z = lo.z;
            const fPoint& hi = lexLessEqual(m_max, c->m_max) ? c->m_max : m_max;
            m_max.x = hi.x;
            m_max.y = hi.y;
            m_max.z = hi.z;
        }
    }
    else
    {
        m_min.z = 0.0f;
        m_min.y = 0.0f;
        m_min.x = 0.0f;
        m_max.z = 0.0f;
        m_max.y = 0.0f;
        m_max.x = 0.0f;
    }
}

void Dungeon::Segment::drawWireframe(GLenum front, GLenum back)
{
    int n = m_modules.size();
    for (int i = 0; i < n; ++i)
        m_modules.m_items[i]->drawWireframe(front, back);
}

void Dungeon::Segment::drawImmediate()
{
    int nm = m_modules.size();
    for (int i = 0; i < nm; ++i)
        m_modules.m_items[i]->drawImmediate();
    if ((m_flags & 2) == 0)
        return;
    int nb = m_bounds.size();
    for (int b = 0; b < nb; ++b)
    {
        Bound* bound = m_bounds.get(b);
        int nf = bound->m_faces.size();
        if (nf < 1)
            continue;
        glDisable(GL_TEXTURE_2D);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(1.0f);
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < nf; ++i)
        {
            fPoint** pts = bound->m_points.m_items;
            Face* f = bound->m_faces.m_items[i];
            fPoint* p2 = pts[f->m_v2];
            fPoint* p3 = pts[f->m_v3];
            if ((f->m_flags & 1) == 0)
            {
                fPoint* p1 = pts[f->m_v1];
                glVertex3f(-p1->x, p1->y, p1->z);
                glVertex3f(-p2->x, p2->y, p2->z);
                glVertex3f(-p3->x, p3->y, p3->z);
            }
        }
        glEnd();
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_LINE);
        glBegin(GL_TRIANGLES);
        int i = 0;
        do
        {
            Face* f = bound->m_faces.m_items[i];
            if ((f->m_flags & 1) != 0)
            {
                fPoint** pts = bound->m_points.m_items;
                fPoint* p1 = pts[f->m_v1];
                fPoint* p2 = pts[f->m_v2];
                fPoint* p3 = pts[f->m_v3];
                glColor3f(0.0f, 1.0f, 1.0f);
                glVertex3f(-p1->x, p1->y, p1->z);
                glColor3f(1.0f, 1.0f, 0.0f);
                glVertex3f(-p2->x, p2->y, p2->z);
                glColor3f(1.0f, 0.0f, 1.0f);
                glVertex3f(-p3->x, p3->y, p3->z);
                glColor3f(0.0f, 0.0f, 1.0f);
                glVertex3f(-p1->x, p1->y, p1->z);
                glColor3f(1.0f, 0.0f, 0.0f);
                glVertex3f(-p3->x, p3->y, p3->z);
                glColor3f(0.0f, 1.0f, 0.0f);
                glVertex3f(-p2->x, p2->y, p2->z);
            }
            ++i;
        } while (i < nf);
        glEnd();
    }
    if (nb > 0)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Dungeon::Segment::scale(const fPoint* factor)
{
    int nm = m_modules.size();
    for (int i = 0; i < nm; ++i)
        m_modules.get(i)->scale(factor);
    int nb = m_bounds.size();
    for (int j = 0; j < nb; ++j)
        m_bounds.get(j);
}

DWORD Dungeon::Segment::setFlag(DWORD mask, bool on)
{
    if (on)
        m_flags |= mask;
    else
        m_flags &= ~mask;
    if ((mask & 1) == 0)
    {
        int n = m_modules.size();
        for (int i = 0; i < n; ++i)
            m_modules.m_items[i]->setFlag(mask, on);
    }
    return 0;
}

void Dungeon::Segment::read(iStream& in, DWORD fmt)
{
    for (int i = 0; i < 5; ++i)
        m_matrice.p[i].read(in, 'file');
    m_modules.read(in, fmt);
    m_bounds.read(in, fmt);
    computeBounds();
}

void Dungeon::Segment::write(oStream& out, DWORD fmt) const
{
    for (int i = 0; i < 5; ++i)
        m_matrice.p[i].write(out, 'file');
    m_modules.write(out, fmt);
    m_bounds.write(out, fmt);
}

bool Dungeon::Segment::toText(String& out, DWORD fmt) const
{
    ++out.m_indent;
    out << L'\t';
    out << "Matrice:" << L"\r\n";
    m_matrice.toText(out, fmt);
    p02_chop(out, 1);
    out << "Modules:" << L"\r\n";
    p02_chop(out, 1);
    out << "Bounds:" << L"\r\n";
    int nb = m_bounds.size();
    for (int i = 0; i < nb; ++i)
    {
        Bound* b = m_bounds.get(i);
        out << (long)b->m_points.size() << L'\t' << (long)b->m_faces.size();
        out.newline();
    }
    p02_chop(out, 1);
    p02_unindent(out);
    return true;
}

Dungeon::Dungeon(int )
{
    m_names.m_owner = (int)this;
    m_recordCount = 0;
    m_textures = 0;
    Dungeon::clear();
}

Dungeon::~Dungeon()
{
    Dungeon::clear();
}

void Dungeon::clear()
{
    if (m_textures)
        glDeleteTextures(m_names.size(), m_textures);
    m_max.z = 0.0f;
    m_max.y = 0.0f;
    m_max.x = 0.0f;
    m_min.x = 0.0f;
    m_min.y = m_max.y;
    m_min.z = m_max.z;
    m_names.deleteAll();
    m_slots.m_count = 0;
    m_segments.deleteAll();
}

void Dungeon::translate(const fPoint* )
{
}

bool Dungeon::isEmpty() const
{
    return 1;
}

void Dungeon::computeBounds()
{
    int n = m_segments.size();
    if (n)
    {
        m_min.z = FLT_MAX;
        m_min.y = FLT_MAX;
        m_min.x = FLT_MAX;
        m_max.z = -FLT_MAX;
        m_max.y = -FLT_MAX;
        m_max.x = -FLT_MAX;
        for (int i = 0; i < n; ++i)
        {
            Segment* c = m_segments.m_items[i];
            c->computeBounds();
            const fPoint& lo = lexLessEqual(c->m_min, m_min) ? c->m_min : m_min;
            m_min.x = lo.x;
            m_min.y = lo.y;
            m_min.z = lo.z;
            const fPoint& hi = lexLessEqual(m_max, c->m_max) ? c->m_max : m_max;
            m_max.x = hi.x;
            m_max.y = hi.y;
            m_max.z = hi.z;
        }
    }
    else
    {
        m_min.z = 0.0f;
        m_min.y = 0.0f;
        m_min.x = 0.0f;
        m_max.z = 0.0f;
        m_max.y = 0.0f;
        m_max.x = 0.0f;
    }
}

void Dungeon::scale(const fPoint* factor)
{
    int n = m_segments.size();
    for (int i = 0; i < n; ++i)
        m_segments.m_items[i]->scale(factor);
    computeBounds();
}

void Dungeon::read(iStream& in, DWORD fmt)
{
    clear();
    try
    {
        m_min.read(in, 'file');
        m_max.read(in, 'file');
        m_names.read(in, fmt);
        String log;
        m_recordCount = in.readInt();
        log << (long)m_recordCount << L"\r\n";
        for (int i = 0; in.m_size - in.m_pos != 0; ++i)
        {
            int slot = (int)in.readInt();
            m_slots.add(slot);
            (log << (long)slot).newline();
            if (slot == 0)
                continue;
            if (slot != 1)
                throw 1;
            Segment* segment = new Segment(i, 0);
            try
            {
                segment->read(in, fmt);
            }
            catch (...)
            {
                delete segment;
                throw;
            }
            m_segments.add(segment);
        }
    }
    catch (...)
    {
        clear();
        throw;
    }
}

void Dungeon::write(oStream& out, DWORD fmt) const
{
    m_min.write(out, 'file');
    m_max.write(out, 'file');
    m_names.write(out, fmt);
    out << (unsigned long)m_recordCount;
    int k = 0;
    int n = m_slots.m_count;
    for (int i = 0; i < n; ++i)
    {
        long slot = (i < 0 || i >= m_slots.m_count) ? 0 : m_slots.m_items[i];
        out << (unsigned long)slot;
        if (slot == 1)
            m_segments.get(k++)->write(out, fmt);
    }
}

bool Dungeon::toText(String& out, DWORD fmt) const
{
    if (fmt == 'resf')
    {
        m_names.toText(out, 'resf');
        out << L"\r\n";
        return true;
    }
    out << L'(';
    appendFPoint(out, &m_min) << L')';
    out << L"\r\n";
    out << L'(';
    appendFPoint(out, &m_max) << L')';
    out << L"\r\n";
    int n = m_segments.size();
    out << (long)n << L" segments" << L"\r\n" << L"\r\n";
    for (int i = 0; i < n; ++i)
    {
        Segment* s = m_segments.get(i);
        (out << L"Segment:\t" << (long)s->m_index).newline();
        m_segments.get(i)->toText(out, fmt);
        out.newline().newline();
    }
    return true;
}

bool Dungeon::loadTextures()
{
    int n = m_names.size();
    m_textures = (GLuint*)::operator new[](((unsigned __int64)(unsigned)n >> 30) != 0 ? (size_t)-1 : 4 * n);
    memset(m_textures, 0, 4 * n);
    glGenTextures(n, m_textures);
    if (glGetError())
        return false;
    Bytes data(0, (UINT)0);
    String dir(L"Entity\\Texture\\");
    bool ok = true;
    for (int i = 0; i < n; ++i)
    {
        Name* name = m_names.get(i);
        String file(name->m_file);
        if (!file.m_length)
            continue;
        HRESULT hr;
        {
            String path = dir + file;
            hr = g_studioFrame->loadFromFile(&data, &path, false);
        }
        if (hr)
        {
            if (file.m_length >= 4 && !_wcsnicmp(file.m_buffer + file.m_length - 4, L".tga", 4))
            {
                file.removeExtension();
                file << L".dds";
                String path = dir + file;
                hr = g_studioFrame->loadFromFile(&data, &path, false);
            }
            if (hr)
            {
                ok = false;
                break;
            }
        }
        HGDIOBJ bitmap = 0;
        if (!loadDDS(data, &bitmap))
        {
            DIBPixels pixels;
            if (pixels.load((HBITMAP)bitmap))
            {
                glBindTexture(GL_TEXTURE_2D, m_textures[i]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                gluBuild2DMipmaps(GL_TEXTURE_2D, 0x8058 , pixels.m_width, pixels.m_height,
                                  0x80E1 , GL_UNSIGNED_BYTE, pixels.m_bits);
                if (glGetError())
                    hr = E_FAIL;
            }
            DeleteObject(bitmap);
            if (pixels.m_bits)
            {
                ::operator delete[](pixels.m_bits);
                pixels.m_bits = 0;
            }
        }
        if (hr)
        {
            ok = false;
            break;
        }
    }
    if (!ok)
    {
        glDeleteTextures(n, m_textures);
        ::operator delete[](m_textures);
        m_textures = 0;
        return false;
    }
    int ns = m_segments.size();
    for (int s = 0; s < ns; ++s)
    {
        Collection<Segment::Module>& modules = m_segments.get(s)->m_modules;
        int nm = modules.size();
        for (int m = 0; m < nm; ++m)
        {
            Segment::Module* module = modules.get(m);
            int t = module->m_textureIndex;
            if (t >= 0 && t < n)
                module->setTexture(m_textures[t]);
        }
    }
    m_texture = (GLuint)-1;
    return true;
}

void Dungeon::drawWireframe(GLenum front, GLenum back)
{
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT, front);
    glPolygonMode(GL_BACK, back);
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    int n = m_segments.size();
    for (int i = 0; i < n; ++i)
        m_segments.m_items[i]->drawWireframe(front, back);
}

void Dungeon::drawImmediate()
{
    if (m_texture != (GLuint)-1)
    {
        loadTextures();
        if (m_texture != (GLuint)-1)
        {
            drawWireframe(GL_LINE, GL_POINT);
            return;
        }
    }
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0);
    if ((m_flags & 1) != 0)
    {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.25f);
    }
    else
    {
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
    }
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    int n = m_segments.size();
    for (int i = 0; i < n; ++i)
        m_segments.m_items[i]->drawImmediate();
}

DWORD Dungeon::setFlag(DWORD mask, bool on)
{
    if (on)
        m_flags |= mask;
    else
        m_flags &= ~mask;
    if ((mask & 1) == 0)
    {
        int n = m_segments.size();
        for (int i = 0; i < n; ++i)
            m_segments.m_items[i]->setFlag(mask, on);
    }
    return 0;
}

void Dungeon::fillProperties(ListCtrl* list)
{
    int vertices = 0;
    int faces = 0;
    int modules = 0;
    int n = m_segments.size();
    for (int s = 0; s < n; ++s)
    {
        Collection<Segment::Module>& mods = m_segments.get(s)->m_modules;
        int nm = mods.size();
        modules += nm;
        for (int m = 0; m < nm; ++m)
        {
            Collection<Segment::Module::Block>& blocks = mods.get(m)->m_blocks;
            int nb = blocks.size();
            for (int b = 0; b < nb; ++b)
            {
                Segment::Module::Block* block = blocks.get(b);
                vertices += block->m_vertices.size();
                faces += block->m_faces.size();
            }
        }
    }
    list->insertNumberRow(L"Number of segments", n, n);
    list->insertNumberRow(L"Number of modules", modules, 0);
    list->insertNumberRow(L"Number of vertices", vertices, 0);
    list->insertNumberRow(L"Number of faces", faces, 0);
}
