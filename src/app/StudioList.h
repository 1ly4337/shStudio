#pragma once
#include "WorkspaceList.h"

class StudioFrame;
class ClientNode;

class StudioList : public WorkspaceList {
public:
    StudioList(StudioFrame* frame, ClientNode* client)
        : WorkspaceList(), m_frame(frame), m_client(client) {}

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    int  getNodeImage(Node* node) override;
    int  getItemMenu(HTREEITEM item) override;
    void onInitItemMenu(HMENU menu, HWND tree, HTREEITEM item) override;
    bool onItemActivate(HTREEITEM item, WPARAM keys) override;
    bool onCtrlDropFile(int index, POINT* pt, const wchar_t* path) override;
    virtual bool onNodeCommand(UINT cmd, HTREEITEM item);

    StudioFrame* m_frame;
    ClientNode*  m_client;
    HTREEITEM    m_clientItem;
    HTREEITEM    m_serverItem;
    HTREEITEM    m_sqlItem;
};
static_assert(sizeof(StudioList) == 0xF0, "StudioList size");
