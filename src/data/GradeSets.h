#pragma once
#include <windows.h>
#include "ptrArray.h"
#include "GradeSet.h"

class GradeSet;

class GradeSets : public ptrArray<GradeSet*>
{
public:
    GradeSets() {}
    virtual ~GradeSets();

    void detachAll();
    GradeSet* find(short grade) const;
    void sort(int first, int last, DWORD tag1, DWORD tag2, bool ascending);
};
static_assert(sizeof(GradeSets) == 0x18, "GradeSets size");
