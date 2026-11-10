#pragma once
#include "ptrArray.h"
#include "p03_free.h"
#include "StandardNPC.h"
#include "iStream.h"
#include "oStream.h"

class StandardNPCs : public ptrArray<StandardNPC*> {
public:
    StandardNPCs();

    virtual void readList(iStream& s, int tag);
    virtual void writeList(oStream& s, int tag);
    virtual void cleanCR();
    virtual void clearAllLists();
    virtual void buildAllListStrings();

    static StandardNPC* create(int type);
    bool sort(DWORD column, DWORD column2, bool ascending) { return sortList(*this, column, column2, ascending); }

public:
    int m_18;
    int m_type;
};
static_assert(sizeof(StandardNPCs) == 0x20, "StandardNPCs size");
