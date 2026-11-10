#pragma once
#include <windows.h>
#include "ItemList.h"

class ItemView;
class ComboBox;
class Node;
namespace Data { class Items; }

class CapeList : public ItemList
{
public:
    CapeList(ItemView* view, Data::Items* items);

    bool onCreate() override;
    int getHeaderMenu() override;
    void fillTypeCombo(ComboBox* combo) override;
    void loadItems(Data::Items* items) override;
    void editItem(Node* node) override;

};
static_assert(sizeof(CapeList) == 0x140, "CapeList size");
