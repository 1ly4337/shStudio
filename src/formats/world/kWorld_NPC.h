#pragma once
#include "p04_common.h"
#include "kWorld_CaptionNode.h"
#include "kWorld_Section.h"
#include "ToggleNode.h"
#include "ptrArray.h"
#include "fPoint.h"
#include "fPoints.h"

class iStream;
class oStream;

namespace kWorld
{
    class NPC : public Node
    {
    public:
        explicit NPC(int index);
        NPC(const NPC& other);
        virtual ~NPC();

        virtual short getImage(int state, short defImage) const override;
        virtual const wchar_t* getString(DWORD tag) const override;
        virtual int getInt(DWORD tag) const override;
        virtual void reset() override;
        virtual int compare(const Node* other, DWORD tag) const override { return 0; }

        virtual void read(iStream& in, DWORD fmt) override;
        virtual void write(oStream& out, DWORD fmt) const override;
        virtual bool toText(String& out, DWORD fmt) const override;
        virtual int getVersion() const override;

    public:
        int     m_index;
        int     m_npcType;
        int     m_npcId;
        fPoint  m_pos;
        float   m_rotation;
        fPoints m_points;
    };
    static_assert(sizeof(NPC) == 0x4C, "kWorld::NPC size");

    class NPCs : public ptrArray<NPC*>, public ToggleNode
    {
    public:
        NPCs();
        virtual ~NPCs();

        virtual bool copyListTo(oStream& out);

        virtual const wchar_t* getString(DWORD tag) const override;
        virtual void reset() override { deleteAll(); }

        virtual void read(iStream& in, DWORD fmt) override;
        virtual void write(oStream& out, DWORD fmt) const override;
        virtual bool toText(String& out, DWORD fmt) const override;
        virtual int getVersion() const override { return 566; }
    };
    static_assert(sizeof(NPCs) == 0x4C, "kWorld::NPCs size");
}
