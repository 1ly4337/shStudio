#pragma once
#include <windows.h>
#include "ListCtrlOD.h"
#include "Data_ItemSets.h"

class ItemSetView;
class Node;
namespace Data { class ItemSets; class ItemSet; }

class ItemSetList : public ListCtrlOD
{
public:
    explicit ItemSetList(ItemSetView* owner);
    virtual ~ItemSetList() {}

    DWORD getStyle() override { return 9; }
    bool onCreate() override;
    Node* getNode(int index) override { return (Node*)m_sets->get(index); }
    DWORD getExStyle() override { return 0x21; }
    bool onActivateItem(int index, int keys) override;
    void onItemChanged(int index, UINT newState) override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override { return 0; }

public:
    ItemSetView*    m_owner;
    Data::ItemSets* m_sets;
};
static_assert(sizeof(ItemSetList) == 0xFC, "ItemSetList size");
