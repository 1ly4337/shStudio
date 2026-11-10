#pragma once
#include <windows.h>
#include "GearList.h"

class ItemView;
class ComboBox;
class Node;
namespace Data { class Items; }

class LightGearList : public GearList
{
public:
    LightGearList(ItemView* view, Data::Items* items);

    int getHeaderMenu() override;
    void fillTypeCombo(ComboBox* combo) override;
    void loadItems(Data::Items* items) override;

};
static_assert(sizeof(LightGearList) == 0x140, "LightGearList size");
