#pragma once
#include "ptrArray.h"
#include "QuestDetails.h"

class QuestDetails;
class iStream;
class oStream;

class QuestsDetails : public ptrArray<QuestDetails*>
{
public:
    QuestsDetails() : ptrArray<QuestDetails*>(32) {}

    void read(iStream& s, int tag);
    void writeList(oStream& s, int tag);

public:
    int m_version;
};
static_assert(sizeof(QuestsDetails) == 0x1C, "QuestsDetails size");
