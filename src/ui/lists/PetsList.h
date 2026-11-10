#pragma once
#include <windows.h>
#include "ItemList.h"

class ItemView;
class ComboBox;
class Node;
namespace Data { class Items; }

class PetsList : public ItemList
{
public:
    PetsList(ItemView* view, Data::Items* items);

    bool onCreate() override;
    int getHeaderMenu() override;
    void loadItems(Data::Items* items) override;
    void editItem(Node* node) override;

};
static_assert(sizeof(PetsList) == 0x140, "PetsList size");
