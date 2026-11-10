#pragma once
#include "String.hpp"

class StringTable
{
public:
    struct Entry
    {
        Entry(long entryId, const wchar_t* entryText);
        long   id;
        String text;
    };
    static_assert(sizeof(Entry) == 0x24, "StringTable::Entry size");

    StringTable(int capacity = 32, int growBy = 32);
    virtual ~StringTable();

    void deleteAll();
    static void deleteEntry(Entry* e);
    bool StringTable::add(Entry* entry);
    bool StringTable::put(int id, const wchar_t* text);
    bool grow(int newCapacity);

    Entry** m_items;
    int     m_count;
    int     m_capacity;
    int     m_growBy;
};
static_assert(sizeof(StringTable) == 0x14, "StringTable size");
