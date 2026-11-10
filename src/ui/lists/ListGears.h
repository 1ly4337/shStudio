#pragma once
#include "ListCtrl.h"
#include "ItemsSet.h"
#include "primitiveVector.h"

class ListGears : public ListCtrl {
public:
    explicit ListGears(void* items) : ListCtrl(), m_items(items), m_set() {}
    ~ListGears() override;
    bool  onCreate() override;
    Node* getNode(int index) override;
    void  fill(const primitiveVector<unsigned char>& types, int modelIndex, unsigned int raceSlot);

    void*          m_items;
    Data::ItemsSet m_set;
};
static_assert(sizeof(ListGears) == 0x120, "ListGears size");
