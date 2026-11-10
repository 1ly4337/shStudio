#pragma once
#include <windows.h>
#include "ModelList.h"

class ModelView;
class Node;
class FileTree;
class DualLayerModels;

class DualLayerList : public ModelList
{
public:
    DualLayerList(ModelView* view, FileTree* node) : ModelList(view), m_node(node), m_models(nullptr) {}

    bool onCreate() override;
    int insertItem(Node* node, int index) override;
    Node* getNode(int index) override;
    bool sort() override;
    bool removeItem(int index) override;
    Node* createNode() override;
    bool moveItemUp(int index) override;
    bool moveItemDown(int index) override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override;
    UINT getClipboardFormat() override;
    bool onCopied(Node* node) override;
    void onPasted(Node* node) override;

public:
    FileTree*   m_node;
    DualLayerModels* m_models;
};
static_assert(sizeof(DualLayerList) == 0x100, "DualLayerList size");
