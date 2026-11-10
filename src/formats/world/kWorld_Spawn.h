#pragma once
#include "p04_common.h"
#include "kWorld_CaptionNode.h"
#include "kWorld_Section.h"
#include "ToggleNode.h"
#include "ptrArray.h"
#include "fPoint.h"

class iStream;
class oStream;

namespace kWorld
{
    class Spawn : public Node
    {
    public:
        explicit Spawn(int index);
        Spawn(const Spawn& other);
        virtual ~Spawn();

        virtual short getImage(int state, short defImage) const override;
        virtual const wchar_t* getString(DWORD tag) const override;
        virtual int getInt(DWORD tag) const override;
        virtual void reset() override;
        virtual int compare(const Node* other, DWORD tag) const override { return 0; }

        virtual void read(iStream& in, DWORD fmt) override;
        virtual void write(oStream& out, DWORD fmt) const override;
        virtual int getVersion() const override;

    public:
        int    m_index;
        int    m_x3;
        fPoint m_pos;
        fPoint m_rot;
        int    m_y3;
        int    m_unkn;
        int    m_z3;
    };
    static_assert(sizeof(Spawn) == 0x48, "kWorld::Spawn size");

    class Spawns : public ptrArray<Spawn*>, public ToggleNode
    {
    public:
        Spawns();
        virtual ~Spawns();

        virtual void reset() override { deleteAll(); }

        virtual void read(iStream& in, DWORD fmt) override;
        virtual void write(oStream& out, DWORD fmt) const override;
        virtual bool toText(String& out, DWORD fmt) const override;
        virtual int getVersion() const override { return 564; }
    };
    static_assert(sizeof(Spawns) == 0x4C, "kWorld::Spawns size");
}
