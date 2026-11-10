#pragma once
#include "Node.h"
#include "Collection.hpp"
#include "ptrArray.h"
#include "MonsterArea.h"

#pragma vtordisp(push, 2)
class MonsterSpot : public Node {
public:
    explicit MonsterSpot(Node* monster);
    ~MonsterSpot() override;

    const wchar_t* getString(DWORD tag) const override;
    int  getInt(DWORD tag) const override;
    void reset() override {}
    int  compare(const Node* other, DWORD tag) const override;

    Node*                   m_monster;
    Collection<MonsterArea> m_areas;
};
#pragma vtordisp(pop)
static_assert(sizeof(MonsterSpot) == 0x30, "MonsterSpot size");

class MonsterSpots : public ptrArray<MonsterSpot*> {
public:
    MonsterSpots() : ptrArray<MonsterSpot*>(256) {}
};
static_assert(sizeof(MonsterSpots) == 0x18, "MonsterSpots size");
