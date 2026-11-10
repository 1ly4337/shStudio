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
    class Portal : public CaptionNode
    {
    public:
        explicit Portal(int index);
        Portal(const Portal& other);
        virtual ~Portal() {}

        virtual short getImage(int state, short defImage) const override;
        virtual const wchar_t* getString(DWORD tag) const override;
        virtual int getInt(DWORD tag) const override;
        virtual void reset() override;
        virtual int compare(const Node* other, DWORD tag) const override { return 0; }
        virtual bool copyListTo(oStream& out);

        virtual void read(iStream& in, DWORD fmt) override;
        virtual void write(oStream& out, DWORD fmt) const override;
        virtual bool toText(String& out, DWORD fmt) const override;
        virtual int getVersion() const override;

    public:
        int           m_index;
        fPoint        m_pos;
        fPoint        m_rot;
        int           m_value70;
        unsigned char m_unkn;
        short         m_value76;
        unsigned char m_value78;
        fPoint        m_dest;
    };
    static_assert(sizeof(Portal) == 0x94, "kWorld::Portal size");

    class Portals : public ptrArray<Portal*>, public ToggleNode
    {
    public:
        Portals();
        virtual ~Portals();

        virtual bool copyListTo(oStream& out);

        virtual void reset() override { deleteAll(); }

        virtual void read(iStream& in, DWORD fmt) override;
        virtual void write(oStream& out, DWORD fmt) const override;
        virtual bool toText(String& out, DWORD fmt) const override;
        virtual int getVersion() const override { return 563; }
    };
    static_assert(sizeof(Portals) == 0x4C, "kWorld::Portals size");
}
