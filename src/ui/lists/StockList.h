#pragma once
#include <windows.h>
#include "ListCtrlOD.h"
#include "ItemsSet.h"

class Window;
class Node;
namespace Data { class Items; }

class StockList : public ListCtrlOD
{
public:
    explicit StockList(Window* owner);
    virtual ~StockList();

    DWORD getStyle() override;
    bool onCreate() override;
    bool onNotify(NMHDR* hdr, LRESULT* result) override;
    Node* getNode(int index) override;
    bool sort() override;
    bool onActivateItem(int index, int keys) override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override;

    bool load(int itemType, unsigned char subType, bool lightFaction, int itemClass);

public:
    Window*        m_owner;
    Data::Items*   m_itemDB;
    Data::ItemsSet m_items;
};
