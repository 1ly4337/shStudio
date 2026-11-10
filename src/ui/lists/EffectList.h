#pragma once
#include <windows.h>
#include "ListCtrlOD.h"

class EffectView;
class EffectSets;
class Node;

class EffectList : public ListCtrlOD
{
public:
    explicit EffectList(EffectView* view) : m_view(view) {}

    DWORD getStyle() override;
    bool onCreate() override;
    bool onInitMenuPopup(HMENU menu) override;
    void onChanged() override;
    int insertItem(Node* node, int index) override;
    Node* getNode(int index) override;
    DWORD getExStyle() override;
    bool sort() override;
    void onItemChanged(int index, UINT newState) override;
    bool removeItem(int index) override;
    Node* createNode() override;
    bool clearItem(int index) override;
    bool moveItemUp(int index) override;
    bool moveItemDown(int index) override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override;
    UINT getClipboardFormat() override;

public:
    EffectView* m_view;
    EffectSets* m_data;
};
static_assert(sizeof(EffectList) == 0xFC, "EffectList size");
