#include "kWorld_Items.h"
#include "iStream.h"
#include "oStream.h"
#include "Name.h"

namespace kWorld
{
    Items::WeatherInfo::WeatherInfo()
        : m_name1()
        , m_name2()
        , m_name3()
    {
        clear();
    }

    Items::WeatherInfo::~WeatherInfo()
    {
    }

    void Items::WeatherInfo::clear()
    {
        m_name1.clear();
        m_name2.clear();
        m_name3.clear();
    }

    String& operator<<(String& out, const Items::WeatherInfo& weather)
    {
        out.append(weather.m_name1.m_buffer, weather.m_name1.m_length) << L"\r\n";
        out.append(weather.m_name2.m_buffer, weather.m_name2.m_length) << L"\r\n";
        out.append(weather.m_name3.m_buffer, weather.m_name3.m_length) << L"\r\n";
        return out;
    }

    Items::Items(const String& path, bool hasWeather)
        : m_path()
        , m_inner()
        , m_building(0, 1)
        , m_shape(1, 1)
        , m_tree(2, 1)
        , m_grass(3, 1)
        , m_vani(4, 1)
        , m_vani2(5, 1)
        , m_dungeonSection(6, 1)
        , m_mani(7, 2)
        , m_effects(11, (int)(INT_PTR)this)
        , m_vertices()
        , m_object(8, 1)
        , m_musics(9)
        , m_sounds(10)
        , m_rawTail()
        , m_portals()
        , m_spawns()
        , m_namedAreas()
        , m_npcs()
        , m_weather()
    {
        if (&path != &m_path)
            m_path = path;
        m_hasWeather = hasWeather;
    }

    Items::~Items()
    {
    }

    void Items::clear()
    {
        m_inner.clear();
        m_building.m_items.deleteAll();
        m_shape.m_items.deleteAll();
        m_tree.m_items.deleteAll();
        m_grass.m_items.deleteAll();
        m_vani.m_items.deleteAll();
        m_vani2.m_items.deleteAll();
        m_dungeonSection.m_items.deleteAll();
        m_mani.m_items.deleteAll();
        m_effects.Name::clearAll();
        m_vertices.deleteAll();
        m_object.m_items.deleteAll();
        m_musics.deleteAllItems();
        m_sounds.deleteAllItems();
        m_rawTail.wipe();
        m_portals.deleteAll();
        m_spawns.deleteAll();
        m_namedAreas.deleteAll();
        m_npcs.deleteAll();
        m_weather.clear();
        memset(m_values, 0, sizeof(m_values));
    }

    void Items::read(iStream& in, DWORD fmt)
    {
        if (!readFixedString256(in, m_inner, 0))
            throw 0;
        m_building.read(in, fmt);
        m_shape.read(in, fmt);
        m_tree.read(in, fmt);
        m_grass.read(in, fmt);
        m_vani.read(in, fmt);
        m_vani2.read(in, fmt);
        m_dungeonSection.read(in, fmt);
        m_mani.read(in, fmt);
        m_effects.read(in, fmt);
        m_vertices.read(in, fmt);
        m_reserved[0] = in.readInt();
        m_reserved[1] = in.readInt();
        m_reserved[2] = in.readInt();
        if (m_reserved[0] || m_reserved[1] || m_reserved[2])
            throw 2;
        m_object.read(in, fmt);
        m_musics.read(in, fmt);
        m_sounds.read(in, fmt);
        if (in.readInt())
        {
            if (in.m_pos >= 4)
                in.m_pos -= 4;
            in.read(static_cast<aStream&>(m_rawTail), in.m_size - in.m_pos - 44);
        }
        else
        {
            m_portals.read(in, fmt);
            m_spawns.read(in, fmt);
            m_namedAreas.read(in, fmt);
            m_npcs.read(in, fmt);
        }
        if (m_hasWeather)
        {
            readFixedString256(in, m_weather.m_name1, 0);
            readFixedString256(in, m_weather.m_name2, 0);
            readFixedString256(in, m_weather.m_name3, 0);
        }
        for (int i = 0; i < 11; ++i)
            m_values[i] = in.readFloat();
    }

