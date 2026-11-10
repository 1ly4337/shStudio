#pragma once
#include <windows.h>
#include "ModelList.h"

class ModelView;
class MonsterModels;
class Node;

class MonList : public ModelList
{
public:
    explicit MonList(ModelView* view) : ModelList(view), m_node(0) {}

    bool onCreate() override;
    int insertItem(Node* node, int index) override;
    Node* getNode(int index) override;
    int listV69() override { return 'name'; }
    bool sort() override;
    void onItemChanged(int index, UINT newState) override;
    bool removeItem(int index) override;
    Node* createNode() override;
    bool clearItem(int index) override;
    bool moveItemUp(int index) override;
    bool moveItemDown(int index) override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override;
    UINT getClipboardFormat() override { return 520; }
    virtual int vslot113() { return 9; }

    MonsterModels* models() const { return (MonsterModels*)m_node; }

public:
    Node* m_node;
};
static_assert(sizeof(MonList) == 0xFC, "MonList size");
