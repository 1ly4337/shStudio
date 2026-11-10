#include "World.h"
#include <cstdlib>
#include <cwchar>
#include "iStream.h"
#include "oStream.h"
#include "Bytes.h"
#include "Dungeon.h"
#include "Names.h"
#include "Name.h"

World::World(int id)
    : Serializable()
    , m_path()
{
    m_id = id;
    m_dungeon = 0;
    m_field = 0;
}

World::~World()
{
    if (m_dungeon)
    {
        delete m_dungeon;
        m_dungeon = 0;
    }
    if (m_field)
    {
        delete m_field;
        m_field = 0;
    }
}

bool World::isEmpty() const
{
    return !m_dungeon && !m_field;
}

void World::clear()
{
    if (m_dungeon)
    {
        delete m_dungeon;
        m_dungeon = 0;
    }
    if (m_field)
    {
        delete m_field;
        m_field = 0;
    }
}

kWorld::Items* World::getItems()
{
    kWorld::Items* items = m_dungeon;
    if (!items)
    {
        items = m_field;
        if (!items)
            throw 0;
    }
    return items;
}

void World::readFile(iStream& in, DWORD fmt, const wchar_t* fileName)
{
    m_path = fileName;
    m_path.removeFileName();
    read(in, fmt);
}

void World::read(iStream& in, DWORD fmt)
{
    clear();
    unsigned long tag = _byteswap_ulong(in.readInt());
    if (tag == 0x44554E00)
    {
        Dungeon* dungeon = new Dungeon(m_path);
        m_dungeon = dungeon;
        dungeon->read(in, fmt);
        return;
    }
    if (tag != 0x464C4400)
        throw 0;
    Field* field = new Field(m_path);
    m_field = field;
    field->m_mapHeight.m_mapId = m_id;
    m_field->read(in, fmt);
}

void World::write(oStream& out, DWORD fmt) const
{
    if (m_dungeon)
    {
        out << (unsigned long)_byteswap_ulong(0x44554E00);
        m_dungeon->write(out, fmt);
    }
    else if (m_field)
    {
        out << (unsigned long)_byteswap_ulong(0x464C4400);
        m_field->write(out, fmt);
    }
}

bool World::toText(String& out, DWORD fmt) const
{
    out.clear();
    if (m_dungeon)
        return m_dungeon->toText(out, fmt);
    if (m_field)
        return m_field->toText(out, fmt);
    return false;
}

bool World::hasItem(int section, const wchar_t* name)
{
    if (m_dungeon)
        return m_dungeon->hasItem(section, name);
    if (m_field)
        return m_field->hasItem(section, name);
    return false;
}

World::Field::Field(const String& path)
    : kWorld::Items(path, true)
    , m_mapHeight(this)
    , m_textSounds()
{
    m_mapSize = 0;
}

World::Field::~Field()
{
}

void World::Field::clear()
{
    m_mapSize = 0;
    m_mapHeight.clear();
    kWorld::Items::clear();
}

void World::Field::read(iStream& in, DWORD fmt)
{
    int mapSize = (int)in.readInt();
    m_mapSize = mapSize;
    if (mapSize != 1024 && mapSize != 2048)
        throw 1;
    m_mapHeight.init(mapSize);
    m_mapHeight.read(in, fmt);
    m_textSounds.read(in, fmt);
    int count = m_textSounds.size();
    for (int i = 0; i < count; ++i)
    {
        TextSound* textSound = m_textSounds.get(i);
        m_mapHeight.m_strings.add((long)i, textSound->m_texture.m_buffer);
    }
    kWorld::Items::read(in, fmt);
}

void World::Field::write(oStream& out, DWORD fmt) const
{
    out << (unsigned long)m_mapSize;
    m_mapHeight.write(out, fmt);
    m_textSounds.write(out, fmt);
    kWorld::Items::write(out, fmt);
}

