#pragma once
#include "p04_common.h"
#include "Serializable.h"
#include "String.hpp"
#include "ptrArray.h"
#include "kWorld_Items.h"
#include "MapHeight.h"

class iStream;
class oStream;

class World : public Serializable
{
public:
    class Field;
    class Dungeon;

    explicit World(int id);
    virtual ~World();

    virtual bool isEmpty() const override;
    virtual void clear() override;
    virtual void worldV7(int a, int b, int c) {}
    virtual void readFile(iStream& in, DWORD fmt, const wchar_t* fileName);
    virtual bool hasItem(int section, const wchar_t* name);

    virtual void read(iStream& in, DWORD fmt) override;
    virtual void write(oStream& out, DWORD fmt) const override;
    virtual bool toText(String& out, DWORD fmt) const override;

    kWorld::Items* getItems();

public:
    int m_id;
    Dungeon* m_dungeon;
    Field* m_field;
    String m_path;
};

class World::Field : public kWorld::Items
{
public:
    class TextSound
    {
    public:
        TextSound() : m_texture(), m_sound(), m_volume(0) {}

        String m_texture;
        String m_sound;
        float m_volume;
    };

    class TextSounds : public ptrArray<TextSound*>
    {
    public:
        TextSounds() : ptrArray<TextSound*>() {}
        virtual ~TextSounds() {}

        int read(iStream& in, DWORD fmt);
        void write(oStream& out, DWORD fmt) const;
        void toText(String& out) const;
        bool contains(const wchar_t* text) const;
    };

    explicit Field(const String& path);
    ~Field();

    virtual void clear() override;
    virtual void read(iStream& in, DWORD fmt) override;
    virtual void write(oStream& out, DWORD fmt) const override;
    virtual bool toText(String& out, DWORD fmt) const override;
    virtual bool hasItem(int section, const wchar_t* name) override;

public:
    int m_mapSize;
    MapHeight m_mapHeight;
    TextSounds m_textSounds;
};

class World::Dungeon : public kWorld::Items
{
public:
    explicit Dungeon(const String& path) : kWorld::Items(path, false) {}

    virtual bool toText(String& out, DWORD fmt) const override;
};

String& operator<<(String& out, const World::Field::TextSound& textSound);

static_assert(sizeof(World::Field::TextSound) == 0x44, "TextSound size");
static_assert(sizeof(World::Field) == 0x83C, "World::Field size");
static_assert(sizeof(World::Dungeon) == 0x6C0, "World::Dungeon size");
static_assert(sizeof(World) == 0x48, "World size");
