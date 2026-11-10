#pragma once
#include <windows.h>
#include "ItemList.h"

class ItemView;
class ComboBox;
class Node;
namespace Data { class Items; }

class MountList : public ItemList
{
public:
    MountList(ItemView* view, Data::Items* items);

    bool onCreate() override;
    void fillTypeCombo(ComboBox* combo) override;
    void loadItems(Data::Items* items) override;
    void editItem(Node* node) override;

};
static_assert(sizeof(MountList) == 0x140, "MountList size");
