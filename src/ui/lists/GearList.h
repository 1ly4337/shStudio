#pragma once
#include <windows.h>
#include "ItemList.h"

class ItemView;
class ComboBox;
class Node;
namespace Data { class Items; }

class GearList : public ItemList
{
public:
    GearList(ItemView* view, Data::Items* items) : ItemList(view, items) {}

    bool onCreate() override;
    Node* findNode(Node* like) override;
    void editItem(Node* node) override;

};
