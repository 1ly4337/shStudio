#pragma once
#include "TreeList.h"

class DocView;
class World;
class Node;
#include "kWorld_CaptionNode.h"
#include "kWorld_Items.h"
#include "kWorld_Sounds.h"
#include "kWorld_Element.h"

class WorldList : public TreeList {
public:
    WorldList(DocView* view, World* world) : TreeList(), m_view(view), m_world(world), m_selected(0) {}
    ~WorldList() override {}
    DWORD getExStyle() override;
    void onItemsChanged() override;
    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onDropFile(LPCWSTR path, HWND target, int targetId) override;
    int  getItemMenu(HTREEITEM item) override;
    void onSelChanged(Node* oldNode, Node* newNode) override;
    bool onItemActivate(HTREEITEM item, WPARAM keys) override;
    bool editNode(Node* node) override;
    bool removeChildNode(Node* parent, HTREEITEM item) override;
    void populate(LPCWSTR rootText);
    void insertWorld(kWorld::Items* world);
    void insertSounds(LPARAM parent, kWorld::Sounds* sounds);
    void insertItems(LPARAM parent, kWorld::Element* element);

    DocView* m_view;
    World*   m_world;
    void*    m_selected;
};
static_assert(sizeof(WorldList) == 0xE0, "WorldList size");
