#pragma once
#include <windows.h>
#include <commctrl.h>
#include "ItemsSet.h"

class GradeSet : public Data::ItemsSet
{
public:
    explicit GradeSet(unsigned short grade);
    virtual ~GradeSet();

    virtual int compare(const Node* other, DWORD tag) const;

public:
    unsigned short m_grade;
    HTREEITEM      m_hItem;
};
static_assert(sizeof(GradeSet) == 0x30, "GradeSet size");
