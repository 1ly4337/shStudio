#pragma once
#include <windows.h>
#include "Serializable.h"
#include "Bytes.h"
#include "Collection.hpp"
#include "Ladder.h"
#include "MonsterArea.h"
#include "NpcLocation.h"
#include "Portal.h"
#include "Spawn.h"
#include "NamedArea.h"

class String;

class SVMap : public Serializable
{
public:
    SVMap();
    ~SVMap() override;

    bool isEmpty() const override;
    void clear() override;
    virtual void vslot07(int a, int b, int c) {}

    void read(iStream& s, DWORD tag) override;
    void write(oStream& s, DWORD tag) const override;
    bool toText(String& out, DWORD fmt) const override;

    void setMapSize(int size);

public:
    int                     m_unk14;
    int                     m_mapSize;
    int                     m_unk1C;
    Bytes                   m_walkBitmap;
    Collection<Ladder>      m_ladders;
    Collection<MonsterArea> m_monsterAreas;
    Collection<NpcLocation> m_npcs;
    Collection<Portal>      m_portals;
    Collection<Spawn>       m_spawns;
    Collection<NamedArea>   m_namedAreas;
};
static_assert(sizeof(SVMap) == 0xE8, "SVMap size");
