#pragma once
#include "p04_common.h"
#include "kWorld_CaptionNode.h"
#include "kWorld_Section.h"
#include "ptrArray.h"
#include "fPoint.h"

class iStream;
class oStream;

namespace kWorld
{
    class Vertice : public Node
    {
    public:
        explicit Vertice(int index);
        Vertice(const Vertice& other);
        virtual ~Vertice();

        virtual short getImage(int state, short defImage) const override;
        virtual const wchar_t* getString(DWORD tag) const override;
        virtual int getInt(DWORD tag) const override;
        virtual void reset() override;
        virtual int compare(const Node* other, DWORD tag) const override { return 0; }

        virtual void read(iStream& in, DWORD fmt) override;
        virtual void write(oStream& out, DWORD fmt) const override;
        virtual bool toText(String& out, DWORD fmt) const override;
        virtual bool copyFrom(const Streamable* src) override;

    public:
        int    m_index;
        fPoint m_pos;
        fPoint m_rot;
        fPoint m_p3;
        int    m_unkn;
    };
    static_assert(sizeof(Vertice) == 0x4C, "kWorld::Vertice size");

    class Vertices : public ptrArray<Vertice*>, public Node
    {
    public:
        Vertices();
        virtual ~Vertices();

        virtual const wchar_t* getString(DWORD tag) const override;
        virtual int getInt(DWORD tag) const override { return tag == '1col'; }
        virtual void reset() override {}
        virtual int compare(const Node* other, DWORD tag) const override { return 0; }

        virtual void read(iStream& in, DWORD fmt) override;
        virtual void write(oStream& out, DWORD fmt) const override;
        virtual bool toText(String& out, DWORD fmt) const override;
    };
    static_assert(sizeof(Vertices) == 0x2C, "kWorld::Vertices size");
}
