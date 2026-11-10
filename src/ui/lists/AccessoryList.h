#pragma once
#include <windows.h>
#include "ItemList.h"
#include "primitiveVector.h"

class ItemView;
class ComboBox;
class Node;
namespace Data { class Items; }

class AccessoryList : public ItemList
{
public:
    AccessoryList(ItemView* view, Data::Items* items);
    virtual ~AccessoryList();

    bool onCreate() override;
    int getHeaderMenu() override;
    void fillTypeCombo(ComboBox* combo) override;
    void loadItems(Data::Items* items) override;
    void editItem(Node* node) override;

public:
    primitiveVector<unsigned char> m_types;
};
static_assert(sizeof(AccessoryList) == 0x154, "AccessoryList size");
