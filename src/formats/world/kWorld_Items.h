#pragma once
#include "p04_common.h"
#include "String.hpp"
#include "Bytes.h"
#include "Name.h"
#include "kWorld_Element.h"
#include "kWorld_Sounds.h"
#include "kWorld_Vertice.h"
#include "kWorld_Portal.h"
#include "kWorld_Spawn.h"
#include "kWorld_NamedArea.h"
#include "kWorld_NPC.h"

class iStream;
class oStream;

namespace kWorld
{
    class Items
    {
    public:
        struct WeatherInfo
        {
            WeatherInfo();
            ~WeatherInfo();
            void clear();

            String m_name1;
            String m_name2;
            String m_name3;
        };

        Items(const String& path, bool hasWeather);
        ~Items();

        virtual void clear();
        virtual void read(iStream& in, DWORD fmt);
        virtual void write(oStream& out, DWORD fmt) const;
        virtual bool toText(String& out, DWORD fmt) const;
        virtual bool hasItem(int section, const wchar_t* name);

    public:
        String m_path;
        bool m_hasWeather;
        String m_inner;
        Element m_building;
        Element m_shape;
        Element m_tree;
        Element m_grass;
        Element m_vani;
        Element m_vani2;
        Element m_dungeonSection;
        Element m_mani;
        Name m_effects;
        Vertices m_vertices;
        int m_reserved[3];
        Element m_object;
        Sounds m_musics;
        Sounds m_sounds;
        Bytes m_rawTail;
        Portals m_portals;
        Spawns m_spawns;
        NamedAreas m_namedAreas;
        NPCs m_npcs;
        WeatherInfo m_weather;
        float m_values[11];
    };
    static_assert(sizeof(Items::WeatherInfo) == 0x60, "Items::WeatherInfo size");
    static_assert(sizeof(Items) == 0x6C0, "kWorld::Items size");

    String& operator<<(String& out, const Items::WeatherInfo& weather);
}
