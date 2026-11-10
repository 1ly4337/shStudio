#pragma once
#include <windows.h>
#include "ItemList.h"

class ItemView;
class ComboBox;
class Node;
namespace Data { class Items; }

class WingList : public ItemList
{
public:
    WingList(ItemView* view, Data::Items* items);

    bool onCreate() override;
    int getHeaderMenu() override;
    void loadItems(Data::Items* items) override;
    void editItem(Node* node) override;

};
static_assert(sizeof(WingList) == 0x140, "WingList size");
