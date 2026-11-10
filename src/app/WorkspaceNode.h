#pragma once
#include <windows.h>
#include <commctrl.h>
#include "Node.h"

#pragma vtordisp(push, 2)
class WorkspaceNode : public Node {
public:
    WorkspaceNode(void* owner, int image, int popupMenu);

    int  getId() const override;
    void reset() override {}
    int  compare(const Node*, DWORD) const override { return 0; }

    virtual void onCommand(int cmd, int arg) {}
    virtual int  queryInfo(int id);
    virtual int  getPopupMenu();
    virtual void onInitItemMenu(HMENU menu, HWND tree, HTREEITEM item) {}

    void* m_owner;
    int   m_image;
    int   m_popupMenu;
};
#pragma vtordisp(pop)
static_assert(sizeof(WorkspaceNode) == 0x20, "WorkspaceNode size");