    void Items::write(oStream& out, DWORD fmt) const
    {
        writeFixedString256(out, m_inner, 0);
        m_building.write(out, fmt);
        m_shape.write(out, fmt);
        m_tree.write(out, fmt);
        m_grass.write(out, fmt);
        m_vani.write(out, fmt);
        m_vani2.write(out, fmt);
        m_dungeonSection.write(out, fmt);
        m_mani.write(out, fmt);
        m_effects.write(out, fmt);
        m_vertices.write(out, fmt);
        out << (unsigned long)m_reserved[0] << (unsigned long)m_reserved[1] << (unsigned long)m_reserved[2];
        m_object.write(out, fmt);
        m_musics.write(out, fmt);
        m_sounds.write(out, fmt);
        if (m_rawTail.m_size <= 0)
        {
            out << (unsigned long)0;
            m_portals.write(out, fmt);
            m_spawns.write(out, fmt);
            m_namedAreas.write(out, fmt);
            m_npcs.write(out, fmt);
        }
        else
        {
            out.put(m_rawTail.m_data + m_rawTail.m_pos, m_rawTail.m_size);
        }
        if (m_hasWeather)
        {
            writeFixedString256(out, m_weather.m_name1, 0);
            writeFixedString256(out, m_weather.m_name2, 0);
            writeFixedString256(out, m_weather.m_name3, 0);
        }
        for (int i = 0; i < 11; ++i)
            out << m_values[i];
    }

    bool Items::toText(String& out, DWORD fmt) const
    {
        wchar_t sep = p04_separator((int)fmt);

        m_building.toText(out, fmt);
        out << L"\r\n";
        m_shape.toText(out, fmt);
        out << L"\r\n";
        m_tree.toText(out, fmt);
        out << L"\r\n";
        m_grass.toText(out, fmt);
        out << L"\r\n";
        m_vani.toText(out, fmt);
        out << L"\r\n";
        m_vani2.toText(out, fmt);
        out << L"\r\n";
        m_dungeonSection.toText(out, fmt);
        out << L"\r\n";
        m_mani.toText(out, fmt);
        out << L"\r\n";
        out << L"Effects File" << L"\r\n" << L'\t' << m_effects.getString('name') << L"\r\n";
        m_vertices.toText(out, 'dump');
        out << L"\r\n";
        m_object.toText(out, 'dump');
        out << L"\r\n";
        out << L"Sounds (Sound/Music)" << L"\r\n";
        m_musics.toText(out, 'dump');
        out << L"\r\n";
        out << L"Sounds.Bak (Sound/Back)" << L"\r\n";
        m_sounds.toText(out, 'dump');
        out << L"\r\n";
        m_portals.toText(out, fmt);
        out << L"\r\n";
        String& spawnsOut = out << L"Spawns" << L"\r\n";
        m_spawns.toText(spawnsOut, '----');
        spawnsOut << L"\r\n";
        m_namedAreas.toText(out, fmt);
        out << L"\r\n";
        m_npcs.toText(out, fmt);
        out << L"\r\n";
        if (m_hasWeather)
            out << L"Weather" << L"\r\n" << m_weather << L"\r\n";
        out << m_values[0] << sep << m_values[1] << sep << m_values[2] << L"\r\n";
        out << m_values[3] << sep << m_values[4] << sep << m_values[5] << L"\r\n";
        out << m_values[6] << sep << m_values[7] << sep << m_values[8] << L"\r\n";
        out << m_values[9] << sep << m_values[10] << L"\r\n";
        return true;
    }

    bool Items::hasItem(int section, const wchar_t* name)
    {
        switch (section)
        {
        case 6:
            return m_dungeonSection.hasItem(name);
        case 0:
            return m_building.hasItem(name);
        case 1:
            return m_shape.hasItem(name);
        case 3:
            return m_grass.hasItem(name);
        case 2:
            return m_tree.hasItem(name);
        case 8:
            return m_object.hasItem(name);
        }
        if ((section == 4 || section == 5 || section == 7)
            && (m_vani.hasItem(name) || m_vani2.hasItem(name) || m_mani.hasItem(name)))
        {
            return true;
        }
        Sounds* sounds;
        if (section == 9)
            sounds = &m_musics;
        else
        {
            if (section != 10)
                return false;
            sounds = &m_sounds;
        }
        return sounds->hasItem(name);
    }
}
