#include "kWorld_Sounds.h"
#include "Exception.h"
#include "String_p02.h"
#include "CapeData.h"

namespace kWorld
{

    Sounds::Sounds(int sectionIndex)
        : Section(sectionIndex)
        , m_items(sectionIndex, this)
    {
    }

    bool Sounds::hasItem(const wchar_t* name)
    {
        return m_items.hasItem(name);
    }

    Node* Sounds::addItem(const wchar_t* name)
    {
        return m_items.addItem(name);
    }

    bool Sounds::removeItem(Node* node)
    {
        Item* item = dynamic_cast<Item*>(node);
        if (!m_items.detachItem(item))
            return false;
        m_items.destroyItem(item);
        return true;
    }

    void Sounds::deleteAllItems()
    {
        m_items.deleteAll();
    }

    void Sounds::read(iStream& in, DWORD fmt)
    {
        m_items.read(in, fmt);
    }

    void Sounds::write(oStream& out, DWORD fmt) const
    {
        m_items.write(out, fmt);
    }

    bool Sounds::Items::hasItem(const wchar_t* name) const
    {
        if (!name)
            return false;
        String lower;
        lower.append(name, (unsigned)-1);
        lower.toLowerInPlace();
        const wchar_t* text = lower.m_buffer;
        for (int i = 0; i < m_count; ++i)
        {
            if (text && FIELD(String, m_items[i], 0x14).equalsLowerCase(text, (unsigned)wcslen(text)))
                return true;
        }
        return false;
    }

    Sounds::Item* Sounds::Items::addItem(const wchar_t* name)
    {
        Item* item = new Item(m_count, m_owner, m_type);
        item->setString('name', name);
        add(item);
        return item;
    }

    void Sounds::Items::read(iStream& in, DWORD fmt)
    {
        deleteAll();
        if (in.m_size - in.m_pos < 4)
            throw 1;
        int count = (int)in.readInt();
        if (count < 0 || count > (in.m_size - in.m_pos) / 256)
            throw 2;
        for (int i = 0; i < count; ++i)
        {
            Item* item = new Item(i, m_owner, m_type);
            static_cast<Streamable*>(item)->read(in, fmt);
            add(item);
        }

        if (m_type == 9)
        {
            if (in.m_size - in.m_pos < 4)
                throw 0;
            int n = (int)in.readInt();
            if (n < 0 || n > (in.m_size - in.m_pos) / 36)
                throw 1;
            for (; n; --n)
            {
                ZoneArea* zone = new ZoneArea();
                static_cast<Streamable*>(zone)->read(in, fmt);
                int idx = zone->m_itemIndex;
                if (idx < 0 || idx >= m_count)
                {
                    delete zone;
                }
                else
                {
                    Item* item = m_items[idx];
                    item->m_zoneAreas.add(zone);
                    zone->m_index = item->m_zoneAreas.size();
                }
            }
        }
        else if (m_type == 10)
        {
            if (in.m_size - in.m_pos < 8)
                throw 0;
            int lists = (int)in.readInt();
            if (lists < 0 || lists > (in.m_size - in.m_pos) / 28)
                throw 1;
            for (int i = 0; i < lists; ++i)
            {
                ZoneList* list = new ZoneList(i);
                list->read(in, fmt);
                m_zoneLists.add(list);
            }

            int n = (int)in.readInt();
            if (n < 0 || n > (in.m_size - in.m_pos) / 20)
                throw 1;
            for (; n; --n)
            {
                SpotArea* spot = new SpotArea();
                static_cast<Streamable*>(spot)->read(in, fmt);
                int idx = spot->m_itemIndex;
                if (idx < 0 || idx >= m_count)
                {
                    delete spot;
                }
                else
                {
                    Item* item = m_items[idx];
                    item->m_spotAreas.add(spot);
                    spot->m_index = item->m_spotAreas.size();
                }
            }
        }
    }

    void Sounds::Items::write(oStream& out, DWORD fmt) const
    {
        int zoneTotal = 0;
        int spotTotal = 0;
        out << (unsigned long)m_count;
        for (int i = 0; i < m_count; ++i)
        {
            Item* item = m_items[i];
            static_cast<const Streamable*>(item)->write(out, fmt);
            zoneTotal += item->m_zoneAreas.size();
            spotTotal += item->m_spotAreas.size();
        }

        if (m_type == 9)
        {
            out << (unsigned long)zoneTotal;
            for (int i = 0; i < m_count; ++i)
            {
                ptrArray<ZoneArea*>& zones = m_items[i]->m_zoneAreas;
                int n = zones.size();
                for (int j = 0; j < n; ++j)
                {
                    ZoneArea* zone = zones.m_items[j];
                    zone->m_itemIndex = i;
                    static_cast<const Streamable*>(zone)->write(out, fmt);
                }
            }
        }
        else
        {
            int lists = m_zoneLists.size();
            out << (unsigned long)lists;
            for (int i = 0; i < lists; ++i)
                m_zoneLists.get(i)->write(out, fmt);

            out << (unsigned long)spotTotal;
            for (int i = 0; i < m_count; ++i)
            {
                ptrArray<SpotArea*>& spots = m_items[i]->m_spotAreas;
                int n = spots.size();
                for (int j = 0; j < n; ++j)
                {
                    SpotArea* spot = spots.m_items[j];
                    spot->m_itemIndex = i;
                    static_cast<const Streamable*>(spot)->write(out, fmt);
                }
            }
        }
    }

