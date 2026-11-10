#pragma once
#include <windows.h>
#include <shtypes.h>
#include "ptrArray.h"

class ComDlgFiltersSpec : public ptrArray<_COMDLG_FILTERSPEC*>
{
public:
    ComDlgFiltersSpec();
    virtual ~ComDlgFiltersSpec() {}

    virtual void destroyItem(_COMDLG_FILTERSPEC* item);

    COMDLG_FILTERSPEC* toArray();
};

_COMDLG_FILTERSPEC* ComDlgFiltersSpec_newEntry(const wchar_t* name, const wchar_t* spec);
