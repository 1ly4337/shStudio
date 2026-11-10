#pragma once
#include <windows.h>
#include "ptrArray.h"
#include "iStream.h"
#include "oStream.h"

template <class P>
void quickSort(ptrArray<P>& a, int lo, int hi, DWORD key1, DWORD key2, bool ascending)
{
    int j = hi;
    if (hi <= lo)
        return;
    for (;;)
    {
        int i = lo;
        P pivot = a.m_items[(lo + j) / 2];
        do
        {
            while (i < hi)
            {
                P item = a.m_items[i];
                int c;
                if (item)
                {
                    if (pivot)
                    {
                        c = item->compare(pivot, key1);
                        if (!c && key2)
                            c = item->compare(pivot, key2);
                        if (!ascending)
                            c = -c;
                    }
                    else
                        c = 2 * (ascending == 0) - 1;
                }
                else
                {
                    if (!pivot)
                        break;
                    c = 2 * (ascending != 0) - 1;
                }
                if (c >= 0)
                    break;
                ++i;
            }
            for (; j > 0; --j)
            {
                P item = a.m_items[j];
                int c;
                if (item)
                {
                    if (pivot)
                    {
                        c = item->compare(pivot, key1);
                        if (!c && key2)
                            c = item->compare(pivot, key2);
                        if (!ascending)
                            c = -c;
                    }
                    else
                        c = 2 * (ascending == 0) - 1;
                }
                else
                {
                    if (!pivot)
                        break;
                    c = 2 * (ascending != 0) - 1;
                }
                if (c <= 0)
                    break;
            }
            if (j < i)
                break;
            if (j != i)
            {
                P t = a.m_items[j];
                a.m_items[j] = a.m_items[i];
                a.m_items[i] = t;
                ++a.m_changes;
            }
            --j;
            ++i;
        } while (i <= j);
        if (lo < j)
            quickSort(a, lo, j, key1, key2, ascending);
        j = hi;
        if (i >= hi)
            break;
        lo = i;
    }
}

template <class T>
class Collection : public ptrArray<T*>
{
public:
    Collection() : ptrArray<T*>() {}
    virtual ~Collection() {}

    virtual void read(iStream& in, DWORD fmt)
    {
        this->deleteAll();
        T* item = 0;
        try
        {
            int count = (int)in.readInt();
            if (count < 0 || count > (in.m_size - in.m_pos) / T::kMinRecordSize)
                throw 1;
            for (int i = 0; i < count; ++i)
            {
                item = new T(i, 0);
                item->read(in, fmt);
                this->add(item);
            }
        }
        catch (...)
        {
            if (item)
                delete item;
            this->deleteAll();
            throw;
        }
    }
    virtual void write(oStream& out, DWORD fmt) const
    {
        out << (unsigned long)this->m_count;
        for (int i = 0; i < this->m_count; ++i)
            this->m_items[i]->write(out, fmt);
    }

    void sort(int lo, int hi, DWORD key1, DWORD key2, bool ascending)
    {
        quickSort(*this, lo, hi, key1, key2, ascending);
    }
};
