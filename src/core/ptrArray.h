#pragma once
#include <windows.h>
#include <cstring>
#include <new>

template <class T>
class ptrArray
{
public:
    explicit ptrArray(int capacity = 256)
    {
        m_capacity = capacity;
        m_growBy = capacity;
        m_count = 0;
        m_items = (T*)::operator new[](sizeof(T) * (unsigned)capacity);
        for (int i = 0; i < m_capacity; ++i)
            m_items[i] = 0;
        m_changes = 0;
    }
    virtual ~ptrArray()
    {
        deleteAll();
        if (m_items)
        {
            ::operator delete[](m_items);
            m_items = 0;
        }
    }
    virtual int size() const { return m_count; }
    virtual int capacity() const { return m_capacity; }
    virtual bool isEmpty() const { return m_count == 0; }
    virtual T get(int index) const
    {
        if (index < 0 || index >= m_count)
            return 0;
        return m_items[index];
    }
    virtual int indexOf(const T item) const
    {
        if (!item)
            return -1;
        for (int i = 0; i < m_count; ++i)
            if (m_items[i] == item)
                return i;
        return -1;
    }
    virtual bool swap(int a, int b)
    {
        if (a < 0 || a >= m_count || b < 0 || b >= m_count || a == b)
            return false;
        T itemA = m_items[a];
        if (!itemA || !m_items[b])
            return false;
        m_items[a] = m_items[b];
        m_items[b] = itemA;
        return true;
    }
    virtual bool contains(const T item) const
    {
        if (!item)
            return false;
        for (int i = 0; i < m_count; ++i)
            if (m_items[i] == item)
                return true;
        return false;
    }
    virtual bool add(T item)
    {
        if (!item)
            return false;
        if (m_count == m_capacity && !grow(m_capacity + m_growBy))
            return false;
        m_items[m_count++] = item;
        return true;
    }
    virtual int insert(T item, int at)
    {
        if (!item)
            return -1;
        if (at < 0 || at >= m_count)
        {
            if (add(item))
                return m_count - 1;
            return -1;
        }
        if (m_count == m_capacity && !grow(m_capacity + m_growBy))
            return -1;
        for (int i = m_count - 1; i >= at; --i)
            m_items[i + 1] = m_items[i];
        m_items[at] = item;
        ++m_count;
        return at;
    }
    virtual T detach(int index)
    {
        if (index < 0 || index >= m_count)
            return 0;
        T item = m_items[index];
        for (int i = index + 1; i < m_count; ++i)
            m_items[i - 1] = m_items[i];
        --m_count;
        m_items[m_count] = 0;
        return item;
    }
    virtual bool remove(int index)
    {
        if (index < 0 || index >= m_count)
            return false;
        T item = m_items[index];
        if (item)
        {
            destroyItem(item);
            for (int i = index + 1; i < m_count; ++i)
                m_items[i - 1] = m_items[i];
        }
        --m_count;
        m_items[m_count] = 0;
        return true;
    }
    virtual void destroyItem(T item)
    {
        if (item)
            delete item;
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
            for (i = newCapacity; i < m_count; ++i)
            {
                destroyItem(m_items[i]);
                m_items[i] = 0;
            }
        }
        ::operator delete[](m_items);
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
                if (m_items[i])
                {
                    destroyItem(m_items[i]);
                    m_items[i] = 0;
                }
            }
            m_count = 0;
        }
    }

    bool detachItem(T item)
    {
        if (!item)
            return false;
        int count = m_count;
        int index = 0;
        if (count <= 0)
            return false;
        while (m_items[index] != item)
            if (++index >= count)
                return false;
        for (int i = index + 1; i < m_count; ++i)
            m_items[i - 1] = m_items[i];
        --m_count;
        m_items[m_count] = 0;
        return true;
    }

    T*  m_items;
    int m_count;
    int m_capacity;
    int m_growBy;
    int m_changes;
};
