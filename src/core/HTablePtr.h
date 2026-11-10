#pragma once
#include "HTable.h"

template <class K, class V>
class HTablePtr : public HTable<K, V>
{
public:
    typedef typename HTable<K, V>::Entry Entry;

    HTablePtr(int capacity = 32, int growBy = 32) : HTable<K, V>(capacity, growBy) {}

    virtual ~HTablePtr()
    {
        for (int i = 0; i < this->m_count; ++i)
            destroyEntry(&this->m_table[i]);
        this->m_count = 0;
    }

    virtual bool removeAt(int index)
    {
        if (index < 0 || index >= this->m_count)
            return false;
        destroyEntry(&this->m_table[index]);
        for (int i = index + 1; i < this->m_count; ++i)
        {
            this->m_table[i - 1].key = this->m_table[i].key;
            this->m_table[i - 1].value = this->m_table[i].value;
        }
        --this->m_count;
        this->m_table[this->m_count].key = (K)-1;
        this->m_table[this->m_count].value = 0;
        return true;
    }

    virtual void clear()
    {
        for (int i = 0; i < this->m_count; ++i)
            destroyEntry(&this->m_table[i]);
        this->m_count = 0;
    }

    virtual void destroyEntry(Entry* e)
    {
        V value = e->value;
        e->key = (K)-1;
        if (value)
        {
            delete value;
            e->value = 0;
        }
    }
};
