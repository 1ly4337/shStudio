#pragma once
#include <windows.h>
#include "Serializable.h"
#include "StandardNPCs.h"

class iStream;
class oStream;
class StandardNPC;

class NpcPerso : public Serializable
{
public:
    NpcPerso();
    virtual ~NpcPerso();

    bool isEmpty() const override;
    void clear() override;

    void read(iStream& s, DWORD tag) override;
    void write(oStream& s, DWORD tag) const override;

    StandardNPC* find(unsigned char type, short id);

    StandardNPCs m_npcs[14];
    bool         m_isNew;
};
static_assert(sizeof(NpcPerso) == 0x1E0, "NpcPerso size");
