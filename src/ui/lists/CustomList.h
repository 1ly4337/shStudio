#pragma once
#include <windows.h>
#include "ItemList.h"
#include "primitiveVector.h"

class ItemView;
class ComboBox;
class Node;
namespace Data { class Items; }

class CustomList : public ItemList
{
public:
    CustomList(ItemView* view, Data::Items* items);
    virtual ~CustomList();

    bool onCreate() override;
    int getHeaderMenu() override;
    void loadItems(Data::Items* items) override;
    void editItem(Node* node) override;

public:
    primitiveVector<unsigned char> m_types;
};
static_assert(sizeof(CustomList) == 0x154, "CustomList size");
