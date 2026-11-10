#pragma once
#include "String.hpp"

template <class T>
class associativeArray
{
public:
    struct Item
    {
        Item() : value(0) {}
        ~Item()
        {
            if (value)
                delete value;
            value = 0;
        }
        String key;
        T value;
    };

    associativeArray(int capacity = 16)
    {
        m_capacity = capacity;
        m_count = 0;
        m_growBy = capacity;
        m_items = new Item[capacity];
    }
    virtual ~associativeArray()
    {
        deleteAll();
        if (m_items)
        {
            delete[] m_items;
            m_items = 0;
        }
    }
    virtual bool contains(const wchar_t* key) { return find(key) != 0; }
    virtual bool set(const wchar_t* key, T value)
    {
        if (!key || !value)
            return false;
        Item* item = find(key);
        if (item)
        {
            if (item->value)
                delete item->value;
            item->value = value;
            return true;
        }
        if (m_count == m_capacity && !grow(m_count + m_growBy))
            return false;
        m_items[m_count].key = key;
        m_items[m_count++].value = value;
        return true;
    }
    virtual bool removeAt(int index)
    {
        if (index < 0 || index >= m_count)
            return false;
        Item* item = &m_items[index];
        if (item->value)
            delete item->value;
        item->value = 0;
        for (int i = index + 1; i < m_count; ++i)
        {
            Item* dst = &m_items[i - 1];
            Item* src = &m_items[i];
            dst->key.assign(src->key.m_buffer, src->key.m_length);
            dst->value = src->value;
            src->value = 0;
        }
        --m_count;
        Item* last = &m_items[m_count];
        last->key.clear();
        last->value = 0;
        return true;
    }

    Item* find(const wchar_t* key) const
    {
        for (int i = 0; i < m_count; ++i)
            if (m_items[i].key.equalsNoCase(key))
                return &m_items[i];
        return 0;
    }
    bool grow(int newCapacity)
    {
        if (m_capacity == newCapacity)
            return true;
        Item* items = new Item[newCapacity];
        if (!items)
            return false;
        for (int i = 0; i < (newCapacity < m_count ? newCapacity : m_count); ++i)
        {
            if (&m_items[i] != &items[i])
                items[i].key.assign(m_items[i].key.m_buffer, m_items[i].key.m_length);
            items[i].value = m_items[i].value;
            m_items[i].value = 0;
        }
        if (m_items)
            delete[] m_items;
        m_items = items;
        m_capacity = newCapacity;
        return true;
    }
    void deleteAll()
    {
        if (m_items)
        {
            for (int i = 0; i < m_count; ++i)
            {
                if (m_items[i].value)
                    delete m_items[i].value;
                m_items[i].value = 0;
            }
            m_count = 0;
        }
    }

    Item* m_items;
    int m_count;
    int m_capacity;
    int m_growBy;
};
