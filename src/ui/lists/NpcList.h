#pragma once
#include <windows.h>
#include "ListCtrlOD.h"
#include "StandardNPCs.h"

class DocView;
class NpcData;
class Node;

class NpcList : public ListCtrlOD
{
public:
    NpcList(DocView* view, NpcData* data);
    virtual ~NpcList();

    DWORD getStyle() override { return 9; }
    bool onInitMenuPopup(HMENU menu) override;
    void onChanged() override;
    int insertItem(Node* node, int index) override;
    Node* getNode(int index) override;
    int listV69() override { return 'name'; }
    bool sort() override;
    void onItemChanged(int index, UINT newState) override;
    bool clearItem(int index) override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override;
    void onPasteDone(bool added) override;

public:
    DocView*     m_view;
    NpcData*     m_data;
    StandardNPCs m_npcs;
};
