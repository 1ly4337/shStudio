#pragma once
#include <windows.h>
#include "ListCtrlOD.h"

class TacticsView;
class TacticsZones;
class Node;

class TacticsList : public ListCtrlOD
{
public:
    TacticsList(TacticsView* owner, TacticsZones* zones) : m_owner(owner), m_zones(zones) {}

    DWORD getStyle() override { return 9; }
    bool onCreate() override;
    int insertItem(Node* item, int index) override;
    Node* getNode(int index) override;
    bool sort() override;
    void onItemChanged(int index, UINT newState) override;
    Node* createNode() override;
    int indexOfNode(Node* item) override;
    Node* findNode(Node* like) override { return 0; }

public:
    TacticsView*  m_owner;
    TacticsZones* m_zones;
};
static_assert(sizeof(TacticsList) == 0xFC, "TacticsList size");
