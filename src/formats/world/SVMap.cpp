#include "SVMap.h"
#include "String.hpp"
#include "iStream.hpp"
#include "oStream.hpp"
#include <cstring>

template <class T>
static void svmap_collectionText(const Collection<T>& list, String& out, DWORD fmt)
{
    for (int i = 0; i < list.m_count; ++i)
    {
        list.m_items[i]->toText(out, fmt);
        out.newline();
    }
}

SVMap::SVMap()
    : Serializable(),
      m_walkBitmap(),
      m_ladders(), m_monsterAreas(), m_npcs(), m_portals(), m_spawns(), m_namedAreas()
{
    m_unk14 = 0;
    m_mapSize = 0;
    m_unk1C = 0;
}

SVMap::~SVMap()
{
}

bool SVMap::isEmpty() const
{
    return m_mapSize == 0;
}

void SVMap::clear()
{
    m_mapSize = 0;
    m_unk1C = 0;
    aStream& bitmap = m_walkBitmap;
    if (bitmap.m_data && bitmap.m_capacity)
        memset(bitmap.m_data, 0, bitmap.m_capacity);
    bitmap.m_size = 0;
    bitmap.m_pos = 0;
    m_ladders.deleteAll();
    m_monsterAreas.deleteAll();
    m_npcs.deleteAll();
    m_portals.deleteAll();
    m_spawns.deleteAll();
    m_namedAreas.deleteAll();
}

void SVMap::read(iStream& s, DWORD tag)
{
    try
    {
        clear();
        m_mapSize = s.readInt();
        int size = MulDiv(m_mapSize, m_mapSize, 8);
        const BYTE* src = s.m_data + s.m_pos;
        aStream& bitmap = m_walkBitmap;
        if (src && size >= 1)
        {
            if (bitmap.m_capacity >= size || bitmap.grow(size - bitmap.m_capacity))
            {
                memset(bitmap.m_data, 0, bitmap.m_capacity);
                memcpy(bitmap.m_data, src, size);
                bitmap.m_size = size;
                bitmap.m_pos = 0;
            }
        }
        s.skip(size);
        m_unk1C = s.readInt();
        m_ladders.read(s, tag);
        m_monsterAreas.read(s, tag);
        m_npcs.read(s, tag);
        m_portals.read(s, tag);
        m_spawns.read(s, tag);
        m_namedAreas.read(s, tag);
    }
    catch (...)
    {
        throw;
    }
}

void SVMap::write(oStream& s, DWORD tag) const
{
    try
    {
        s << (unsigned long)m_mapSize;
        const aStream& bitmap = m_walkBitmap;
        s.put(bitmap.m_data, bitmap.m_size);
        s << (unsigned long)m_unk1C;
        m_ladders.write(s, tag);
        m_monsterAreas.write(s, tag);
        m_npcs.write(s, tag);
        m_portals.write(s, tag);
        m_spawns.write(s, tag);
        m_namedAreas.write(s, tag);
    }
    catch (...)
    {
        throw;
    }
}

bool SVMap::toText(String& out, DWORD fmt) const
{
    out << "mapsSize:\t" << (long)m_mapSize << L"\r\n";
    svmap_collectionText(m_ladders, out, fmt);
    svmap_collectionText(m_monsterAreas, out, fmt);
    svmap_collectionText(m_npcs, out, fmt);
    svmap_collectionText(m_portals, out, fmt);
    svmap_collectionText(m_spawns, out, fmt);
    svmap_collectionText(m_namedAreas, out, fmt);
    return true;
}

void SVMap::setMapSize(int size)
{
    int rounded = size;
    if ((size & 7) != 0)
        rounded = 8 * (size / 8) + 8;
    m_mapSize = rounded;
    aStream& bitmap = m_walkBitmap;
    if (bitmap.m_data && bitmap.m_capacity)
        memset(bitmap.m_data, 0, bitmap.m_capacity);
    bitmap.m_size = 0;
    bitmap.m_pos = 0;
    int bytes = MulDiv(m_mapSize, m_mapSize, 8);
    if (bytes > 0)
    {
        if (bytes + bitmap.m_size + 64 <= bitmap.m_capacity || bitmap.grow(bytes))
        {
            for (int n = bytes; n; --n)
            {
                bitmap.m_data[bitmap.m_size] = 0;
                ++bitmap.m_size;
            }
        }
    }
}
