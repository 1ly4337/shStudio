#include "Strings.h"
#include "Exception.h"
#include <shlwapi.h>
#include <cwchar>

int Strings::compare_string::operator()(const String* a, const String* b) const
{
    const wchar_t* textA = a->m_buffer;
    const wchar_t* textB = b->m_buffer;
    if (!textA)
        return -(textB != 0);
    if (textB)
        return StrCmpLogicalW(textA, textB);
    return 1;
}

int Strings::indexOf(const wchar_t* text) const
{
    if (!text)
        return -1;
    for (int i = 0; i < m_count; ++i)
    {
        const String* item = m_items[i];
        bool equal;
        if (!*text)
            equal = item->m_length == 0;
        else
        {
            if (wcslen(text) != item->m_length)
                continue;
            equal = _wcsnicmp(item->m_buffer, text, item->m_length) == 0;
        }
        if (equal)
            return i;
    }
    return -1;
}

bool Strings::add(long id, const wchar_t* text)
{
    const wchar_t* value = L"";
    if (text)
        value = text;
    bool result = add(value);
    if (result)
    {
        m_ids.add(id);
        return true;
    }
    return result;
}

bool Strings::add(const wchar_t* text)
{
    if (!text || (!m_allowDuplicates && indexOf(text) != -1))
        return false;
    String* item = new String(text, (unsigned)-1);
    if (!item)
        return false;
    if (m_count == m_capacity && !grow(m_capacity + m_growBy))
        return false;
    m_items[m_count++] = item;
    return true;
}

Strings& Strings::operator=(const Strings& other)
{
    if (&other != this)
    {
        deleteAll();
        m_ids.m_count = 0;
        m_allowDuplicates = other.m_allowDuplicates;
        m_ids.m_count = 0;
        m_ids.addAll(other.m_ids.m_count, other.m_ids.m_items);
        for (int i = 0; i < other.m_count; ++i)
        {
            const String* source = other.m_items[i];
            if (source)
            {
                String* copy = new String(*source);
                if (copy)
                {
                    if (m_count != m_capacity || grow(m_capacity + m_growBy))
                        m_items[m_count++] = copy;
                }
            }
        }
    }
    return *this;
}

String* Strings::at(int index) const
{
    if (index < 0 || index >= m_count)
        throw ArrayIndexOutOfBoundsException(index, m_count);
    return m_items[index];
}

String* Strings::findById(long id) const
{
    int count = m_ids.m_count;
    for (int i = 0; i < count; ++i)
    {
        long value = (i < 0 || i >= count) ? 0 : m_ids.m_items[i];
        if (value == id)
            return m_items[i];
    }
    return 0;
}

String* Strings::strID(long id) const
{
    int count = m_ids.m_count;
    for (int i = 0; i < count; ++i)
    {
        long value = (i < 0 || i >= count) ? 0 : m_ids.m_items[i];
        if (value == id)
            return m_items[i];
    }
    throw NullPointerException(L"Strings::strID");
}

static inline int compareItems(const String* item, const String* pivot, bool logical)
{
    if (logical)
    {
        const wchar_t* a = item->m_buffer;
        const wchar_t* b = pivot->m_buffer;
        if (!a)
            return -(b != 0);
        if (b)
            return StrCmpLogicalW(a, b);
        return 1;
    }
    return item->compare(*pivot);
}

void Strings::sort(int first, int last, bool logical)
{
    if (last <= first)
        return;
    for (;;)
    {
        int i = first;
        int j = last;
        String* pivot = m_items[(first + last) / 2];
        do
        {
            while (i < last)
            {
                String* item = m_items[i];
                int c;
                if (item)
                {
                    if (!pivot)
                    {
                        ++i;
                        continue;
                    }
                    c = compareItems(item, pivot, logical);
                }
                else
                    c = pivot != 0;
                if (c >= 0)
                    break;
                ++i;
            }
            while (j > 0)
            {
                String* item = m_items[j];
                int c;
                if (item)
                {
                    if (!pivot)
                        break;
                    c = compareItems(item, pivot, logical);
                }
                else
                    c = pivot != 0;
                if (c <= 0)
                    break;
                --j;
            }
            if (j < i)
                break;
            if (j != i)
            {
                String* t = m_items[j];
                m_items[j] = m_items[i];
                m_items[i] = t;
                ++m_changes;
            }
            --j;
            ++i;
        } while (i <= j);
        if (first < j)
            sort(first, j, logical);
        if (i < last)
        {
            first = i;
            continue;
        }
        break;
    }
}
