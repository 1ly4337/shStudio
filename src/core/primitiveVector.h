#pragma once
#include <windows.h>
#include <cstring>
#include <new>

template <class T>
class primitiveVector
{
public:
    primitiveVector(int capacity = 32, int growBy = 32)
    {
        m_capacity = capacity;
        m_growBy = growBy;
        m_count = 0;
        m_items = (T*)::operator new[](sizeof(T) * (unsigned)capacity);
        for (int i = 0; i < m_capacity; ++i)
            m_items[i] = 0;
    }
    virtual ~primitiveVector()
    {
        for (int i = 0; i < m_capacity; ++i)
            resetItem(&m_items[i]);
        m_count = 0;
        ::operator delete[](m_items);
    }
    virtual bool add(T value)
    {
        if (m_count == m_capacity)
        {
            if (!m_growBy || !grow(m_capacity + m_growBy))
                return false;
        }
        m_items[m_count++] = value;
        return true;
    }
    virtual bool set(T value, int index)
    {
        if (index < 0 || index >= m_capacity)
            return false;
        if (m_items[index])
        {
            resetItem(&m_items[index]);
            m_items[index] = value;
        }
        else
        {
            ++m_count;
            m_items[index] = value;
        }
        return true;
    }
    virtual T* resetItem(T* item)
    {
        *item = 0;
        return item;
    }

    primitiveVector& operator=(const primitiveVector& other)
    {
        for (int i = 0; i < m_capacity; ++i)
            resetItem(&m_items[i]);
        m_count = 0;
        m_growBy = other.m_growBy;
        grow((other.m_count & 0xFFFFFFC0) + 64);
        for (int j = 0; j < other.m_count; ++j)
            add(other.m_items[j]);
        return *this;
    }
    primitiveVector(const primitiveVector&) = default;

    bool grow(int newCapacity)
    {
        if (newCapacity < m_capacity)
            return false;
        T* items = (T*)::operator new[](((unsigned __int64)(unsigned)newCapacity >> 30) != 0
                                            ? (size_t)-1 : sizeof(T) * (unsigned)newCapacity);
        if (!items)
            return false;
        memset(items, 0, sizeof(T) * newCapacity);
        for (int i = 0; i < m_count; ++i)
            items[i] = m_items[i];
        ::operator delete[](m_items);
        m_items = items;
        m_capacity = newCapacity;
        return true;
    }

    bool contains(T value) const
    {
        for (int i = 0; i < m_count; ++i)
            if (m_items[i] == value)
                return true;
        return false;
    }

    T*  m_items;
    int m_count;
    int m_capacity;
    int m_growBy;
};
