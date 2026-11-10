#include "kWorld_NamedArea.h"
#include "iStream.h"
#include "oStream.h"
#include "Name.h"
#include "String_p02.h"
#include "CapeData.h"

namespace kWorld
{

    NamedArea::NamedArea(int index)
        : CaptionNode()
        , m_pos()
        , m_rot()
    {
        m_index = index;
    }

    short NamedArea::getImage(int state, short defImage) const
    {
        switch (state)
        {
        case 'x3__':
        case 'y3__':
        case 'z3__':
        case 'unkn':
            return 18;
        case 'posX':
        case 'posY':
        case 'posZ':
        case 'rotX':
        case 'rotY':
        case 'rotZ':
            return 130;
        }
        return defImage;
    }

    const wchar_t* NamedArea::getString(DWORD tag) const
    {
        if (tag != 'name')
            return 0;
        sub_43CBF0(g_textBuffer, L"#%d", m_index);
        return g_textBuffer;
    }

    int NamedArea::getInt(DWORD tag) const
    {
        switch (tag)
        {
        case 'posX': return floatBits(m_pos.x);
        case 'posY': return floatBits(m_pos.y);
        case 'posZ': return floatBits(m_pos.z);
        case 'rotX': return floatBits(m_rot.x);
        case 'rotY': return floatBits(m_rot.y);
        case 'rotZ': return floatBits(m_rot.z);
        case 'x3__': return m_x3;
        case 'y3__': return m_y3;
        case 'z3__': return m_z3;
        }
        return 0;
    }

    void NamedArea::reset()
    {
        m_rot.x = m_rot.y = m_rot.z = 0;
        m_pos.x = 0;
        m_pos.y = m_rot.y;
        m_pos.z = m_rot.z;
        m_z3 = 0;
        m_y3 = 0;
        m_x3 = 0;
        m_name1.clear();
        m_name2.clear();
    }

    void NamedArea::read(iStream& in, DWORD fmt)
    {
        m_pos.read(in, 'file');
        m_rot.read(in, 'file');
        m_x3 = in.readInt();
        readFixedString256(in, m_name1, 949);
        readFixedString256(in, m_name2, 949);
        m_y3 = in.readInt();
        m_z3 = in.readInt();
    }

    void NamedArea::write(oStream& out, DWORD fmt) const
    {
        m_pos.write(out, 'file');
        m_rot.write(out, 'file');
        out << m_x3;
        writeFixedString256(out, m_name1, 949);
        writeFixedString256(out, m_name2, 949);
        out << m_y3 << m_z3;
    }

    bool NamedArea::toText(String& out, DWORD fmt) const
    {
        wchar_t sep = p04_separator(fmt);
        unsigned flags = out.m_flags;
        out.m_flags = flags | 0x11;
        out << L'(';
        appendFPoint(out, &m_pos) << L')' << sep << L'(';
        appendFPoint(out, &m_rot) << L')' << sep;
        out.m_flags = 0;
        out << (long)m_x3 << sep << (long)m_y3 << sep << (long)m_z3 << sep;
        if (m_y3 == 2)
            out.append(m_name1.m_buffer, m_name1.m_length);
        out.m_flags = flags;
        return true;
    }

    int NamedArea::getVersion() const
    {
        return 565;
    }

    NamedAreas::NamedAreas()
        : ptrArray<NamedArea*>()
        , ToggleNode(L"Named Areas")
    {
    }

    NamedAreas::~NamedAreas()
    {
    }

    const wchar_t* NamedAreas::getString(DWORD tag) const
    {
        const wchar_t* result = L"Named Areas";
        if (tag != 'name')
            return 0;
        return result;
    }

    void NamedAreas::read(iStream& in, DWORD fmt)
    {
        deleteAll();
        int count = (int)in.readInt();
        for (int i = 0; i < count; ++i)
        {
            NamedArea* area = new NamedArea(i);
            area->read(in, fmt);
            add(area);
        }
    }

    void NamedAreas::write(oStream& out, DWORD fmt) const
    {
        out << m_count;
        for (int i = 0; i < m_count; ++i)
            m_items[i]->write(out, fmt);
    }

    bool NamedAreas::toText(String& out, DWORD fmt) const
    {
        if (fmt == 'dump')
            out << "Named Aeras" << L"\r\n";
        for (int i = 0; i < m_count; ++i)
        {
            out << L'\t' << L'#';
            out << (long)i << L'\t';
            m_items[i]->toText(out, fmt);
            out.newline();
        }
        return true;
    }
}
