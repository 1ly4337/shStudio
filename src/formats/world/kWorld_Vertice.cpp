#include "kWorld_Vertice.h"
#include "iStream.h"
#include "oStream.h"
#include <typeinfo>
#include "String_p02.h"
#include "CapeData.h"

namespace kWorld
{

    Vertice::Vertice(int index)
        : Node()
        , m_pos()
        , m_rot()
        , m_p3()
    {
        m_index = index;
        reset();
    }

    Vertice::~Vertice()
    {
    }

    short Vertice::getImage(int state, short defImage) const
    {
        switch (state)
        {
        case 'x3__':
        case 'y3__':
        case 'z3__':
        case 'posX':
        case 'posY':
        case 'posZ':
        case 'rotX':
        case 'rotY':
        case 'rotZ':
            return 130;
        case 'unkn':
            return 18;
        }
        return defImage;
    }

    const wchar_t* Vertice::getString(DWORD tag) const
    {
        if (tag != 'name')
            return 0;
        sub_43CBF0(g_textBuffer, L"#%d", m_index);
        return g_textBuffer;
    }

    int Vertice::getInt(DWORD tag) const
    {
        switch (tag)
        {
        case 'idnt': return m_index;
        case 'posX': return floatBits(m_pos.x);
        case 'posY': return floatBits(m_pos.y);
        case 'posZ': return floatBits(m_pos.z);
        case 'rotX': return floatBits(m_rot.x);
        case 'rotY': return floatBits(m_rot.y);
        case 'rotZ': return floatBits(m_rot.z);
        case 'x3__': return floatBits(m_p3.x);
        case 'y3__': return floatBits(m_p3.y);
        case 'z3__': return floatBits(m_p3.z);
        case 'unkn': return m_unkn;
        }
        return 0;
    }

    void Vertice::reset()
    {
        m_p3.x = m_p3.y = m_p3.z = 0;
        m_rot.x = 0;
        m_rot.y = m_p3.y;
        m_rot.z = m_p3.z;
        m_pos.x = 0;
        m_pos.y = m_rot.y;
        m_pos.z = m_rot.z;
        m_unkn = 0;
    }

    void Vertice::read(iStream& in, DWORD fmt)
    {
        m_pos.read(in, 'file');
        m_rot.read(in, 'file');
        m_p3.read(in, 'file');
        m_unkn = in.readInt();
    }

    void Vertice::write(oStream& out, DWORD fmt) const
    {
        m_pos.write(out, 'file');
        m_rot.write(out, 'file');
        m_p3.write(out, 'file');
        out << m_unkn;
    }

    bool Vertice::toText(String& out, DWORD fmt) const
    {
        char c;
        if (fmt == 'dump')
            c = 9;
        else if ((int)fmt > 'dump')
            c = (fmt != 'sql ' && fmt == 'tabs') ? 9 : ',';
        else if (fmt == '----' || fmt == 'csv ')
            c = ';';
        else
            c = ',';
        wchar_t sep = (wchar_t)(short)c;
        appendFPoint(out, &m_pos) << sep;
        appendFPoint(out, &m_rot) << sep;
        appendFPoint(out, &m_p3) << sep;
        out << (long)m_unkn;
        return true;
    }

    bool Vertice::copyFrom(const Streamable* src)
    {
        const Vertice* v = dynamic_cast<const Vertice*>(src);
        if (!v)
            return false;
        m_pos.x = v->m_pos.x;
        m_pos.y = v->m_pos.y;
        m_pos.z = v->m_pos.z;
        m_rot.x = v->m_rot.x;
        m_rot.y = v->m_rot.y;
        m_rot.z = v->m_rot.z;
        m_p3.x = v->m_p3.x;
        m_p3.y = v->m_p3.y;
        m_p3.z = v->m_p3.z;
        m_unkn = v->m_unkn;
        return true;
    }

    Vertices::Vertices()
        : ptrArray<Vertice*>()
        , Node()
    {
    }

    Vertices::~Vertices()
    {
    }

    const wchar_t* Vertices::getString(DWORD tag) const
    {
        const wchar_t* result = 0;
        if (tag == 'name')
            return g_sectionNames[11];
        return result;
    }

    void Vertices::read(iStream& in, DWORD fmt)
    {
        deleteAll();
        if (in.m_size - in.m_pos < 4)
            throw 1;
        int count = (int)in.readInt();
        for (int i = 0; i < count; ++i)
        {
            Vertice* v = new Vertice(i);
            v->read(in, fmt);
            add(v);
        }
    }

    void Vertices::write(oStream& out, DWORD fmt) const
    {
        out << m_count;
        for (int i = 0; i < m_count; ++i)
            m_items[i]->write(out, fmt);
    }

    bool Vertices::toText(String& out, DWORD fmt) const
    {
        if (fmt == 'dump')
            out << "Vertices (" << (long)m_count << " items)" << L"\r\n";
        for (int i = 0; i < m_count; ++i)
        {
            out << L'\t' << L'#';
            out << (long)i << L'\t';
            m_items[i]->toText(out, fmt);
            out.newline();
        }
        out.newline();
        return true;
    }
}
