#pragma once
#include <windows.h>
#include "WorkspaceNode.h"

class MsSQLserver;
class TreeList;

#pragma vtordisp(push, 2)
class SqlSvrNode : public WorkspaceNode
{
public:
    explicit SqlSvrNode(void* owner);
    virtual ~SqlSvrNode();

    const wchar_t* getString(DWORD tag) const override;
    void onCommand(int cmd, int arg) override;
    void onInitItemMenu(HMENU menu, HWND tree, HTREEITEM item) override;

    int connect(TreeList* tree, bool showDialog);
    void disconnect(TreeList* tree);

public:
    MsSQLserver* m_server;
    bool   m_connecting;
    bool   m_connected;
    LPARAM m_dbItem;
};
#pragma vtordisp(pop)
static_assert(sizeof(SqlSvrNode) == 0x2C, "SqlSvrNode size");
