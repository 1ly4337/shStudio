#include "ComDlgFiltersSpec.h"
#include "String.hpp"
#include <cstdlib>
#include <cstring>
#include "_imports_p10.h"

_COMDLG_FILTERSPEC* ComDlgFiltersSpec_newEntry(const wchar_t* name, const wchar_t* spec)
{
    String nameCopy;
    nameCopy.append(name, (unsigned)-1);
    String specCopy;
    specCopy.append(spec, (unsigned)-1);
    _COMDLG_FILTERSPEC* entry = (_COMDLG_FILTERSPEC*)operator new(8);
    entry->pszName = _wcsdup(nameCopy.m_buffer);
    entry->pszSpec = _wcsdup(specCopy.m_buffer);
    return entry;
}

COMDLG_FILTERSPEC* ComDlgFiltersSpec::toArray()
{
    unsigned bytes = ((unsigned long long)(unsigned)m_count >> 29) != 0 ? (unsigned)-1 : 8 * (unsigned)m_count;
    COMDLG_FILTERSPEC* result = (COMDLG_FILTERSPEC*)operator new[](bytes);
    for (int i = 0; i < m_count; ++i)
    {
        result[i].pszName = m_items[i]->pszName;
        result[i].pszSpec = m_items[i]->pszSpec;
        m_items[i]->pszName = NULL;
        m_items[i]->pszSpec = NULL;
    }
    return result;
}

void ComDlgFiltersSpec::destroyItem(_COMDLG_FILTERSPEC* item)
{
    if (item->pszName)
        free((void*)item->pszName);
    if (item->pszSpec)
        free((void*)item->pszSpec);
    operator delete(item);
}

ComDlgFiltersSpec::ComDlgFiltersSpec()
    : ptrArray<_COMDLG_FILTERSPEC*>(16)
{
}
