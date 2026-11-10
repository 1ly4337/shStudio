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
    class NamedArea : public CaptionNode
    {
    public:
        explicit NamedArea(int index);
        NamedArea(const NamedArea& other);
        virtual ~NamedArea() {}

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
        int    m_index;
        fPoint m_pos;
        fPoint m_rot;
        int    m_x3;
        int    m_y3;
        int    m_z3;
    };
    static_assert(sizeof(NamedArea) == 0x84, "kWorld::NamedArea size");

    class NamedAreas : public ptrArray<NamedArea*>, public ToggleNode
    {
    public:
        NamedAreas();
        virtual ~NamedAreas();

        virtual const wchar_t* getString(DWORD tag) const override;
        virtual void reset() override { deleteAll(); }

        virtual void read(iStream& in, DWORD fmt) override;
        virtual void write(oStream& out, DWORD fmt) const override;
        virtual bool toText(String& out, DWORD fmt) const override;
        virtual int getVersion() const override { return 565; }
    };
    static_assert(sizeof(NamedAreas) == 0x4C, "kWorld::NamedAreas size");
}
