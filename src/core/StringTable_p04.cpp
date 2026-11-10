#include <windows.h>
#include <cstring>
#include <new>
#include "StringTable.h"

bool StringTable::grow(int newCapacity)
{
    if (newCapacity < m_capacity)
        return false;
    Entry** items = (Entry**)::operator new[](((unsigned __int64)(unsigned)newCapacity >> 30) != 0
                                                  ? (size_t)-1 : sizeof(Entry*) * (unsigned)newCapacity);
    if (!items)
        return false;
    if (m_items)
    {
        for (int i = 0; i < m_count; ++i)
        {
            items[i] = m_items[i];
            m_items[i] = 0;
        }
    }
    if (m_count < newCapacity)
        memset(items + m_count, 0, sizeof(Entry*) * (newCapacity - m_count));
    ::operator delete[](m_items);
    m_items = items;
    m_capacity = newCapacity;
    return true;
}

bool StringTable::StringTable::add(Entry* entry)
{
    if (!entry)
        return false;
    for (int i = 0; i < m_count; ++i)
    {
        Entry* e = m_items[i];
        if (e && e->id == entry->id)
        {
            e->text = entry->text;
            if (entry != m_items[i])
                deleteEntry(entry);
            return true;
        }
    }
    if (m_count == m_capacity)
    {
        if (!m_growBy || !grow(m_capacity + m_growBy))
        {
            deleteEntry(entry);
            return false;
        }
    }
    m_items[m_count++] = entry;
    return true;
}

bool StringTable::StringTable::put(int id, const wchar_t* text)
{
    if (id < 0)
        return false;
    Entry* entry = new Entry(id, text);
    return StringTable::add(entry);
}
