#pragma once
#include "TreeList.h"

class WorkspaceList : public TreeList {
public:
    WorkspaceList() : TreeList(), m_reserved(0), m_popupMenu(161) {}
    ~WorkspaceList() override {}

    bool onCreate() override;
    bool onItemActivate(HTREEITEM item, WPARAM keys) override;
    void reportDropError(int error);

    int m_reserved;
    int m_popupMenu;
};
static_assert(sizeof(WorkspaceList) == 0xDC, "WorkspaceList size");