bool World::Field::toText(String& out, DWORD fmt) const
{
    out << "Size:" << L'\t' << (long)m_mapSize << L"\r\n";
    m_textSounds.toText(out);
    (out << "Inner: ").append(m_inner.m_buffer, m_inner.m_length) << L"\r\n" << L"\r\n";
    return kWorld::Items::toText(out, fmt);
}

bool World::Field::hasItem(int section, const wchar_t* name)
{
    if (section == 9 && m_textSounds.contains(name))
        return true;
    return kWorld::Items::hasItem(section, name);
}

String& operator<<(String& out, const World::Field::TextSound& textSound)
{
    if (textSound.m_texture.m_length)
        out.append(textSound.m_texture.m_buffer, textSound.m_texture.m_length);
    else
        out << "nullStr";
    out << L'\t';
    out << textSound.m_volume << L'\t';
    if (textSound.m_sound.m_length)
        out.append(textSound.m_sound.m_buffer, textSound.m_sound.m_length);
    else
        out << "nullStr";
    return out;
}

int World::Field::TextSounds::read(iStream& in, DWORD fmt)
{
    if (in.m_size - in.m_pos < 4)
        throw 0;
    int count = (int)in.readInt();
    for (int i = 0; i < count; ++i)
    {
        TextSound* textSound = new TextSound;
        if (in.m_size - in.m_pos < 516)
            throw 0;
        readFixedString256(in, textSound->m_texture, 0);
        textSound->m_volume = in.readFloat();
        readFixedString256(in, textSound->m_sound, 0);
        add(textSound);
    }
    return count;
}

void World::Field::TextSounds::write(oStream& out, DWORD fmt) const
{
    out << (unsigned long)m_count;
    for (int i = 0; i < m_count; ++i)
    {
        TextSound* textSound = m_items[i];
        writeFixedString256(out, textSound->m_texture, 0);
        out << textSound->m_volume;
        writeFixedString256(out, textSound->m_sound, 0);
    }
}

void World::Field::TextSounds::toText(String& out) const
{
    out << "Texture - Sounds records" << L"\r\n";
    for (int i = 0; i < m_count; ++i)
    {
        TextSound* textSound = m_items[i];
        out << L'#' << (long)i << L'\t' << *textSound << L"\r\n";
    }
    out.newline();
}

bool World::Field::TextSounds::contains(const wchar_t* text) const
{
    if (m_count <= 0)
        return false;
    for (int i = 0; i < m_count; ++i)
    {
        TextSound* textSound = m_items[i];
        if (text && wcslen(text) <= textSound->m_texture.m_length)
        {
            String needle(text);
            String haystack(textSound->m_texture);
            for (unsigned k = 0; k < needle.m_length; ++k)
                if ((unsigned short)(needle.m_buffer[k] - L'A') <= 25)
                    needle.m_buffer[k] += 32;
            for (unsigned k = 0; k < haystack.m_length; ++k)
                if ((unsigned short)(haystack.m_buffer[k] - L'A') <= 25)
                    haystack.m_buffer[k] += 32;
            bool found = wcsstr(haystack.m_buffer, needle.m_buffer) != 0;
            if (found)
                return true;
        }
        if (m_items[i]->m_sound.containsNoCase(text))
            return true;
    }
    return false;
}

bool World::Dungeon::toText(String& out, DWORD fmt) const
{
    (out << "Dungeon: ").append(m_inner.m_buffer, m_inner.m_length) << L"\r\n" << L"\r\n";
    if (m_inner.m_length)
    {
        String fileName(m_path);
        (fileName << "Dungeon\\").append(m_inner.m_buffer, m_inner.m_length);
        Bytes bytes;
        if (!bytes.aStream::load(fileName.m_buffer))
        {
            try
            {
                ::Dungeon dungeon;
                dungeon.read(bytes, 'file');
                FIELD(Names, &dungeon, 0x7C).toText(out, 'resf');
                out << L"\r\n";
            }
            catch (...)
            {
            }
        }
        out << L"\r\n";
    }
    return kWorld::Items::toText(out, fmt);
}