    Sounds::Item::Item(int index, Sounds* owner, int type)
        : Section::wItem(index, owner)
        , m_zoneAreas()
        , m_spotAreas()
        , m_type(type)
    {
    }

    void Sounds::Item::reset()
    {
        String& name = FIELD(String, this, 0x14);
        wchar_t* buf = name.m_buffer;
        name.m_length = 0;
        name.m_capacity = 0;
        if (buf)
        {
            operator delete[](buf);
            name.m_buffer = 0;
        }
        String& second = FIELD(String, this, 0x34);
        buf = second.m_buffer;
        second.m_length = 0;
        second.m_capacity = 0;
        if (buf)
        {
            operator delete[](buf);
            second.m_buffer = 0;
        }
        m_zoneAreas.deleteAll();
        m_spotAreas.deleteAll();
    }

    Sounds::ZoneList::ZoneList(int index)
        : m_min()
        , m_max()
        , m_ids()
    {
        m_index = index;
    }

    void Sounds::ZoneList::read(iStream& in, DWORD fmt)
    {
        m_min.read(in, 'file');
        m_max.read(in, 'file');
        int n = (int)in.readInt();
        for (int i = 0; i < n; ++i)
        {
            unsigned long id = in.peekInt();
            int next = in.m_pos + 4;
            if (next > in.m_size)
                throw ArrayIndexOutOfBoundsException(next, in.m_size);
            in.m_pos = next;
            m_ids.add((long)id);
        }
    }

    void Sounds::ZoneList::write(oStream& out, DWORD fmt) const
    {
        int n = m_ids.m_count;
        m_min.write(out, 'file');
        m_max.write(out, 'file');
        out << (unsigned long)n;
        for (int i = 0; i < n; ++i)
        {
            long id = (i < 0 || i >= m_ids.m_count) ? 0 : m_ids.m_items[i];
            out << (unsigned long)id;
        }
    }

    bool Sounds::ZoneList::toText(String& out, DWORD fmt) const
    {
        wchar_t sep = p04_separator(fmt);
        int n = m_ids.m_count;
        String& s1 = appendFPoint(out, &m_min);
        s1 << sep;
        String& s2 = appendFPoint(s1, &m_max);
        s2 << sep;
        s2 << (long)n;
        for (int i = 0; i < m_ids.m_count; ++i)
        {
            long id = (i < 0 || i >= m_ids.m_count) ? 0 : m_ids.m_items[i];
            out << sep;
            out << id;
        }
        return true;
    }

    Sounds::ZoneArea::ZoneArea()
        : Node()
        , m_pos()
        , m_rot()
    {
        m_index = -1;
        m_unknown = 0;
        m_itemIndex = 0;
        m_pos.z = 0;
        m_pos.y = 0;
        m_pos.x = 0;
        m_rot.z = 0;
        m_rot.y = 0;
        m_rot.x = 0;
        m_x3 = 0;
    }

    short Sounds::ZoneArea::getImage(int tag, short defImage) const
    {
        if (tag > 'unkn')
        {
            if (tag == 'x3__')
                return 130;
        }
        else
        {
            if (tag == 'unkn')
                return 18;
            if (tag <= 'rotZ' && (tag >= 'rotX' || (tag >= 'posX' && tag <= 'posZ')))
                return 130;
        }
        return defImage;
    }

    const wchar_t* Sounds::ZoneArea::getString(DWORD tag) const
    {
        if (tag != 'name')
            return 0;
        sub_43CBF0(g_textBuffer, (wchar_t*)L"#%d", m_index);
        return g_textBuffer;
    }

    int Sounds::ZoneArea::getInt(DWORD tag) const
    {
        switch (tag)
        {
        case 'indx': return m_index;
        case 'idnt': return m_itemIndex;
        case 'posX': return *(const int*)&m_pos.x;
        case 'posY': return *(const int*)&m_pos.y;
        case 'posZ': return *(const int*)&m_pos.z;
        case 'rotX': return *(const int*)&m_rot.x;
        case 'rotY': return *(const int*)&m_rot.y;
        case 'rotZ': return *(const int*)&m_rot.z;
        case 'unkn': return m_unknown;
        case 'x3__': return *(const int*)&m_x3;
        }
        return 0;
    }

