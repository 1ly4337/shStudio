#include "kWorld_Portal.h"
#include "iStream.h"
#include "oStream.h"
#include "Name.h"
#include "String_p02.h"
#include "CapeData.h"

namespace kWorld
{

    Portal::Portal(int index)
        : CaptionNode()
        , m_pos()
        , m_rot()
        , m_dest()
    {
        m_index = index;
        m_value76 = 0;
        m_value70 = 0;
        m_value78 = 0;
        m_unkn = 0;
    }

    short Portal::getImage(int state, short defImage) const
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

    const wchar_t* Portal::getString(DWORD tag) const
    {
        if (tag != 'name')
            return 0;
        sub_43CBF0(g_textBuffer, L"#%d", m_index);
        return g_textBuffer;
    }

    int Portal::getInt(DWORD tag) const
    {
        switch (tag)
        {
        case 'posX': return floatBits(m_pos.x);
        case 'posY': return floatBits(m_pos.y);
        case 'posZ': return floatBits(m_pos.z);
        case 'rotX': return floatBits(m_rot.x);
        case 'rotY': return floatBits(m_rot.y);
        case 'rotZ': return floatBits(m_rot.z);
        case 'x3__': return floatBits(m_dest.x);
        case 'y3__': return floatBits(m_dest.y);
        case 'z3__': return floatBits(m_dest.z);
        case 'unkn': return m_unkn;
        }
        return 0;
    }

    void Portal::reset()
    {
        m_value70 = 0;
        m_value78 = 0;
        m_unkn = 0;
        m_value76 = 0;
        m_dest.x = m_dest.y = m_dest.z = 0;
        m_rot.x = m_dest.x;
        m_rot.y = m_dest.y;
        m_rot.z = m_dest.z;
        m_pos.x = m_rot.x;
        m_pos.y = m_rot.y;
        m_pos.z = m_rot.z;
        m_name1.clear();
        m_name2.clear();
    }

    bool Portal::copyListTo(oStream& out)
    {
        out << 1;
        write(out, 'clip');
        return true;
    }

    void Portal::read(iStream& in, DWORD fmt)
    {
        m_pos.read(in, 'file');
        m_rot.read(in, 'file');
        m_value70 = in.readInt();
        if (fmt != 'clip')
        {
            readFixedString256(in, m_name1, 949);
            readFixedString256(in, m_name2, 949);
        }
        m_unkn = in.readByte();
        m_value76 = in.readShort();
        m_value78 = in.readByte();
        m_dest.read(in, 'file');
    }

    void Portal::write(oStream& out, DWORD fmt) const
    {
        m_pos.write(out, 'file');
        m_rot.write(out, 'file');
        if (fmt == 'clip')
        {
            out << m_value70;
        }
        else
        {
            out << m_value70;
            writeFixedString256(out, m_name1, 949);
            writeFixedString256(out, m_name2, 949);
        }
        out << (char)m_unkn << (unsigned short)m_value76 << (char)m_value78;
        m_dest.write(out, 'file');
    }

    bool Portal::toText(String& out, DWORD fmt) const
    {
        wchar_t sep = p04_separator(fmt);
        unsigned flags = out.m_flags;
        out.m_flags = flags | 0x11;
        out << L'(';
        appendFPoint(out, &m_pos) << L')' << sep << L'(';
        appendFPoint(out, &m_rot) << L')' << sep;
        out << (long)m_value70 << sep << m_unkn << sep << m_value76 << sep << m_value78 << sep;
        out << L'(';
        appendFPoint(out, &m_dest) << L')';
        out.m_flags = flags;
        return true;
    }

    int Portal::getVersion() const
    {
        return 563;
    }

    Portals::Portals()
        : ptrArray<Portal*>()
        , ToggleNode(L"Portals")
    {
    }

    Portals::~Portals()
    {
    }

    bool Portals::copyListTo(oStream& out)
    {
        out << m_count;
        for (int i = 0; i < m_count; ++i)
            m_items[i]->write(out, 'clip');
        return true;
    }

    void Portals::read(iStream& in, DWORD fmt)
    {
        deleteAll();
        if (in.m_size - in.m_pos < 4)
            throw 1;
        int count = (int)in.readInt();
        if (in.m_size - in.m_pos < 556 * count)
            throw 2;
        for (int i = 0; i < count; ++i)
        {
            Portal* portal = new Portal(i);
            portal->read(in, fmt);
            add(portal);
        }
    }

    void Portals::write(oStream& out, DWORD fmt) const
    {
        out << m_count;
        for (int i = 0; i < m_count; ++i)
            m_items[i]->write(out, fmt);
    }

    bool Portals::toText(String& out, DWORD fmt) const
    {
        wchar_t sep = p04_separator(fmt);
        if (fmt == 'dump')
            out << L"Portals (" << (long)m_count << L" items)" << L"\r\n";
        for (int i = 0; i < m_count; ++i)
        {
            out << L'\t' << L'#';
            out << (long)i << sep;
            m_items[i]->toText(out, fmt);
            out.newline();
        }
        return true;
    }
}
