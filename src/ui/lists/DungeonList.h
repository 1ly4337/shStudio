#pragma once
#include "TreeList.h"

class Dungeon;

class DungeonList : public TreeList {
public:
    DungeonList() : TreeList(), m_dungeon(0) {}
    ~DungeonList() override {}

    DWORD getStyle() override { return 0x20; }
    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void onSelChanged(Node* oldNode, Node* newNode) override;
    bool onItemActivate(HTREEITEM item, WPARAM keys) override;
    virtual const wchar_t* getNumberFormat();
    LRESULT setDungeon(Dungeon* dungeon);

    Dungeon* m_dungeon;
};
static_assert(sizeof(DungeonList) == 0xD8, "DungeonList size");
