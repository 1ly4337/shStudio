#pragma once
#include <windows.h>
#include "WeaponList.h"

class ItemView;
class ComboBox;
class Node;
namespace Data { class Items; }

class DarkWeaponList : public WeaponList
{
public:
    DarkWeaponList(ItemView* view, Data::Items* items);

    int getHeaderMenu() override;
    void loadItems(Data::Items* items) override;

};
static_assert(sizeof(DarkWeaponList) == 0x154, "DarkWeaponList size");
