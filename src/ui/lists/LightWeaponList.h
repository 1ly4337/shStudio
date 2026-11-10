#pragma once
#include <windows.h>
#include "WeaponList.h"

class ItemView;
class ComboBox;
class Node;
namespace Data { class Items; }

class LightWeaponList : public WeaponList
{
public:
    LightWeaponList(ItemView* view, Data::Items* items);

    int getHeaderMenu() override;
    void loadItems(Data::Items* items) override;

};
static_assert(sizeof(LightWeaponList) == 0x154, "LightWeaponList size");
