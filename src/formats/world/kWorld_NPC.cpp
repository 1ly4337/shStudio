#include "kWorld_NPC.h"
#include "iStream.h"
#include "oStream.h"
#include "String_p02.h"
#include "CapeData.h"

namespace kWorld
{

    NPC::NPC(int index)
        : Node()
        , m_pos()
        , m_points()
    {
        reset();
        m_index = index;
    }

    NPC::NPC(const NPC& other)
        : Node()
        , m_pos()
        , m_points()
    {
        m_index = other.m_index;
        m_npcType = other.m_npcType;
        m_npcId = other.m_npcId;
        m_pos.x = other.m_pos.x;
        m_pos.y = other.m_pos.y;
        m_pos.z = other.m_pos.z;
        m_rotation = other.m_rotation;
        int count = other.m_points.size();
        for (int i = 0; i < count; ++i)
        {
            fPoint* p = other.m_points.get(i);
            if (p)
                m_points.add(new fPoint(p->x, p->y, p->z));
        }
    }

    NPC::~NPC()
    {
    }

    short NPC::getImage(int state, short defImage) const
    {
        switch (state)
        {
        case 'x3__':
        case 'y3__':
        case 'unkn':
            return 18;
        case 'posX':
        case 'posY':
        case 'posZ':
        case 'rotX':
            return 130;
        }
        return defImage;
    }

    const wchar_t* NPC::getString(DWORD tag) const
    {
        if (tag != 'name')
            return 0;
        sub_43CBF0(g_textBuffer, L"#%d", m_index);
        return g_textBuffer;
    }

    int NPC::getInt(DWORD tag) const
    {
        switch (tag)
        {
        case 'rotX':
            return floatBits(m_rotation);
        case 'posX':
            return floatBits(m_pos.x);
        case 'posY':
            return floatBits(m_pos.y);
        case 'posZ':
            return floatBits(m_pos.z);
        case 'unkn':
            return m_points.size();
        case 'x3__':
            return m_npcType;
        case 'y3__':
            return m_npcId;
        }
        return 0;
    }

    void NPC::reset()
    {
        m_npcId = 0;
        m_npcType = 0;
        m_pos.x = m_pos.y = m_pos.z = 0;
        m_rotation = 0;
        m_points.deleteAll();
    }

    void NPC::read(iStream& in, DWORD fmt)
    {
        m_npcType = in.readInt();
        m_npcId = in.readInt();
        m_pos.read(in, 'file');
        m_rotation = in.readFloat();
        m_points.read(in, fmt);
    }

    void NPC::write(oStream& out, DWORD fmt) const
    {
        out << m_npcType << m_npcId;
        m_pos.write(out, 'file');
        out << m_rotation;
        out << m_points.m_count;
        for (int i = 0; i < m_points.m_count; ++i)
            m_points.m_items[i]->write(out, fmt);
    }

    bool NPC::toText(String& out, DWORD fmt) const
    {
        wchar_t sep = p04_separator(fmt);
        unsigned flags = out.m_flags;
        out.m_flags = flags | 0x11;
        out << (long)m_npcType << sep;
        out << (long)m_npcId << sep << L'(';
        appendFPoint(out, &m_pos) << L')' << sep;
        out << m_rotation << sep;
        out << (long)m_points.size();
        out.m_flags = flags;
        return true;
    }

    int NPC::getVersion() const
    {
        return 566;
    }

    NPCs::NPCs()
        : ptrArray<NPC*>()
        , ToggleNode(L"NPCs")
    {
    }

    NPCs::~NPCs()
    {
    }

    bool NPCs::copyListTo(oStream& out)
    {
        out << m_count;
        for (int i = 0; i < m_count; ++i)
            m_items[i]->write(out, 'clip');
        return true;
    }

    const wchar_t* NPCs::getString(DWORD tag) const
    {
        const wchar_t* result = L"NPCs";
        if (tag != 'name')
            return 0;
        return result;
    }

    void NPCs::read(iStream& in, DWORD fmt)
    {
        deleteAll();
        if (in.m_size - in.m_pos < 4)
            throw 0;
        int index = 0;
        for (int remaining = (int)in.readInt(); remaining > 0;)
        {
            NPC* npc = new NPC(index++);
            npc->read(in, fmt);
            add(npc);
            remaining += -1 - npc->m_points.size();
        }
    }

    void NPCs::write(oStream& out, DWORD fmt) const
    {
        int total = 0;
        for (int i = 0; i < m_count; ++i)
            total += m_items[i]->m_points.size() + 1;
        out << total;
        for (int i = 0; i < m_count; ++i)
            m_items[i]->write(out, fmt);
    }

    bool NPCs::toText(String& out, DWORD fmt) const
    {
        if (fmt == 'dump')
            out << "NPCs (" << (long)m_count << " items)" << L"\r\n";
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
