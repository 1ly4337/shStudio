#pragma once
#include <windows.h>
#include "ItemList.h"
#include "primitiveVector.h"

class ItemView;
class ComboBox;
class Node;
namespace Data { class Items; }

class GoodsList : public ItemList
{
public:
    GoodsList(ItemView* view, Data::Items* items);
    virtual ~GoodsList();

    bool onCreate() override;
    const wchar_t* getItemText(Node* node, int columnId) override;
    int getHeaderMenu() override;
    void fillTypeCombo(ComboBox* combo) override;
    void loadItems(Data::Items* items) override;
    void editItem(Node* node) override;

public:
    primitiveVector<unsigned char> m_types;
    primitiveVector<unsigned char> m_types2;
};
static_assert(sizeof(GoodsList) == 0x168, "GoodsList size");
