#pragma once
#include <windows.h>
#include "GearList.h"

class ItemView;
class ComboBox;
class Node;
namespace Data { class Items; }

class DarkGearList : public GearList
{
public:
    DarkGearList(ItemView* view, Data::Items* items);

    int getHeaderMenu() override;
    void fillTypeCombo(ComboBox* combo) override;
    void loadItems(Data::Items* items) override;

};
static_assert(sizeof(DarkGearList) == 0x140, "DarkGearList size");
