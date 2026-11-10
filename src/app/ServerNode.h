#pragma once
#include "WorkspaceNode.h"
#include "String.hpp"
#include "FolderRoot.h"

class TreeList;

#pragma vtordisp(push, 2)
class ServerNode : public WorkspaceNode {
public:
    ServerNode(void* owner, void* frame);
    ~ServerNode() override;

    const wchar_t* getString(DWORD tag) const override;
    void onCommand(int cmd, int arg) override;
    void onInitItemMenu(HMENU menu, HWND tree, HTREEITEM item) override;
    virtual int connect(TreeList* tree, String* path);
    int populate(TreeList* tree);

    String     m_name;
    FolderRoot m_root;
    bool       m_connected;
};
#pragma vtordisp(pop)
static_assert(sizeof(ServerNode) == 0xE0, "ServerNode size");
