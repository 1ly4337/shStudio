#include "kWorld_Spawn.h"
#include "iStream.h"
#include "oStream.h"
#include "CapeData.h"

namespace kWorld
{

    Spawn::Spawn(int index)
        : Node()
        , m_pos()
        , m_rot()
    {
        m_index = index;
        m_x3 = 1;
        m_z3 = 0;
        m_y3 = 0;
        m_unkn = 0;
    }

    Spawn::~Spawn()
    {
    }

    short Spawn::getImage(int state, short defImage) const
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

    const wchar_t* Spawn::getString(DWORD tag) const
    {
        if (tag != 'name')
            return 0;
        sub_43CBF0(g_textBuffer, L"#%d", m_index);
        return g_textBuffer;
    }

    int Spawn::getInt(DWORD tag) const
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
        case 'unkn': return m_unkn;
        case 'z3__': return m_z3;
        }
        return 0;
    }

    void Spawn::reset()
    {
        m_z3 = 0;
        m_unkn = 0;
        m_y3 = 0;
        m_x3 = 0;
        m_rot.x = m_rot.y = m_rot.z = 0;
        m_pos.x = 0;
        m_pos.y = m_rot.y;
        m_pos.z = m_rot.z;
    }

    void Spawn::read(iStream& in, DWORD fmt)
    {
        m_x3 = in.readInt();
        m_pos.read(in, 'file');
        m_rot.read(in, 'file');
        m_y3 = in.readInt();
        m_unkn = in.readInt();
        m_z3 = in.readInt();
    }

    void Spawn::write(oStream& out, DWORD fmt) const
    {
        out << m_x3;
        m_pos.write(out, 'file');
        m_rot.write(out, 'file');
        out << m_y3 << m_unkn << m_z3;
    }

    int Spawn::getVersion() const
    {
        return 564;
    }

    Spawns::Spawns()
        : ptrArray<Spawn*>()
        , ToggleNode(L"Spawn Areas")
    {
    }

    Spawns::~Spawns()
    {
    }

    void Spawns::read(iStream& in, DWORD fmt)
    {
        deleteAll();
        int count = (int)in.readInt();
        for (int i = 0; i < count; ++i)
        {
            Spawn* spawn = new Spawn(i);
            spawn->read(in, fmt);
            add(spawn);
        }
    }

    void Spawns::write(oStream& out, DWORD fmt) const
    {
        out << m_count;
        for (int i = 0; i < m_count; ++i)
            m_items[i]->write(out, fmt);
    }

    bool Spawns::toText(String& out, DWORD fmt) const
    {
        for (int i = 0; i < m_count; ++i)
        {
            m_items[i]->toText(out, fmt);
            out.newline();
        }
        return true;
    }
}