    void Sounds::ZoneArea::reset()
    {
        m_unknown = 0;
        m_itemIndex = 0;
        m_pos.z = 0;
        m_pos.y = 0;
        m_pos.x = 0;
        m_rot.z = 0;
        m_rot.y = 0;
        m_rot.x = 0;
        m_x3 = 0;
    }

    void Sounds::ZoneArea::describe(String& out) const
    {
        String& s1 = appendFPoint(out, &m_pos);
        s1 << L'\t';
        String& s2 = appendFPoint(s1, &m_rot);
        s2 << L'\t';
        String& s3 = s2 << m_x3;
        s3 << L'\t';
        String& s4 = s3 << (long)m_itemIndex;
        s4 << L'\t';
        s4 << (long)m_unknown;
    }

    void Sounds::ZoneArea::assignFrom(const ZoneArea* src)
    {
        m_itemIndex = src->m_itemIndex;
        m_pos.x = src->m_pos.x;
        m_pos.y = src->m_pos.y;
        m_pos.z = src->m_pos.z;
        m_rot.x = src->m_rot.x;
        m_rot.y = src->m_rot.y;
        m_rot.z = src->m_rot.z;
        m_x3 = src->m_x3;
        m_unknown = src->m_unknown;
    }

    void Sounds::ZoneArea::read(iStream& in, DWORD fmt)
    {
        m_pos.read(in, 'file');
        m_rot.read(in, 'file');
        m_x3 = in.readFloat();
        m_itemIndex = (int)in.readInt();
        m_unknown = (int)in.readInt();
    }

    void Sounds::ZoneArea::write(oStream& out, DWORD fmt) const
    {
        m_pos.write(out, 'file');
        m_rot.write(out, 'file');
        oStream& o = out << m_x3;
        o << (unsigned long)m_itemIndex << (unsigned long)m_unknown;
    }

    bool Sounds::ZoneArea::copyFrom(const Streamable* src)
    {
        const ZoneArea* zone = dynamic_cast<const ZoneArea*>(src);
        if (!zone)
            return false;
        assignFrom(zone);
        return true;
    }

    Sounds::SpotArea::SpotArea()
        : Node()
        , m_pos()
    {
        m_index = -1;
        m_itemIndex = 0;
        m_pos.z = 0;
        m_pos.y = 0;
        m_pos.x = 0;
        m_radius = 0;
    }

    short Sounds::SpotArea::getImage(int tag, short defImage) const
    {
        if (tag >= 'posX' && (tag <= 'posZ' || tag == 'rotX'))
            return 130;
        return defImage;
    }

    const wchar_t* Sounds::SpotArea::getString(DWORD tag) const
    {
        if (tag != 'name')
            return 0;
        sub_43CBF0(g_textBuffer, (wchar_t*)L"#%d", m_index);
        return g_textBuffer;
    }

    int Sounds::SpotArea::getInt(DWORD tag) const
    {
        switch (tag)
        {
        case 'posY': return *(const int*)&m_pos.y;
        case 'idnt': return m_itemIndex;
        case 'indx': return m_index;
        case 'posX': return *(const int*)&m_pos.x;
        case 'posZ': return *(const int*)&m_pos.z;
        case 'rotX': return *(const int*)&m_radius;
        }
        return 0;
    }

    void Sounds::SpotArea::reset()
    {
        m_itemIndex = 0;
        m_pos.z = 0;
        m_pos.y = 0;
        m_pos.x = 0;
        m_radius = 0;
    }

    void Sounds::SpotArea::describe(String& out) const
    {
        String& s1 = out << (long)m_itemIndex;
        s1 << L'\t';
        String& s2 = appendFPoint(s1, &m_pos);
        s2 << L'\t';
        s2 << m_radius;
    }

    void Sounds::SpotArea::assignFrom(const SpotArea* src)
    {
        m_itemIndex = src->m_itemIndex;
        m_pos.x = src->m_pos.x;
        m_pos.y = src->m_pos.y;
        m_pos.z = src->m_pos.z;
        m_radius = src->m_radius;
    }

    void Sounds::SpotArea::read(iStream& in, DWORD fmt)
    {
        m_itemIndex = (int)in.readInt();
        m_pos.read(in, 'file');
        m_radius = in.readFloat();
    }

    void Sounds::SpotArea::write(oStream& out, DWORD fmt) const
    {
        oStream& o = out << (unsigned long)m_itemIndex;
        m_pos.write(o, 'file');
        o << m_radius;
    }

    bool Sounds::SpotArea::copyFrom(const Streamable* src)
    {
        const SpotArea* spot = dynamic_cast<const SpotArea*>(src);
        if (!spot)
            return false;
        assignFrom(spot);
        return true;
    }
}
