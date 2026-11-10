#pragma once
#include <windows.h>

class String;

void sqlTruncateHeader(String& out, const wchar_t* table1, const wchar_t* table2);

#include "ptrArray.h"
#include "Collection.hpp"
template <class P>
inline bool sortList(ptrArray<P>& list, DWORD key1, DWORD key2, bool ascending)
{
    int count = list.m_count;
    if (count < 2)
        return false;
    list.m_changes = 0;
    quickSort(list, 0, count - 1, key1, key2, ascending);
    return list.m_changes > 0;
}
