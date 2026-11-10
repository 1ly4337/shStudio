#pragma once
#include <windows.h>
#include "ListCtrlOD.h"
#include "DropTarget.h"
#include "ItemsSet.h"
#include "primitiveArray.h"

class Window;
class Node;
class iStream;
class Merchant;
namespace Data { class Item; class Items; }

class ItemsList : public ListCtrlOD, public DropTarget
{
public:
    ItemsList(Window* owner, Merchant* merchant);
    virtual ~ItemsList();

    DWORD getStyle() override;
    bool onCreate() override;
    bool onNotify(NMHDR* hdr, LRESULT* result) override;
    Node* getNode(int index) override;
    bool sort() override { return false; }
    bool onActivateItem(int index, int keys) override;
    bool removeItem(int index) override;
    bool moveItemUp(int index) override;
    bool moveItemDown(int index) override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override;

    bool acceptsFormat(int format) override;
    void setDropHighlight(bool on) override;
    void onDropStream(int format, iStream& stream) override;

    void load(const primitiveArray<long>* ids);
    void save(primitiveArray<long>* ids);
    static Data::Item* findItem(Data::ItemsSet* items, int unused, const int& key);

public:
    Window*        m_owner;
    Merchant*      m_merchant;
    Data::Items*   m_itemDB;
    Data::ItemsSet m_items;
};
