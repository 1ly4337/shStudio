#pragma once
#include <windows.h>
#include "ModelList.h"

class ModelView;
class Node;
class ModelsMLT;

class MltList : public ModelList
{
public:
    explicit MltList(ModelView* view) : ModelList(view), m_models(nullptr) {}

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

public:
    ModelsMLT* m_models;
};
static_assert(sizeof(MltList) == 0xFC, "MltList size");
