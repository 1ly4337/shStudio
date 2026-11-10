#pragma once
#include <windows.h>
#include "ItemList.h"

class ItemView;
class ComboBox;
class Node;
namespace Data { class Items; }

class LapisList : public ItemList
{
public:
    LapisList(ItemView* view, Data::Items* items);

    bool onCreate() override;
    int getHeaderMenu() override;
    void fillTypeCombo(ComboBox* combo) override;
    void loadItems(Data::Items* items) override;
    void addClassColumns() override;
    void editItem(Node* node) override;
    virtual const wchar_t* getLapisText(Node* node, int columnId);

};
static_assert(sizeof(LapisList) == 0x140, "LapisList size");
