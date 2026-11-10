#pragma once
#include "Spot.h"
#include "ptrArray.h"

class String;
class iStream;
class oStream;

class NpcLocation : public Spot
{
public:
    class Location : public Node
    {
    public:
        Location(int index);
        Location(const Location* src) { copyPos(src); m_index = src->m_index; }
        ~Location() override {}

        int  getInt(DWORD tag) const override;
        void reset() override;
        int  compare(const Node* other, DWORD tag) const override;
        virtual bool copyPos(const Location* other);

        void read(iStream& s, DWORD tag) override;
        void write(oStream& s, DWORD tag) const override;
        bool toText(String& out, DWORD fmt) const override;
        bool copyFrom(const Streamable* src) override;

        int   m_index;
        int   m_unk10;
        float m_x;
        float m_y;
        float m_z;
        float m_orient;
    };

    class Locations : public ptrArray<Location*>
    {
    public:
        Locations() : ptrArray<Location*>(16) {}
        void copyFrom(const Locations& src);
        bool toText(String& out, DWORD fmt) const;
        void read(iStream& s, DWORD tag);
        void write(oStream& s, DWORD tag) const;
    };

public:
    static const int kMinRecordSize = 12;
    NpcLocation(int id, int unused = 0);
    ~NpcLocation() override;

    const wchar_t* getString(DWORD tag) const override;
    int  getInt(DWORD tag) const override;
    void reset() override {}
    int  compare(const Node* other, DWORD tag) const override;
    virtual bool copyFrom(const NpcLocation* other);

    void read(iStream& s, DWORD tag) override;
    void write(oStream& s, DWORD tag) const override;
    bool toText(String& out, DWORD fmt) const override;
    bool copyFrom(const Streamable* src) override;

public:
    int       m_type;
    int       m_typeId;
    Locations m_locations;
};
static_assert(sizeof(NpcLocation) == 0x48, "NpcLocation size");
static_assert(sizeof(NpcLocation::Location) == 0x2C, "NpcLocation::Location size");
