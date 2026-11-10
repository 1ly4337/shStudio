#pragma once
#include <windows.h>
#include <cstring>
#include "Exception.h"

template <class K, class V>
class HTable
{
public:
    struct Entry
    {
        K key;
        V value;
    };

    HTable(int capacity, int growBy)
    {
        m_capacity = capacity;
        m_growBy = growBy;
        m_count = 0;
        m_table = (Entry*)operator new[]((unsigned __int64)(unsigned int)capacity >> 29 != 0
                                             ? (size_t)-1
                                             : 8 * (size_t)capacity);
        for (int i = 0; i < m_capacity; ++i)
        {
            m_table[i].key = (K)-1;
            m_table[i].value = (V)0;
        }
    }

    virtual ~HTable()
    {
        for (int i = 0; i < m_count; ++i)
            clearEntry(&m_table[i]);
        m_count = 0;
        operator delete[](m_table);
    }

    virtual Entry* set(K key, V value)
    {
        if ((long)key < 0)
            return 0;
        int count = m_count;
        int i = 0;
        if (count > 0)
        {
            Entry* e = m_table;
            while (e->key != key)
            {
                ++i;
                ++e;
                if (i >= count)
                    goto append;
            }
            if (i != -1)
            {
                m_table[i].value = value;
                return &m_table[i];
            }
        }
    append:
        if (count == m_capacity && (m_growBy == 0 || !grow(m_capacity + m_growBy)))
            return 0;
        m_table[m_count].key = key;
        m_table[m_count++].value = value;
        return &m_table[m_count];
    }

    virtual Entry* update(K key, V value)
    {
        if ((long)key < 0)
            return 0;
        int count = m_count;
        int i = 0;
        if (count <= 0)
            return 0;
        for (Entry* e = m_table; e->key != key; ++e)
        {
            if (++i >= count)
                return 0;
        }
        if (i < 0)
            return 0;
        m_table[i].value = value;
        return &m_table[i];
    }

    virtual Entry* clearEntry(Entry* e)
    {
        e->key = (K)-1;
        e->value = (V)0;
        return e;
    }

    void removeAll()
    {
        for (int i = 0; i < m_count; ++i)
            clearEntry(&m_table[i]);
        m_count = 0;
    }
    V get(K key, V def) const
    {
        for (int i = 0; i < m_count; ++i)
            if (m_table[i].key == key)
                return m_table[i].value;
        return def;
    }
    K keyAt(int index) const
    {
        if (index < 0 || index >= m_capacity)
            throw ArrayIndexOutOfBoundsException(index, m_capacity);
        return m_table[index].key;
    }
    V* valueAt(int index)
    {
        if (index < 0 || index >= m_capacity)
            throw ArrayIndexOutOfBoundsException(index, m_capacity);
        return &m_table[index].value;
    }

    bool grow(int newCapacity)
    {
        if (newCapacity < m_capacity)
            return false;
        static_assert(sizeof(Entry) == 8, "HTable::grow allocates 8 bytes per entry");
        Entry* t = (Entry*)operator new[]((unsigned __int64)(unsigned int)newCapacity >> 29 != 0
                                               ? (size_t)-1
                                               : 8 * (size_t)newCapacity);
        if (!t)
            return false;
        int i = 0;
        for (; i < m_count; ++i)
            t[i] = m_table[i];
        for (int j = m_count; j < newCapacity; ++j)
        {
            t[j].key = (K)-1;
            t[j].value = (V)0;
        }
        operator delete[](m_table);
        m_table = t;
        m_capacity = newCapacity;
        return true;
    }

public:
    Entry* m_table;
    int m_count;
    int m_capacity;
    int m_growBy;
};
