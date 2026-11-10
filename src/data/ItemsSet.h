#pragma once
#include "ptrArray.h"
#include "Node.h"
#include "Data_Item.h"

namespace Data {
class Item;

class ItemsSet : public ptrArray<Data::Item*>, public Node {
public:
    ItemsSet();
    ~ItemsSet() override;
    void reset() override;
    int  compare(const Node* other, DWORD tag) const override;
};
}
static_assert(sizeof(Data::ItemsSet) == 0x28, "Data::ItemsSet size");
