#pragma once
#include <windows.h>
#include "ItemList.h"

class ItemView;
class ComboBox;
class Node;
namespace Data { class Items; }

class RawItemList : public ItemList
{
public:
    RawItemList(ItemView* view, Data::Items* items) : ItemList(view, items) {}

    bool onCreate() override;
    void loadItems(Data::Items* items) override;
    void editItem(Node* node) override;

};
static_assert(sizeof(RawItemList) == 0x140, "RawItemList size");
