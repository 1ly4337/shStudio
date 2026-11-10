#pragma once
#include <windows.h>
#include "ItemList.h"
#include "primitiveVector.h"

class ItemView;
class ComboBox;
class Node;
namespace Data { class Items; }

class WeaponList : public ItemList
{
public:
    WeaponList(ItemView* view, Data::Items* items);
    virtual ~WeaponList();

    bool onCreate() override;
    void fillTypeCombo(ComboBox* combo) override;
    void editItem(Node* node) override;

public:
    primitiveVector<unsigned char> m_types;
};
static_assert(sizeof(WeaponList) == 0x154, "WeaponList size");
