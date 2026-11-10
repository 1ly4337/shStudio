#include "StringTable.h"
#include <new>

StringTable::Entry::Entry(long entryId, const wchar_t* entryText) : id(entryId)
{
    text = entryText;
}

StringTable::StringTable(int capacity, int growBy)
{
    m_capacity = capacity;
    m_growBy = growBy;
    m_count = 0;
    m_items = (Entry**)::operator new[](sizeof(Entry*) * (unsigned)capacity);
    for (int i = 0; i < m_capacity; ++i)
        m_items[i] = 0;
}

StringTable::~StringTable()
{
    deleteAll();
    ::operator delete[](m_items);
}

void StringTable::deleteEntry(Entry* e)
{
    delete e;
}

void StringTable::deleteAll()
{
    for (int i = 0; i < m_count; ++i)
    {
        Entry* e = m_items[i];
        if (e)
            delete e;
        m_items[i] = 0;
    }
    m_count = 0;
}
