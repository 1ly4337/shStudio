#pragma once
#include "p04_common.h"
#include "kWorld_Section.h"
#include "ptrArray.h"
#include "fPoint.h"
#include "iStream.h"
#include "oStream.h"

namespace kWorld
{
    class Element : public Section
    {
    public:
        class Coord;
        class Coords;
        class Item;
        class Items;

        Element(int sectionIndex, int mode);
        virtual ~Element() {}

        virtual short getImage(int state, short defImage) const override { return 0; }
        virtual bool hasItem(const wchar_t* name) override;
        virtual Node* addItem(const wchar_t* name) override;
        virtual bool removeItem(Node* item) override;

        virtual void read(iStream& in, DWORD fmt) override;
        virtual void write(oStream& out, DWORD fmt) const override;
        virtual bool toText(String& out, DWORD fmt) const override;

    public:
        class Items : public ptrArray<Item*>
        {
        public:
            Items(int mode, Element* owner) : ptrArray<Item*>(), m_mode(mode), m_owner(owner) {}
            virtual ~Items() {}
            bool hasItem(const wchar_t* name) const;
            void read(iStream& in, DWORD fmt);
            void write(oStream& out, DWORD fmt) const;

        public:
            int m_mode;
            Element* m_owner;
        };

        Items m_items;
    };

    class Element::Coords : public ptrArray<Element::Coord*>
    {
    public:
        Coords() : ptrArray<Element::Coord*>() {}
        virtual ~Coords() {}
        virtual bool add(Element::Coord* coord) override;
    };

    class Element::Item : public Section::wItem
    {
    public:
        Item(int index, Element* owner, int mode);
        virtual ~Item() {}

        virtual void reset() override;
        virtual int nodeV11(int hTree) override;
        virtual bool nodeV12(int hTree) override;
        virtual bool removeCoord(Element::Coord* coord);

    public:
        Coords m_coords;
        int m_mode;
    };

    class Element::Coord : public Node
    {
    public:
        explicit Coord(int mode);
        Coord(const Coord& other);
        virtual ~Coord() {}

        virtual short getImage(int tag, short defImage) const override;
        virtual const wchar_t* getString(DWORD tag) const override;
        virtual int getInt(DWORD tag) const override;
        virtual void reset() override;
        virtual int compare(const Node* other, DWORD tag) const override { return 0; }
        virtual void assignFrom(const Coord* src);

        virtual void read(iStream& in, DWORD fmt) override;
        virtual void write(oStream& out, DWORD fmt) const override;
        virtual bool toText(String& out, DWORD fmt) const override;
        virtual int getVersion() const override { return 572; }
        virtual bool copyFrom(const Streamable* src) override;

    public:
        int m_index;
        int m_mode;
        int m_unknown;
        int m_itemIndex;
        fPoint m_pos;
        fPoint m_rot;
        fPoint m_scale;
    };
}
