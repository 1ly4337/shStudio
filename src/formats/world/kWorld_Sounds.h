#pragma once
#include "p04_common.h"
#include "kWorld_Section.h"
#include "ptrArray.h"
#include "primitiveArray.h"
#include "fPoint.h"
#include "iStream.h"
#include "oStream.h"

namespace kWorld
{
    class Sounds : public Section
    {
    public:
        class Item;
        class Items;
        class ZoneList;
        class ZoneArea;
        class SpotArea;

        explicit Sounds(int sectionIndex);
        virtual ~Sounds() {}

        virtual short getImage(int state, short defImage) const override { return 0; }
        virtual bool hasItem(const wchar_t* name) override;
        virtual Node* addItem(const wchar_t* name) override;
        virtual bool removeItem(Node* item) override;
        virtual void deleteAllItems();

        virtual void read(iStream& in, DWORD fmt) override;
        virtual void write(oStream& out, DWORD fmt) const override;
        virtual bool toText(String& out, DWORD fmt) const override { return true; }
        virtual int getVersion() const override { return 567; }

    public:
        class Items : public ptrArray<Item*>
        {
        public:
            Items(int type, Sounds* owner) : ptrArray<Item*>(), m_type(type), m_owner(owner), m_zoneLists() {}
            virtual ~Items() {}
            virtual void read(iStream& in, DWORD fmt);
            virtual void write(oStream& out, DWORD fmt) const;
            virtual bool hasItem(const wchar_t* name) const;
            virtual Item* addItem(const wchar_t* name);

        public:
            int m_type;
            Sounds* m_owner;
            ptrArray<ZoneList*> m_zoneLists;
        };

        Items m_items;
    };

    class Sounds::Item : public Section::wItem
    {
    public:
        Item(int index, Sounds* owner, int type);
        virtual ~Item() {}
        virtual void reset() override;

    public:
        ptrArray<ZoneArea*> m_zoneAreas;
        ptrArray<SpotArea*> m_spotAreas;
        int m_type;
    };

    class Sounds::ZoneList
    {
    public:
        explicit ZoneList(int index);

        virtual int getVersion() const { return 569; }
        virtual void read(iStream& in, DWORD fmt);
        virtual void write(oStream& out, DWORD fmt) const;
        virtual bool toText(String& out, DWORD fmt) const;

    public:
        int m_index;
        fPoint m_min;
        fPoint m_max;
        primitiveArray<long> m_ids;
    };

    class Sounds::ZoneArea : public Node
    {
    public:
        ZoneArea();
        ZoneArea(const ZoneArea& other);
        virtual ~ZoneArea() {}

        virtual short getImage(int tag, short defImage) const override;
        virtual const wchar_t* getString(DWORD tag) const override;
        virtual int getInt(DWORD tag) const override;
        virtual void reset() override;
        virtual int compare(const Node* other, DWORD tag) const override { return 0; }
        virtual void describe(String& out) const;
        virtual void assignFrom(const ZoneArea* src);

        virtual void read(iStream& in, DWORD fmt) override;
        virtual void write(oStream& out, DWORD fmt) const override;
        virtual int getVersion() const override { return 568; }
        virtual bool copyFrom(const Streamable* src) override;

    public:
        int m_index;
        fPoint m_pos;
        fPoint m_rot;
        float m_x3;
        int m_itemIndex;
        int m_unknown;
    };

    class Sounds::SpotArea : public Node
    {
    public:
        SpotArea();
        SpotArea(const SpotArea& other);
        virtual ~SpotArea() {}

        virtual short getImage(int tag, short defImage) const override;
        virtual const wchar_t* getString(DWORD tag) const override;
        virtual int getInt(DWORD tag) const override;
        virtual void reset() override;
        virtual int compare(const Node* other, DWORD tag) const override { return 0; }
        virtual void describe(String& out) const;
        virtual void assignFrom(const SpotArea* src);

        virtual void read(iStream& in, DWORD fmt) override;
        virtual void write(oStream& out, DWORD fmt) const override;
        virtual int getVersion() const override { return 570; }
        virtual bool copyFrom(const Streamable* src) override;

    public:
        int m_index;
        int m_itemIndex;
        fPoint m_pos;
        float m_radius;
    };
}
