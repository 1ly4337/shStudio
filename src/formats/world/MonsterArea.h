#pragma once
#include "Spot.h"
#include "ptrArray.h"

class String;
class iStream;
class oStream;

class MonsterArea : public Spot
{
public:
    class MonsterID : public Node
    {
    public:
        MonsterID();
        MonsterID(const MonsterID* src) : m_mobId(src->m_mobId), m_count(src->m_count) {}
        ~MonsterID() override {}

        const wchar_t* getString(DWORD tag) const override;
        int  getInt(DWORD tag) const override;
        bool setInt(DWORD tag, int value) override;
        void reset() override;
        int  compare(const Node* other, DWORD tag) const override;

        void read(iStream& s, DWORD tag) override;
        void write(oStream& s, DWORD tag) const override;
        bool toText(String& out, DWORD fmt) const override;
        bool copyFrom(const Streamable* src) override;

        int m_mobId;
        int m_count;
    };

    class MonsterIDs : public ptrArray<MonsterID*>
    {
    public:
        MonsterIDs() : ptrArray<MonsterID*>(16) {}
        void copyFrom(const MonsterIDs& src);
        bool toText(String& out, DWORD fmt) const;
        void read(iStream& s, DWORD tag);
        void write(oStream& s, DWORD tag) const;
    };

public:
    static const int kMinRecordSize = 28;
    MonsterArea(int id, int unused = 0);
    ~MonsterArea() override;

    int  getInt(DWORD tag) const override;
    void reset() override;

    void read(iStream& s, DWORD tag) override;
    void write(oStream& s, DWORD tag) const override;
    bool toText(String& out, DWORD fmt) const override;
    bool copyFrom(const Streamable* src) override;

public:
    float      m_x2;
    float      m_y2;
    float      m_z2;
    MonsterIDs m_ids;
};
static_assert(sizeof(MonsterArea) == 0x4C, "MonsterArea size");
static_assert(sizeof(MonsterArea::MonsterID) == 0x1C, "MonsterID size");
