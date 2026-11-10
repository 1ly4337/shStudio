#pragma once
#include <windows.h>
#include <cstring>
#include <new>
#include "Exception.h"

template <class T>
class primitiveArray
{
public:
    primitiveArray(int capacity = 16)
    {
        m_capacity = capacity;
        m_count = 0;
        m_items = (T*)::operator new[](sizeof(T) * (unsigned)capacity);
        for (int i = 0; i < m_capacity; ++i)
            m_items[i] = 0;
    }
    virtual ~primitiveArray()
    {
        ::operator delete[](m_items);
    }
    virtual bool addAll(int count, const T* values)
    {
        for (int i = 0; i < count; ++i)
            if (!add(values[i]))
                return false;
        return true;
    }
    virtual bool add(T value)
    {
        if (m_count == m_capacity && !grow(m_capacity + 16))
            return false;
        m_items[m_count++] = value;
        return true;
    }

    T* at(int index)
    {
        if (index < 0 || index >= m_count)
            throw ArrayIndexOutOfBoundsException(index, m_count);
        return &m_items[index];
    }
    bool removeAt(int index)
    {
        if (index < 0 || index >= m_count)
            return false;
        if (m_items[index])
        {
            for (int i = index + 1; i < m_count; ++i)
                m_items[i - 1] = m_items[i];
        }
        --m_count;
        m_items[m_count] = 0;
        return true;
    }
    bool remove(T value)
    {
        int index = 0;
        int count = m_count;
        if (count <= 0)
            return false;
        while (m_items[index] != value)
        {
            if (++index >= count)
                return false;
        }
        if (index < 0 || index >= count)
            return false;
        if (m_items[index])
        {
            for (int i = index + 1; i < m_count; ++i)
                m_items[i - 1] = m_items[i];
        }
        --m_count;
        m_items[m_count] = 0;
        return true;
    }

    bool grow(int newCapacity)
    {
        if (m_capacity == newCapacity)
            return true;
        T* items = (T*)::operator new[](((unsigned __int64)(unsigned)newCapacity >> 30) != 0
                                            ? (size_t)-1 : sizeof(T) * (unsigned)newCapacity);
        if (!items)
            return false;
        int i = 0;
        if (m_count <= newCapacity)
        {
            for (; i < m_count; ++i)
                items[i] = m_items[i];
            if (m_count < newCapacity)
                memset(items + m_count, 0, sizeof(T) * (newCapacity - m_count));
        }
        else
        {
            for (; i < newCapacity; ++i)
                items[i] = m_items[i];
        }
        ::operator delete[](m_items);
        m_items = items;
        m_capacity = newCapacity;
        return true;
    }

    T*  m_items;
    int m_count;
    int m_capacity;
};
